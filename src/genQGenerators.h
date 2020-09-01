/**
 * @file genQGenerators.h
 * @author Bryce Deary (github@bd2357.org)
 * @brief 
 * @version 0.1
 * @date 2020-08-22
 * 
 * Copyright 2020, bdPrime
 * 
 */

#ifndef _GENQGENERATORS_H_
#define _GENQGENERATORS_H_


/**
 * @brief macro for compile time definition of GenQ structure
 *  given an existing storage pointer of known type and length
 */
#define DefineGenQFromStore(name, store, len) \
enum {store##_len=len}; \
GenQ_t name##_instance = (GenQ_t){ \
    .base_add=store, \
    .next=(uint8_t*)store, \
    .last=(uint8_t*)store, \
    .end=(uint8_t*)&store[store##_len - 1], \
    .objectSize=sizeof store[0] }; \
GenQ_t * const name = &name##_instance    


/**
 * @brief macro for defining a static queue of given type and length
 * 
 */
#define DefineGenQ(name, type, len) \
enum {name##_len = len }; \
type name##_space[name##_len+1] = {0}; \
GenQ_t name##_instance = (GenQ_t){ \
    .base_add=name##_space, \
    .next=(uint8_t*)name##_space, \
    .last=(uint8_t*)name##_space, \
    .end=(uint8_t*)&name##_space[name##_len], \
    .objectSize=sizeof(type) }; \
STATIC GenQ_t * const name = &name##_instance

/**
 * @brief macro for defining a static queue of given type and length
 * 
 */
#define DefineStaticGenQ(name, type, len) \
enum {name##_len = len }; \
STATIC type name##_space[name##_len+1] = {0}; \
STATIC GenQ_t name##_instance = (GenQ_t){ \
    .base_add=name##_space, \
    .next=(uint8_t*)name##_space, \
    .last=(uint8_t*)name##_space, \
    .end=(uint8_t*)&name##_space[name##_len], \
    .objectSize=sizeof(type) }; \
STATIC GenQ_t * const name = &name##_instance

/**
 * @brief define typesafe callers for a CLASS of genQ
 */
#define DefineTypeSafeGenQMethods(name, type) \
void name##_reset(GenQ_t* self){GenQ_Reset(self);} \
Status_t name##_put(GenQ_t* self, type const *obj) {return GenQ_Put(self, obj);} \
Status_t name##_get(GenQ_t* self, type *obj) {GenQ_Get(self, obj);} \
bool name##_hasData(GenQ_t* self)  {return GenQ_HasData(self);}\
bool name##_hasSpace(GenQ_t* self) {return GenQ_HasSpace(self);}

/**
 * @brief Define typesafe wrappers for the named queue
 */
#define DefineGenQWrappers(name, type) \
void name##Reset(void){GenQ_Reset(name);} \
Status_t name##Put(type const *obj) {return GenQ_Put(name, obj);} \
Status_t name##Get(type *obj) {GenQ_Get(name, obj);} \
bool name##HasData(void)  {return GenQ_HasData(name);}\
bool name##HasSpace(void) {return GenQ_HasSpace(name);}
/**
 * @brief Declare typesafe wrappers for the named queue
 */
#define DeclareGenQWrappers(name, type) \
void name##Reset(void); \
Status_t name##Put(type const *obj); \
Status_t name##Get(type *obj); \
bool name##HasData(void); \
bool name##HasSpace(void)


#endif