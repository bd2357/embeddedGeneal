/**
 * @file test_isr_comm.c
 * @author Bryce Deary (github@bd2357.org)
 * @brief Test the comm functions for sending data between isr and user layers
 * @date 2020-08-16
 * 
 * Copyright 2020, bdPrime
 * 
 */
#include "unity.h"

#include "isr_comm.h"
#include <genQ.h>
#include <genPool.h>
#include <mock_pool_support.h>

void setUp(void)
{
}

void tearDown(void)
{
}

void test_isr_comm_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement isr_comm");
}

typedef struct 
{   
    char *str;
    uint8_t flag;
    int32_t i1;
} myMes;


DefineSingleIsrObjectQueue(Isr1Pipe, myMes, 5, poolTestCallback );


void test_GenerationSingle(void)
{
    int avail, total;
    Ipt_Reset(Isr1Pipe);    
    // peer into the pool object
    TEST_ASSERT_EQUAL(Status_OK, GenPool_status((GenPool_t*)PIsr1Pipe, &avail, &total) );
    TEST_ASSERT_EQUAL(5,avail);
    TEST_ASSERT_EQUAL(5,total);
    myMes *mes = Allocate_Isr1Pipe_Object();        // get object from pool
    mes->str = "hello world";
    mes->flag = 1;
    mes->i1 = 42;
    TEST_ASSERT_NOT_NULL(mes);
    TEST_ASSERT_EQUAL(Status_OK, GenPool_status((GenPool_t*)PIsr1Pipe, &avail, &total) );
    TEST_ASSERT_EQUAL(4,avail);
    TEST_ASSERT_EQUAL(5,total);    
    myMes *mesIsr;
    TEST_ASSERT_EQUAL(Status_OK, Send_Isr1Pipe_Object(mes));  // send object to isr
    TEST_ASSERT_EQUAL(Status_OK, Dequeue_Isr1Pipe_Object(&mesIsr));  // dequeue and object in isr    
    TEST_ASSERT_EQUAL_PTR(mesIsr, mes);
    TEST_ASSERT_EQUAL(42, mesIsr->i1);

    Free_Isr1Pipe_Object(mesIsr);  // used by isr to free pool object
    TEST_ASSERT_EQUAL(Status_OK, GenPool_status((GenPool_t*)PIsr1Pipe, &avail, &total) );
    TEST_ASSERT_EQUAL(4,avail);
    TEST_ASSERT_EQUAL(5,total);
    // calling ProcessReturnPoolObjects will free all items in queue and call their 
    // callback functions which we pointed at a mocked function.
    poolTestCallback_ExpectAndReturn((Context_t){.v_context=(intptr_t)mes}, Status_OK);
    ProcessReturnPoolObjects();
    TEST_ASSERT_EQUAL(Status_OK, GenPool_status((GenPool_t*)PIsr1Pipe, &avail, &total) );
    TEST_ASSERT_EQUAL(5,avail);
    TEST_ASSERT_EQUAL(5,total);

}

typedef struct 
{
    float d1;
    int index;
    char *str;
} aThing;

DefineDoubleIsrObjectQueue(Isr2Pipe, aThing, 7, poolTestCallback );


