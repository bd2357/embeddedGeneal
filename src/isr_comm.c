#include "isr_comm.h"
#define RET_Q_LEN 20  // more than we need



uint32_t isrReqQ_dropped_objects = 0;
uint32_t isrReqQ_errors = 0;

DefineStaticGenQ(isrRetQ, PoolQType, RET_Q_LEN);

// called in isr layer to dispose of pool objects in user space
void ReturnPoolObject(void *obj)
{
    // try to send it down to user layer
    PoolQType object = obj; // for clarity
    Status_t status = GenQ_Put(isrRetQ, &object);
    if(status != Status_OK) // if the Q is full, disable any callback and just return it to pool
    {
        isrReqQ_dropped_objects++;      // note the error
        GenPool_extract_callback(obj);  // disable the callback so it does not run unexpectedly in isr mode
        GenPool_return(obj);            // try returning blind
    }
}



// return via specific queue
STATIC void returnPoolObjectToProcess(IsrProcessTarget_t *ipt, void *obj)
{   
    PoolQType object = obj; // for clarity
    Status_t status = GenQ_Put(ipt->fromIsrQ, &object);
    if(status != Status_OK) // if reported error or FULL
    {
        ipt->droppedFrom++;
        // try to get rid of it another way.
        ReturnPoolObject(obj);
    }
}

// return the object back to user process via the isr return queue
void Ipt_ReturnToUserProcess(IsrProcessTarget_t *ipt, void *object)
{
    if(ipt->fromIsrQ)
    {
        returnPoolObjectToProcess(ipt, object);
    }
    else
    {
        ReturnPoolObject(object);
    }
    
}

Status_t Ipt_DequeueInIsr(IsrProcessTarget_t *ipt, void **object)
{
    if(ipt->toIsrQ)
    {
        return GenQ_Get(ipt->toIsrQ, object);
    }
    return Status_BadState;    
}

/* **********************************************************/
/* *********************** User Space functions  ************/
/* **********************************************************/

// Try to reset as best we can given the queues
void Ipt_Reset(IsrProcessTarget_t *ipt)
{
    if(ipt->toIsrQ)
    {
        GenQ_Reset(ipt->toIsrQ);
    }
    if(ipt->fromIsrQ)
    {
        GenQ_Reset(ipt->fromIsrQ);
    }
   
    GenPool_reset(ipt->iptPool); // this will prevent callbacks from some in general queue
    if(NULL == ipt->fromIsrQ) // if no from queue, try flushing the general queue
    {
        ProcessReturnPoolObjects(); // do cleanup now, 
    }
}

// put this in background task somewhere, this need to run all the time
void ProcessReturnPoolObjects(void)
{
    PoolQType obj; // holder
    // extract all the items in the queue and return them to the pool
    while (Status_OK == GenQ_Get(isrRetQ, &obj))
    {
        // return item to pool record any errors
        // This will trigger the attached callback if assigned
        if(Status_OK != GenPool_return(obj))
        {
            isrReqQ_errors++;
        }
    }
}

// wrapper to fix object, for clarity
STATIC Status_t sendPoolObjectToIsr(IsrProcessTarget_t *ipt, void *object)
{
    PoolQType obj = object;
    return GenQ_Put(ipt->toIsrQ, &obj);
}

// Send pool object to Isr Target
Status_t Ipt_SendToIsr(IsrProcessTarget_t *ipt, void *object)
{
    if(ipt->toIsrQ)
    {
        return sendPoolObjectToIsr(ipt, object);
    }
    return Status_BadState;
}

Status_t Ipt_ReceiveFromIsr(IsrProcessTarget_t *ipt, void **object)
{
    if(ipt->fromIsrQ)
    {
        return GenQ_Get(ipt->fromIsrQ, object);
    }
    return Status_BadState;
}

