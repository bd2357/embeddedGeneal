#include "unity.h"
#include "taskService.h"
#include "generalQueue.h"
#include "gencmdef.h"
#include "taskServiceClass.h"
#include <malloc.h>
#include <string.h>

uint32_t space[100];
extern taskMasterRecord_t *tmr;

int runs[6];
int data[6];

void setUp(void)
{
    memset(runs,0,sizeof(runs));
}

int testVal;

void tearDown(void)
{
    
}


void task1(void)
{
    runs[0]++;
    TS_Get( &data[0]);
}

void task2(void)
{
    runs[1]++;
    TS_Get( &data[1]);
}

void task3(void)
{
    runs[2]++;
    TS_Get( &data[2]);    
}

void task4(void)
{
    runs[3]++;
    TS_Get( &data[3]);    
}

void task5(void)
{
    runs[4]++;
    TS_Get( &data[4]);    
}

void task6(void)
{
    runs[5]++;
    TS_Get( &data[5]);    
}

taskFunc_f taskTable[] =
{
    task1, 
    task2,
    task3,
    task4,
    task5,
    task6
};

void test_taskService_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement taskService");
}


void test_taskService_Init(void)
{
    int tasks;
    taskHandle_t task[100];
    TEST_ASSERT_EQUAL(0, TS_Init(space, 5));
    TEST_ASSERT_NULL(tmr);
    TEST_ASSERT_NOT_EQUAL(0,tasks = TS_Init(space, sizeof(space)));
    TEST_ASSERT_EQUAL(space,tmr);
    int i=0;
    for (i=0; i< tasks-5; i++)
    {
        TEST_ASSERT_NOT_EQUAL(0, task[i] = TS_AddTask(task1));
    }

    QBUILDER_STATIC(q1,uint8_t, 10);
    TEST_ASSERT_NOT_EQUAL(0, task[i++] = TS_AddTaskWithQueue(task2,q1));
    QBUILDER_STATIC(q2,uint8_t, 10);
    TEST_ASSERT_NOT_EQUAL(0, task[i++] = TS_AddTaskWithQueue(task2,q2));
    QBUILDER_STATIC(q3,uint8_t, 10);
    TEST_ASSERT_NOT_EQUAL(0, task[i++] = TS_AddTaskWithQueue(task2,q3));    
    QBUILDER_STATIC(q4,uint8_t, 10);
    TEST_ASSERT_NOT_EQUAL(0, task[i++] = TS_AddTaskWithQueue(task2,q4));    
    QBUILDER_STATIC(q5,uint8_t, 10);
    TEST_ASSERT_NOT_EQUAL(0, task[i++] = TS_AddTaskWithQueue(task2,q5));    


    // verify we can't add anymore
    TEST_ASSERT_EQUAL(TSC_TableFull, TS_AddTask(task1));

    i--;
    int run = 0;
    while(run)
    {
        TS_Background();
    }
    
}


void test_taskService_signal(void)
{
    int tasks;
    taskHandle_t task[100];
    TEST_ASSERT_EQUAL(0, TS_Init(space, 5));
    TEST_ASSERT_NULL(tmr);
    TEST_ASSERT_NOT_EQUAL(0,tasks = TS_Init(space, sizeof(space)));
    TEST_ASSERT_EQUAL(space,tmr);
    for (int i=0; i<DIM(taskTable); i++)
    {
        TEST_ASSERT_NOT_EQUAL(0, task[i] = TS_AddTask(taskTable[i]));
    }


    TS_Background();
    TEST_ASSERT_EQUAL(0,runs[0]);
    TEST_ASSERT_EQUAL(0,runs[1]);
    TEST_ASSERT_EQUAL(0,data[1]);    
    
    TS_SignalTask(task[1]);
    TS_Background();
    TEST_ASSERT_EQUAL(0,runs[0]);
    TEST_ASSERT_EQUAL(1,runs[1]);    
    TEST_ASSERT_EQUAL(0,data[1]);     

    
}


