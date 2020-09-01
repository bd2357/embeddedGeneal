/**
 * @file test_genPool.c
 * @author Bryce Deary (github@bd2357.org)
 * @brief 
 * @version 0.1
 * @date 2020-08-16
 * 
 * Copyright 2020, bdPrime
 * 
 */

#include <config.h>
#include "unity.h"
#include "genPool.h"
#include <mock_pool_support.h>

#include <stdbool.h>

typedef struct 
{
    char *str;
    int i1;
    bool flag;
} myBlk_t;

// Generate the wrappers, we can do this before we generate pool object because
// the wrappers break the dependency 
DeclareGenPoolWrappers(myPool, myBlk_t);

// test the declare macro for object functions
DeclareGenPool(secondPool, myBlk_t*);

typedef struct
{
    GenPool_t *pool;
    PreBuf_t  *pre;
    int index;
} PoolObjId_t;

#define POOL_LEN 10

Status_t myGenCallback(Context_t context)
{
    PreBufMeta_t meta = GenPool_object_meta((void*)context.v_context);
    TEST_ASSERT_EQUAL(sizeof(myBlk_t), meta.objectSize);
    TEST_ASSERT_TRUE(meta.index < POOL_LEN);
    TEST_ASSERT_EQUAL_PTR(myGenCallback, meta.onRelease.callback);
    TEST_ASSERT_EQUAL_MEMORY(&context, &(meta.onRelease.context), sizeof context);
    return Status_OK;
}


DefineStaticGenPool(myPool, myBlk_t, POOL_LEN, myGenCallback);

DefineGenPool(secondPool, myBlk_t* , POOL_LEN, myGenCallback); // second pool of pointer to first type

DefineGenPoolWrappers(myPool,myBlk_t );

void setUp(void)
{
    int avail, total;
    // since pool is build at compile time, it should always be Ok
    TEST_ASSERT_EQUAL(Status_OK, GenPool_status((GenPool_t*)myPool, &avail, &total));
    TEST_ASSERT_TRUE(avail <= POOL_LEN);
    TEST_ASSERT_EQUAL(POOL_LEN, total);
    GenPool_reset((GenPool_t*)myPool);
}

void tearDown(void)
{
    TEST_ASSERT_TRUE(POOL_LEN >= myPoolStatus());  // test wrapper version on exit
}


void test_defined_object(void)
{
    TEST_ASSERT_EQUAL_PTR(myPool, &myPool_pool);
    TEST_ASSERT_EQUAL_PTR(((GenPool_t*)myPool)->base, (void*)myPool_pool.pool);
    TEST_ASSERT_EQUAL_PTR( (void*)&(((GenPool_t*)myPool)->next->obj[0]), (void*)&(myPool_pool.pool[0].poolObject));    
}

// use some normally static internals for tests
PoolObjId_t getPoolId(void *p_obj);

void test_GenPool_allocate(void)
{
    void *obj = GenPool_allocate((GenPool_t*)myPool);
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj).pool, myPool);
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj).index, 0);
    GenPool_return(obj);
}


void test_GenPool_return(void)
{
    void *obj1 = GenPool_allocate((GenPool_t*)myPool);
    void *obj2 = GenPool_allocate((GenPool_t*)myPool);
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj1).pool, myPool);
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj1).index, 0);
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj2).pool, myPool);
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj2).index, 1);    
    TEST_ASSERT_EQUAL(Status_OK, GenPool_return(obj1) ); // return the first one
    // create a new context and replace the callback in the second object
    Context_t new_context = (Context_t){.v_context=(intptr_t)obj2};
    TEST_ASSERT_EQUAL(Status_OK, GenPool_set_return_callback(obj2, poolTestCallback, new_context ) );
    poolTestCallback_ExpectAndReturn(new_context, Status_OK); // tell mock system to expect the call
    TEST_ASSERT_EQUAL(Status_OK, GenPool_return(obj2));
}

void test_GenPool_return_alternate_callback(void)
{
    Context_t context = {0};
    myBlk_t *obj1 = GenPool_allocate_with_callback((GenPool_t*)myPool, poolTestCallback, context );
    poolTestCallback_ExpectAndReturn(context, Status_OK);  // tell mock system to expect the call
    TEST_ASSERT_EQUAL(Status_OK, GenPool_return(obj1) );
}


// Test Generated obj functions
void test_myPool_allocate(void)
{
    myBlk_t *obj = myPool_allocate(myPool);
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj).pool, myPool);
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj).index, 0);
    myPool_return(obj);
}


void test_myPool_return(void)
{
    myBlk_t *obj1 = myPool_allocate(myPool);
    myBlk_t *obj2 = myPool_allocate(myPool);
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj1).pool, myPool);
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj1).index, 0);
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj2).pool, myPool);
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj2).index, 1);    
    TEST_ASSERT_EQUAL(Status_OK, myPool_return(obj1) ); // return the first one
    // create a new context and replace the callback in the second object
    Context_t new_context = (Context_t){.v_context=(intptr_t)obj2};
    TEST_ASSERT_EQUAL(Status_OK, GenPool_set_return_callback(obj2, poolTestCallback, new_context ) );
    poolTestCallback_ExpectAndReturn(new_context, Status_OK); // tell mock system to expect the call
    TEST_ASSERT_EQUAL(Status_OK, myPool_return(obj2));
}

