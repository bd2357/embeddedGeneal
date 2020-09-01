/**
 * @file genPool.c
 * @author Bryce Deary (github@bd2357.org)
 * @brief functions implimenting the general Pool
 * @version 0.1
 * @date 2020-08-10
 * 
 * Copyright 2020, bdPrime
 * 
 */
#include <config.h>
#include "genPool.h"

#include <stdlib.h>
#include <assert.h>


typedef struct
{
    GenPool_t *pool;
    PreBuf_t  *pre;
    int index;
} PoolObjId_t;

STATIC PoolObjId_t getPoolId(void *buf);

void GenPool_reset(GenPool_t *self)
{
    if (self && offsetof(GenPool_t, base)+(uintptr_t)self == (uintptr_t)(self->base))
    {
        int index = 0;
        PreBuf_t *next = self->base;
        while (next <= self->end)
        {
            next->theboss = 0;
            next->guard = index ? 0: 1;
            index++;
            next = (PreBuf_t*)( (uintptr_t)(self->base) + self->cellSize * index );
        }
        self->next = self->base;
    }
}

// given the user's object pointer, work back to find if it
// belongs to a pool and if it does return the meta data
STATIC PoolObjId_t getPoolId(void *p_obj)
{
    PoolObjId_t id;
    const int offset = offsetof(PreBuf_t, obj); // address adjustment
    id.pre = (PreBuf_t*)((intptr_t)p_obj - offset); // possible address of PreBuf
    if (id.pre->theboss && id.pre->guard) // if guard vars are not zero
    {   // theboss is offset back in 32bit words, guard is offset forward as pool index + 1
        id.pool = (GenPool_t*)(((intptr_t)id.pre) - ((int32_t)(id.pre->theboss) * 4));
        int delta = (intptr_t)id.pre - (intptr_t)id.pool->base;
        div_t ans = div(delta, id.pool->cellSize); // 
        if (ans.rem == 0 && ans.quot == id.pre->guard-1)
        {
            id.index = ans.quot;
            return id;
        }
    }
    return (PoolObjId_t){0}; // Not an allocated pool object
}


PreBufMeta_t GenPool_object_meta(void *obj)
{
    PoolObjId_t id = getPoolId(obj);
    if (id.pool != NULL)
        return (PreBufMeta_t){
            .onRelease=id.pre->onRelease,
            .index=id.index,
            .objectSize=id.pool->objectSize
            };
    else
        return (PreBufMeta_t){
            .objectSize=0
            };
}

// simple allocation with no additional callback, sets the default callback if one was
// included when defined at compile time.
void *GenPool_allocate(GenPool_t *self)
{
    return GenPool_allocate_with_callback(self, self->onRelease, (Context_t){.v_context=-1});
}

// allocate a pool object and attach an explicit optional callback
void *GenPool_allocate_with_callback(GenPool_t *self, GenCallback_t cb, Context_t context)
{
    PreBuf_t *next = self->next;
    int index = next->guard-1;
    int first = index;
    
    while(next->theboss)
    {
        if(next >= self->end)
        {
            next = (PreBuf_t*)self->base;
            index = 0;
        }
        else
        {
            next = (PreBuf_t*)( (intptr_t)next + self->cellSize );
            index++;
        }
        // if we wrapped around. give up
        if (index == first)
        {
            return NULL;
        }
    }
    next->theboss++; // claim it
    next->guard = index+1; // make sure guard is set correctly
    intptr_t delta = (intptr_t)next - (intptr_t)self;
    assert((delta & 0x3) == 0 );  // divisible by 4
    self->next = next; // save where we where
    next->onRelease.callback = cb;
    // if -1 then use this object as context
    next->onRelease.context = (context.v_context==-1)?(Context_t){.v_context=(intptr_t)next->obj}: context;
    next->theboss = (uint16_t)(delta >> 2); // mark the field for recovery(validates the allocation)
    return next->obj; // return the object
}

// return the object to the pool, execute enclosed callback if set
Status_t GenPool_return(void *obj)
{
    Status_t status = Status_OK;
    PoolObjId_t id = getPoolId(obj);
    if (id.pool != NULL)
    {
        // before returning buffer see if there is a callback
        if (id.pre->onRelease.callback)
        {
            status = id.pre->onRelease.callback(id.pre->onRelease.context);
            // if callback returns Interrupt it means the callback reused the
            // pool object, so don't return it
            if (status == Status_Interrupt)
            {
                return Status_OK;
            }
            else
            {
                id.pre->onRelease.callback=NULL;  // safe the mete data
            }
        }
        // setup for instant reuse
        id.pool->next = id.pre;
        id.pre->theboss=0; // this releases the object back to the pool
    }    
    else
    {
        status = Status_Unexpected;
    }
    return status;
}

// extract any callback from object, (disables it in the obj)
CbInstance_t GenPool_extract_callback(void *obj)
{
    PoolObjId_t id = getPoolId(obj);
    CbInstance_t cb = {0};
    if (id.pool != NULL)
    {
        cb = id.pre->onRelease; // get copy
        id.pre->onRelease.callback = NULL; // disable callback in obj
    }    
    return cb;
}

// set or update the on return callback
Status_t GenPool_set_return_callback(void *obj, GenCallback_t cb, Context_t context)
{
    PoolObjId_t id = getPoolId(obj);
    if (id.pool != NULL)
    {    
        id.pre->onRelease.context = context;
        id.pre->onRelease.callback = cb;
        return Status_OK;
    }
    return Status_Unexpected;
}


// Not expected to use in production, checks the status of a pool
// and returns available slots.
Status_t GenPool_status(GenPool_t const *self, int *availale, int *total)
{
    int unclaimed = 0;

    if(self->next <= self->end && self->next >= self->base)
    {
        int guard = 1; // is array index + 1
        PreBuf_t const *next = self->base;
        while (next <= self->end)
        {
            if (0 == next->guard || guard == next->guard)
            {
                if(0 == next->theboss) 
                {
                    unclaimed++;
                }
                else
                {
                    uint16_t boss = (uint16_t)(((intptr_t)next - (intptr_t)self) >> 2);
                    if (boss != next->theboss)
                    {
                        return Status_BadState;
                    }
                }
            } 
            else // guard was wrong
            {
                return Status_BadState;
            }

            next = (PreBuf_t*)( (uintptr_t)(self->base) + self->cellSize * guard );
            guard++;
        }
        if(availale) *availale = unclaimed;
        if(total) *total = guard-1;
        return Status_OK;
    } 
    // pointer are messedup
    return Status_BadState;
}