void test_taskService_queue(void)
{
    int tasks;
    taskHandle_t task[100];
    TEST_ASSERT_EQUAL(0, TS_Init(space, 5));
    TEST_ASSERT_NULL(tmr);
    TEST_ASSERT_NOT_EQUAL(0,tasks = TS_Init(space, sizeof(space)));
    TEST_ASSERT_EQUAL(space,tmr);
    genQ_t *q = alloca(sizeof(genQ_t) * 6);
    int *buf = alloca(sizeof(int) * 10 * 6);
    for (int j=0; j< 6; j++)
    {
        GenQ_Init(&q[j], &buf[j*10], sizeof(int), 10);
    }    
    
    
    TEST_ASSERT_NOT_EQUAL(0, task[0] = TS_AddTaskWithQueue(task1, &q[0]));
    TEST_ASSERT_NOT_EQUAL(0, task[1] = TS_AddTaskWithQueue(task2, &q[1]));
    TEST_ASSERT_NOT_EQUAL(0, task[2] = TS_AddTaskWithQueue(task3, &q[2]));
    TEST_ASSERT_NOT_EQUAL(0, task[3] = TS_AddTaskWithQueue(task4, &q[3]));
    TEST_ASSERT_NOT_EQUAL(0, task[4] = TS_AddTaskWithQueue(task5, &q[4]));
    TEST_ASSERT_NOT_EQUAL(0, task[5] = TS_AddTaskWithQueue(task6, &q[5]));   

    TS_Background();
    TEST_ASSERT_EQUAL(0,runs[0]);
    TEST_ASSERT_EQUAL(0,runs[1]);
    TEST_ASSERT_EQUAL(0,data[1]);    
    
    TS_SignalTask(task[1]);
    TS_Background();
    TEST_ASSERT_EQUAL(0,runs[0]);
    TEST_ASSERT_EQUAL(1,runs[1]);    
    TEST_ASSERT_EQUAL(0,data[1]);   
    
    int testVal =23;
    TS_SignalTask(task[1]);
    TS_Put(task[2], &testVal);
    TS_Background();
    TEST_ASSERT_EQUAL(0,runs[0]);
    TEST_ASSERT_EQUAL(2,runs[1]);    
    TEST_ASSERT_EQUAL(0,data[1]);  
    TEST_ASSERT_EQUAL(23,data[2]);  

}

taskHandle_t secondTask;
taskHandle_t yieldTask;

// priority 1 task
void secondTest(void)
{
    int testVal;
    if (0 == TS_Get(&testVal))
    {
        if(testVal++ == 4)  // if we got a 4, send a 5 to yieldTask
         TS_Put(yieldTask, &testVal);
    }    
    
}

// priority 2 task
void yieldTest(void)
{
    int testVal;
    if (0 == TS_Get(&testVal))
    {
        TEST_ASSERT_EQUAL(2, testVal);
        testVal = 4;
        TS_Put(secondTask, &testVal);
    }
    TEST_ASSERT_EQUAL(0, TS_Test());
    TS_Yield();
    TEST_ASSERT_EQUAL(1, TS_Test());
    TEST_ASSERT_EQUAL(0,TS_Get(&testVal));
    TEST_ASSERT_EQUAL(5,testVal);
}

void test_taskService_yield(void)
{
    int tasks;
    taskHandle_t task[100];
    TEST_ASSERT_NOT_EQUAL(0,tasks = TS_Init(space, sizeof(space)));
    TEST_ASSERT_EQUAL(space,tmr);
    genQ_t *q = alloca(sizeof(genQ_t) * 2);
    int *buf = alloca(sizeof(int) * 10 * 2);
    GenQ_Init(&q[0], &buf[0*10], sizeof(int), 10);
    GenQ_Init(&q[1], &buf[1*10], sizeof(int), 10);
    TEST_ASSERT_NOT_EQUAL(0, task[0] = TS_AddTask(task1));
    TEST_ASSERT_NOT_EQUAL(0,secondTask = TS_AddTaskWithQueue(secondTest, &q[0]));
    TEST_ASSERT_NOT_EQUAL(0,yieldTask = TS_AddTaskWithQueue(yieldTest, &q[1]));
    
    TS_Background();
    int testval = 2;
    TS_Put(yieldTask, &testval);
    TS_Background();    

}