void test_GenerationDouble(void)
{
    int avail, total;
    Ipt_Reset(Isr2Pipe);
    // peer into the pool object
    TEST_ASSERT_EQUAL(Status_OK, GenPool_status((GenPool_t*)PIsr2Pipe, &avail, &total) );
    TEST_ASSERT_EQUAL(7,avail);
    TEST_ASSERT_EQUAL(7,total);
    aThing *mes = Allocate_Isr2Pipe_Object();        // get object from pool
    mes->str = "hello world";
    mes->d1 = 3.14159;

    TEST_ASSERT_NOT_NULL(mes);
    TEST_ASSERT_EQUAL(Status_OK, GenPool_status((GenPool_t*)PIsr2Pipe, &avail, &total) );
    TEST_ASSERT_EQUAL(6,avail);
    TEST_ASSERT_EQUAL(7,total);    
    aThing *mesIsr;
    TEST_ASSERT_EQUAL(Status_OK, Send_Isr2Pipe_Object(mes));  // send object to isr
    TEST_ASSERT_EQUAL(Status_OK, Dequeue_Isr2Pipe_Object(&mesIsr));  // dequeue and object in isr    
    TEST_ASSERT_EQUAL_PTR(mesIsr, mes);
    TEST_ASSERT_EQUAL_FLOAT(3.14159, mesIsr->d1);

    Return_Isr2Pipe_Object(mesIsr);  // used by isr to free pool object
    TEST_ASSERT_EQUAL(Status_OK, GenPool_status((GenPool_t*)PIsr2Pipe, &avail, &total) );
    TEST_ASSERT_EQUAL(6,avail);
    TEST_ASSERT_EQUAL(7,total);
    aThing *mesRet;    
    TEST_ASSERT_EQUAL(Status_OK, Receive_Isr2Pipe_Object(&mesRet));
    TEST_ASSERT_EQUAL_PTR(mesRet, mes);
    TEST_ASSERT_EQUAL_FLOAT(3.14159, mesRet->d1);    
    TEST_ASSERT_EQUAL_STRING("hello world", mesRet->str);

    // calling ProcessReturnPoolObjects will free all items in queue and call their 
    // callback functions which we pointed at a mocked function.
    poolTestCallback_ExpectAndReturn((Context_t){.v_context=(intptr_t)mes}, Status_OK);
    TEST_ASSERT_EQUAL(Status_OK, PoolReturn_Isr2Pipe_Object(mesRet));
    TEST_ASSERT_EQUAL(Status_OK, GenPool_status((GenPool_t*)PIsr2Pipe, &avail, &total) );
    TEST_ASSERT_EQUAL(7,avail);
    TEST_ASSERT_EQUAL(7,total);
}


void isrFunction(void)
{
    // presumable entered on an event
    aThing *mesIsr;
    Status_t status = Dequeue_Isr2Pipe_Object(&mesIsr);
    int index = mesIsr->index;
    Return_Isr2Pipe_Object(mesIsr);
    while(Status_OK == (status = Dequeue_Isr2Pipe_Object(&mesIsr)))
    {
        index++;
        TEST_ASSERT_EQUAL_MESSAGE(index, mesIsr->index, "message index should increase");
        mesIsr->d1 = mesIsr->index + 1;
        Return_Isr2Pipe_Object(mesIsr);
    }
    TEST_ASSERT_EQUAL(Status_EMPTY, status);
}


void userFunction1(void)
{
    int counter = 0;
    aThing *mes;      // get object from pool
    while(NULL != (mes = Allocate_Isr2Pipe_Object()))        // get object from pool
    {
        counter++;
        mes->str = "hello world";
        mes->d1 = 3.14159;
        mes->index = counter;
        TEST_ASSERT_EQUAL(Status_OK, Send_Isr2Pipe_Object(mes));
    }
    // got here, the pool is empty
}

void userFunction2(void)
{
    aThing *mesRet;      // get object from pool
    Status_t status = Receive_Isr2Pipe_Object(&mesRet);
    TEST_ASSERT_EQUAL(Status_OK, status); 
    int counter = mesRet->index;
    poolTestCallback_ExpectAndReturn((Context_t){.v_context=(intptr_t)mesRet}, Status_OK);      
    TEST_ASSERT_EQUAL(Status_OK, PoolReturn_Isr2Pipe_Object(mesRet));    
    while(Status_OK == (status = Receive_Isr2Pipe_Object(&mesRet)))
    {
        counter++;
        TEST_ASSERT_EQUAL_MESSAGE(counter, mesRet->index, "counter should match index");
        TEST_ASSERT_EQUAL_FLOAT((float)counter+1, mesRet->d1); 
        poolTestCallback_ExpectAndReturn((Context_t){.v_context=(intptr_t)mesRet}, Status_OK);        
        TEST_ASSERT_EQUAL(Status_OK, PoolReturn_Isr2Pipe_Object(mesRet));  
    }
    TEST_ASSERT_EQUAL(Status_EMPTY, status);

}

void test_sequence(void)
{
    userFunction1();
    isrFunction();
    userFunction2();
}