
#include "unity.h"
#include <mock_config.h>

#include "set_run_isr.h"
#include <genQ.h>
#include <genPool.h>
#include <isr_comm.h>

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

void ProcessUser(void)
{
    int avail, total;
    GenPool_status(futureHolders, &avail, &total);
    if(avail == total) system_run = false;
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