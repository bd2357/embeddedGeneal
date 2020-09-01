/**
 * @file config.h
 * @brief This is a project config file and is different than normal module
 *        config files. It should normally be place at the top of any 
 *        include lists in a translation unit, before the module include.
 *        It contains defines and other compile time switches and helper
 *        macros, including those used in test.

 * @date 2020-08-16
 * 
 * Copyright 2020, bdPrime
 * 
 */
#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <stdbool.h>

// With unit test we want to expose static symbols
// so remove STATIC. Will potentially have name collisions
// for very large tests incluing many modules
#ifdef TEST
#define STATIC
#endif
// if not testing do protections
#ifndef STATIC 
#define STATIC static
#endif

// Common helper macros
#define ARR_LEN(arr) (sizeof arr / sizeof arr[0])
#define SARR_LEN(arr) ((int_fast32_t)(sizeof arr / sizeof arr[0]))
#define DIM(arr) SARR_LEN(arr)  // common alias

/**
 * @brief This is a master collection of all possible Status return values
 * defined for the project, Generally negative are serious while positive
 * ones are expected
 * 
 */
typedef enum Status_t
{
    Status_BadState = -3,   // Some invariant was violated (like assert)
    Status_Param = -2,      // A function all parameter was incorrect
    Status_Failed = -1,     // general non specifc fail
    Status_OK = 0,          // 0 means all is well
    Status_Interrupt,       // Interrupt expected action, Alternate return form callbacks
    Status_Timeout,         // expected timeout
    Status_FULL,            // A container is full, can't add to it
    Status_EMPTY,           // A container is empty, can't remove from it
    Status_BUSY,            // A process is busy, like an immediate timeout
    Status_Unexpected,      // Non fatal invariant violation, Did you try to free something twice? or use after free?
} Status_t;

typedef volatile unsigned int register_t;  //This needs to be set for every machine used

/**
 * @brief This is an object type that can hold any single
 *        value usefull as a context, typically a pointer or int.
 * @details A union because on some systems intptr_t can't hold
 *          a function pointer correctly, and C99 does not require it.
 *          (remember segment hell from old intel 16bit days)
 */
typedef union Context_t
{
    // can hold a pointer to any data object
    intptr_t v_context;
    // can hold a pointer to a function, we use our GenCallback_t but user
    // typically always casts out of a context variable for clarity.
    Status_t (*f_context)(union Context_t context);
} Context_t;


/**
 * @brief   This is a do nothing stub that has the prototype of
 *          the GenCallback_t function pointer used for generalizing
 *          callback functions in the system.
 * @details This function is not defined unless it is useful. It is left to the
 *          User to define it in their project or test enviornment. 
 *          Useful in testing by mocking this header.
 * 
 * @param context A Context_t object capable of holding and int or any 
 *                pointer (data or code)
 * @return Status_t value - returns one of the config.h types, 0 for Ok
 */
Status_t GenCallback(Context_t context);

/**
 * @brief A function pointer of general usefulness
 * 
 * @param context A Context_t object capable of holding and int or any 
 *                pointer (data or code)
 * @return Status_t value - returns one of the config.h types, 0 for Ok 
 */
typedef Status_t (*GenCallback_t)(Context_t context);


/**
 * @brief structure usefull for building queues of functions or as a function return value.
 * 
 */
typedef struct CbInstance_t
{
    GenCallback_t callback;
    Context_t context;
} CbInstance_t;





#endif