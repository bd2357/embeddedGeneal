/**
 * @file genQ.c
 * @author Bryce Deary (github@bd2357.org)
 * @brief General Q function, turn any array of objects into a Q
 * @date 2020-08-10
 * 
 * @details Simple single ended queue with separate reader and writer members to
 * allow fast access without disabling interrupts.
 * 
 * 
 * Copyright 2020, bdPrime
 * 
 */
#include <config.h>
#include "genQ.h"
#include <string.h>








/**
 * @brief inline code to do circular buffer pointer wrap check
 */
#define NEXTINQ(val) \
((val) >= q->end ? q->base_add : (void*)((val) + q->objectSize))

Status_t GenQ_Init(GenQ_t *q, void *buffer, uint16_t objectSize, uint16_t totalObjects)
{
    if (!q || totalObjects < 2) return Status_Param;
    *(void**)(&(q->base_add)) = buffer;
    q->next = buffer;  // modified only by source
    q->last = buffer;  // modified only by consumer
    *(uint8_t**)(&(q->end)) = (uint8_t*)buffer + objectSize * (totalObjects-1);
    *(size_t*)(&(q->objectSize)) = objectSize;
    return Status_OK;
}

void GenQ_Reset(GenQ_t *q)
{
    q->last = q->next;
}

// always leaves one empty space, so putter only modifies next
// and getter only modifies last
Status_t GenQ_Put(GenQ_t *q, void const *obj)
{
    if (!q) return Status_Param;
    uint8_t *next = NEXTINQ(q->next);
    if (next ==  q->last) return Status_FULL;
    memcpy(q->next, obj, q->objectSize);
    q->next = next;
    return Status_OK;
}

// Queue has data when next != last
Status_t GenQ_Get(GenQ_t *q, void *obj)
{
    if (!q) return Status_Param;
    if (q->next == q->last) return Status_EMPTY;
    memcpy(obj, q->last, q->objectSize);
    q->last = NEXTINQ(q->last);
    return Status_OK;    
}

bool GenQ_HasData(GenQ_t *q)
{
    return q && (q->next != q->last);
}

bool GenQ_HasSpace(GenQ_t *q)
{
    return q && (q->last != NEXTINQ(q->next) );
}

size_t GenQ_ObjectSize(GenQ_t *q)
{
    return q->objectSize;
}