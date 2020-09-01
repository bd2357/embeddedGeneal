#include <mock_config.h>
#include "unity.h"
#include "isr_helper.h"
#include <mock_isr_abstraction.h> // MOck out the isr protection functions

void setUp(void)
{
}

void tearDown(void)
{
}

void test_isr_helper_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement isr_helper");
}

Status_t my_function(Context_t context)
{
    TEST_ASSERT_EQUAL_PTR(my_function, (GenCallback_t)context.f_context);
    return Status_OK;
}

void test_Run_Protected(void)
{
    Isr_Disable_ExpectAndReturn(0xAA);
    Isr_Enable_Expect(0xAA);
    TEST_ASSERT_EQUAL(Status_OK, 
        Run_Protected((CbInstance_t){
            .callback=my_function,
            .context.f_context=my_function}));
}

void test_Run_Privileged(void)
{
    Isr_Mask_ExpectAndReturn(0X55,0xAA);
    Isr_Restore_Expect(0xAA);

    TEST_ASSERT_EQUAL(Status_OK, 
        Run_Privileged(0x55, (CbInstance_t){
            .callback=my_function,
            .context.f_context=my_function}));
}