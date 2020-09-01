/**
 * @file linkNode.c
 * @author Bryce Deary (github@bd2357.org)
 * @brief generic linked list type functions that operate
 *   on the LinkBase_t base class
 * 
 * Copyright 2020, bdPrime
 * 
 */
#include "linkNode.h"

// classic base class list
LinkBase_t *StackPop(LinkBase_t **head)
{
    LinkBase_t * next = *head;
    if(next) // if head was not null
    {
        *head = next->next; // update head to new head
    }
    return next; // return old head, which could be null
}

void StackPush(LinkBase_t **head, LinkBase_t *node)
{
    LinkBase_t *next = *head;
    if(next) // if something on stack
    {
        node->next = next;
    }
    else
    {
        node->next = NULL;
    }
    *head = node;
}