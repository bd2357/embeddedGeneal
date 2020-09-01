/**
 * @file genPool.h
 * @author Bryce Deary (github@bd2357.org)
 * @brief Generic pool object for creating a pool of any type
 * @date 2020-08-10
 * @details A General Pool is a structure that holds some fixed number 
 *      of objects in memory.  The user then asks for one of the objects
 *      and receives a pointer to the object.  When the user is finished they
 *      return the object to the pool.
 *      This is not an array and the objects are not held in contigious memory
 *      so having the address of one object does not allow direct access to 
 *      another object. 
 *      Each object is associated with some meta data to help recover the 
 *      object size if needed and the object's id number within the pool. 
 *      The meta data may also include an optional callback function and a 
 *      context variable.
 *      The callback function is called automatically with the associated 
 *      context as its parameter before the pool object is returned to the 
 *      pool.
 *      A default callback may be assigned at pool definition or a callback
 *      may be attached after a pool object is allocated.
 *      The callback mechanism is intended to decouple the pool object from 
 *      the calling code for situations like passing buffers to base class
 *      methods that can then execute the derived class method on completion.
 *      The pool objects should be managed like any other memory resource
 *      (like using malloc) so make sure you free them when you don't need
 *      them anymore.
 * 
 *      Since this is primarily for use in embedded systems without malloc,
 *      There are helper macros for compile time creation of pools.
 *      To create a fixed number of objects use
 *      #define GenPoolDefine(name, type, len, onRelease)  
 *      Tis also creates type safe functions for use with the provided type
 *      and name.
 *      These type safe functions may further be automatically wrapped in type
 *      safe accessor functions wrapping the pool object itself, very handy
 *      for passing as function pointers or callbacks.
 *       
 * Copyright 2020, bdPrime
 * 
 */
#ifndef _GENPOOL_H
#define _GENPOOL_H
#include <config.h>

typedef struct
{
    volatile uintptr_t theboss:16;// a non zero number 1 to some max size
    volatile uintptr_t guard:16;  // the address of the buffer area
    CbInstance_t onRelease;
    uintptr_t obj[];              // Not actually part of the structure
} PreBuf_t;

// Pool object meta data
typedef struct 
{
    size_t objectSize;  // size of the object 
    int index;          // the offset in the pool
    CbInstance_t onRelease; // optional per buffer callback
} PreBufMeta_t;

typedef struct GenPool_t
{
    PreBuf_t * volatile next; // next entry point
    PreBuf_t * const end;     // address of last object in buffer
    GenCallback_t onRelease;  // optional function to call on release
    size_t const objectSize;  // size of the object 
    size_t const cellSize;    // size of the cell holding the object
    PreBuf_t base[];     // buffer start, not part of the structure
} GenPool_t;

#include <genPoolGenerators.h> // This contains templated typed genpool code generators

/**
 * @brief Reset a previously defined pool
 * 
 * @param self the pool object pointer
 */
void GenPool_reset(GenPool_t *self);

// General calls using base class
/**
 * @brief Allocate an object from the pool container
 * 
 * @param self - pointer to base class of pool object
 * @return void* - returns pointer to allocated object as void pointer or NULL if none available
 */
void *GenPool_allocate(GenPool_t *self);

/**
 * @brief Allocate an object from the pool container with optional
 *   callback function on attempt to return to pool.
 * 
 * @param self - pointer to base class fo pool container
 * @param cb - optional function address to call of type GenCallback_t
 * @param context - optional context used with call back function
 * @return void* - returns pointer to allocated object or NULL in none are available
 */
void *GenPool_allocate_with_callback(GenPool_t *self, GenCallback_t cb, Context_t context);

/**
 * @brief Diagnostic to check status of pool and return size and available 
 * 
 * @param self - pool object pointer
 * @param availale [out] - pointer to slot to return number of available obj i pool
 * @param total [out] - pointer to slot to return the max number of objs in pool  
 * @return Status_t Ok or BadState
 */
Status_t GenPool_status(GenPool_t const *self, int *availale, int *total);


/**
 * @brief return a object to its pool
 * 
 * @param obj pointer to original allocated object
 * @details fails silently if object was not a pool object or was
 *   already returned.
 */
Status_t GenPool_return(void *obj);

/**
 * @brief Get copy of the pool object meta data
 * 
 * @param obj the pool object pointer
 * @return PreBufMeta_t a copy of the data, zero filled on error
 */
PreBufMeta_t GenPool_object_meta(void *obj);

/**
 * @brief Get a copy of the callback information and disable the callback in the object.
 * 
 * @param obj pointer to pool object
 * @return CbInstance_t The callback information
 */
CbInstance_t GenPool_extract_callback(void *obj);

/**
 * @brief Set or update the on-return callback function attacjed to the pool object
 * 
 * @param obj pointer to the pool object
 * @param cb the callback function pointer
 * @param context the associated context 
 */
Status_t GenPool_set_return_callback(void *obj, GenCallback_t cb, Context_t context);


#endif // _GENPOOL_H
