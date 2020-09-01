/**
 * @file isr_comm_generators.h
 * @author Bryce Deary (github@bd2357.org)
 * @brief 
 * @version 0.1
 * @date 2020-08-22
 * 
 * Copyright 2020, bdPrime
 * 
 */

#ifndef _ISR_COMM_GENERATORS_H_
#define _ISR_COMM_GENERATORS_H_

/**
 * @brief Helper macros to auto generate user/isr queue prototypes
 * 
 */
#define DeclareSingleIsrObjectQueue(name, type) \
type *Allocate_##name##_Object(void); \
Status_t Send_##name##_Object(type *obj); \
Status_t Dequeue_##name##_Object(type **obj); \
void Free_##name##_Object(type *obj)

#define DeclareDoubleIsrObjectQueue(name, type) \
type *Allocate_##name##_Object(void); \
Status_t Send_#name##_Object(type *obj); \
Status_t Dequeue_##name##_Object(type **obj); \
void Return_##name##_Object(type *obj); \
Status_t Receive_##name##_Object(type **obj); \
Status_t PoolReturn_##name##_Object(type *obj)

/**
 * @brief helper macros to autogenerate and initialize user/isr queues
 * @details A Single is designed for Isr that are transmit only
 */
#define DefineSingleIsrObjectQueue(name, type, depth, complete) \
/* Define a pool of communication types */ \
DefineStaticGenPool(P##name, type, depth, complete); \
/* */ \
DefineStaticGenQ(name##_toQ, type*, depth); \
STATIC IsrProcessTarget_t name##_instance = (IsrProcessTarget_t){ \
    .iptPool = (GenPool_t*)&P##name##_pool, \
    .toIsrQ = &name##_toQ_instance, \
    .droppedTo = 0, \
}; \
IsrProcessTarget_t *name = &name##_instance; \
/* Single Q functions */ \
type *Allocate_##name##_Object(void) \
{ return (type*) GenPool_allocate(name->iptPool); } \
Status_t Send_##name##_Object(type *obj) \
{ return Ipt_SendToIsr(name, obj); } \
Status_t Dequeue_##name##_Object(type **obj) \
{ return Ipt_DequeueInIsr(name, (void**)obj); } \
void Free_##name##_Object(type *obj) \
{ Ipt_ReturnToUserProcess(name, obj); }


#define DefineDoubleIsrObjectQueue(name, type, depth, complete) \
DefineStaticGenPool(P##name, type, depth, complete); \
DefineStaticGenQ(name##_toQ, type*, depth); \
DefineStaticGenQ(name##_fromQ, type*, depth); \
IsrProcessTarget_t name##_instance = (IsrProcessTarget_t){ \
    .iptPool = (GenPool_t*)&P##name##_pool, \
    .toIsrQ = &name##_toQ_instance, \
    .droppedTo = 0, \
    .fromIsrQ = &name##_fromQ_instance, \
    .droppedFrom = 0 \
}; \
IsrProcessTarget_t * const name = &name##_instance; \
/* Double Q functions */ \
type *Allocate_##name##_Object(void) \
{ return (type*) GenPool_allocate(name->iptPool); } \
Status_t Send_##name##_Object(type *obj) \
{ return Ipt_SendToIsr(name, obj); } \
void Return_##name##_Object(type *obj) \
{ Ipt_ReturnToUserProcess(name, obj);} \
Status_t Dequeue_##name##_Object(type **obj) \
{ return Ipt_DequeueInIsr(name, (void**)obj); } \
Status_t Receive_##name##_Object(type **obj) \
{ return Ipt_ReceiveFromIsr(name, (void**)obj); }\
Status_t PoolReturn_##name##_Object(type *obj) \
{ return GenPool_return(obj); }


#endif