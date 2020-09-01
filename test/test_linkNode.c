#include "unity.h"

#include "linkNode.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_linkNode_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement linkNode");
}

typedef struct
{
    LinkBase_t lnk;
    char *str;
    int i;
} MyThing;


void test_stack(void)
{
    MyThing items[10];
    LinkBase_t *q = NULL;
    

    for (int i=0; i<DIM(items); i++)
    {
        items[i].i = i;
        items[i].str = "Hello";
        StackPush(&q, &items[i].lnk);
    }

    int cnt = DIM(items)-1;
    LinkBase_t *lnk;
    LinkBase_t *q2;
    while(lnk = StackPop(&q))
    {
        MyThing *item = (MyThing*)lnk;
        TEST_ASSERT_EQUAL(cnt, item->i);
        TEST_ASSERT_EQUAL_STRING("Hello", item->str);
        cnt--;
        StackPush(&q2, lnk);
    }
    TEST_ASSERT_EQUAL_PTR(q2, &items[0] );
}
