
#include "unity.h"
#include <mock_config.h>

#include "set_run_isr.h"
#include <genQ.h>
#include <genPool.h>
#include <isr_comm.h>
#include <linkNode.h>

void setUp(void)
{
    time_tick=0;
    system_run=true;
}

void tearDown(void)
{
}

void test_set_run_isr_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement set_run_isr");
}

extern GenPool_t *futureHolders;
extern GenPool_t *taskHolders;
extern GenQ_t * const taskQs[2];
extern int taskQ;

void ProcessUser(void)
{
    int avail, total;
    int avail2, total2;
    GenPool_status(futureHolders, &avail, &total);
    GenPool_status(taskHolders, &avail2, &total2);
    if(avail == total &&
       avail2== total2 ) system_run = false;
}

Status_t shut_down(Context_t context)
{
    if(time_tick > 200)
    {
        printf("All Done %i\n", time_tick);
        return Status_Interrupt;
    }
    else
    {
        printf("Cb called %i\n", time_tick);
    }
    return Status_OK;
}

Status_t simple_task(Context_t context)
{
    if(time_tick > 41)
    {
        printf("Task Done %i\n", time_tick);
        return Status_Interrupt;
    }
    if(time_tick == 0)
    {
        printf("Task Start %i\n", time_tick);
    }
    

    return Status_OK;
}

void test_timer_once(void)
{
    TEST_ASSERT_EQUAL(Status_OK, Run_Later((CbInstance_t){.callback=shut_down}, 200 )); 

    User_Loop();

}

void test_timer_multi(void)
{
    TEST_ASSERT_EQUAL(Status_OK, Run_Periodically((CbInstance_t){.callback=shut_down}, -50 )); 

    User_Loop();
}

void test_task_once(void)
{
    TEST_ASSERT_EQUAL(Status_OK, Run_Task_Once((CbInstance_t){.callback=simple_task})); 

    User_Loop();
}

void test_task_repeating(void)
{
    TEST_ASSERT_EQUAL(Status_OK, Run_Task((CbInstance_t){.callback=simple_task})); 

    User_Loop();
}