void test_myPool_return_alternate_callback(void)
{
    Context_t context = {0};
    myBlk_t *obj1 = myPool_allocate_with_callback(myPool, poolTestCallback, context );
    poolTestCallback_ExpectAndReturn(context, Status_OK);  // tell mock system to expect the call
    TEST_ASSERT_EQUAL(Status_OK, myPool_return(obj1) );
}


// Test generated wrapper functions
void test_myPoolAllocate(void)
{
    myBlk_t *obj = myPoolAllocate();
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj).pool, myPool);
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj).index, 0);
    myPoolReturn(obj);
}


void test_myPoolReturn(void)
{
    myBlk_t *obj1 = myPoolAllocate();
    myBlk_t *obj2 = myPoolAllocate();
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj1).pool, myPool);
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj1).index, 0);
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj2).pool, myPool);
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj2).index, 1);    
    TEST_ASSERT_EQUAL(Status_OK, myPoolReturn(obj1) ); // return the first one
    // create a new context and replace the callback in the second object
    Context_t new_context = (Context_t){.v_context=(intptr_t)obj2};
    TEST_ASSERT_EQUAL(Status_OK, GenPool_set_return_callback(obj2, poolTestCallback, new_context ) );
    poolTestCallback_ExpectAndReturn(new_context, Status_OK); // tell mock system to expect the call
    TEST_ASSERT_EQUAL(Status_OK, myPoolReturn(obj2));
}

void test_myPoolReturnAlternateCallback(void)
{
    Context_t context = {0};
    myBlk_t *obj1 = myPoolAllocateWithCallback(poolTestCallback, context );
    poolTestCallback_ExpectAndReturn(context, Status_OK);  // tell mock system to expect the call
    TEST_ASSERT_EQUAL(Status_OK, myPoolReturn(obj1) );
}


void test_status_fuctions(void)
{
    Context_t context = {0};
    myBlk_t *obj1 = GenPool_allocate_with_callback((GenPool_t*)myPool, poolTestCallback, context );
    myBlk_t *obj2 = GenPool_allocate((GenPool_t*)myPool);

    PreBufMeta_t meta1 = GenPool_object_meta(obj1);
    PreBufMeta_t meta2 = GenPool_object_meta(obj2);
    CbInstance_t cb1 = GenPool_extract_callback(obj1); // should clear cb in obj1
    CbInstance_t cb2 = GenPool_extract_callback(obj2); // should clear cb in obj2
    PreBufMeta_t meta1a = GenPool_object_meta(obj1);
    PreBufMeta_t meta2a = GenPool_object_meta(obj2);    

    CbInstance_t empty_cb = (CbInstance_t){0};

    TEST_ASSERT_EQUAL(0, meta1.index);
    TEST_ASSERT_EQUAL(1, meta2.index);
    TEST_ASSERT_EQUAL(0, meta1a.index);
    TEST_ASSERT_EQUAL(1, meta2a.index);    
    TEST_ASSERT_EQUAL(sizeof *obj1, meta1.objectSize);
    TEST_ASSERT_EQUAL(sizeof *obj2, meta2.objectSize);
    TEST_ASSERT_EQUAL(sizeof *obj1, meta1a.objectSize);
    TEST_ASSERT_EQUAL(sizeof *obj2, meta2a.objectSize);    
    TEST_ASSERT_EQUAL_MEMORY(&cb1, &meta1.onRelease, sizeof cb1);
    TEST_ASSERT_EQUAL_MEMORY(&cb2, &meta2.onRelease, sizeof cb2);
    TEST_ASSERT_EQUAL_PTR(NULL, meta1a.onRelease.callback);
    TEST_ASSERT_EQUAL_PTR(NULL, meta2a.onRelease.callback);
    TEST_ASSERT_EQUAL_PTR(cb1.callback, poolTestCallback);
    TEST_ASSERT_EQUAL_PTR(cb2.callback, myGenCallback);
    TEST_ASSERT_EQUAL(0, cb1.context.v_context);
    TEST_ASSERT_EQUAL((intptr_t)obj2, cb2.context.v_context);
    TEST_ASSERT_EQUAL(0, meta1a.onRelease.context.v_context);
    TEST_ASSERT_EQUAL((intptr_t)obj2, meta2a.onRelease.context.v_context);    

    TEST_ASSERT_EQUAL(Status_OK, GenPool_return(obj1) );
    TEST_ASSERT_EQUAL(Status_Unexpected, GenPool_return(obj1) );    
    TEST_ASSERT_EQUAL(Status_OK, GenPool_return(obj2) );
    TEST_ASSERT_EQUAL(Status_Unexpected, GenPool_return(obj2) );   

    meta1 = GenPool_object_meta(obj1);  // should return 0 filled
    TEST_ASSERT_EQUAL(0, meta1.objectSize);
    meta2 = GenPool_object_meta(obj2);  // should return 0 filled
    TEST_ASSERT_EQUAL(0, meta2.objectSize);
    cb1 = GenPool_extract_callback(obj1); // should return 0
    TEST_ASSERT_EQUAL_MEMORY(&empty_cb, &cb1, sizeof empty_cb);
    cb2 = GenPool_extract_callback(obj2); // should return 0
    TEST_ASSERT_EQUAL_MEMORY(&empty_cb, &cb2, sizeof empty_cb);
}