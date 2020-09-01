/**
 * @file isr_helper.h
 * @author Bryce Deary (github@bd2357.org)
 * @brief Simple wrapper to allow User functions to operate
 *   protected or with elevated privledge.
 * @date 2020-08-30
 * @details Just simple function to call another fuction wrapped in
 *   a protection block. 
 *  
 * Copyright 2020, bdPrime
 * 
 */
#ifndef ISR_HELPER_H
#define ISR_HELPER_H
#include <isr_abstraction.h> // to verify the calls to protection

/**
 * @brief Helper function to run a user function protected from 
 *  interrupts
 * 
 * @param cb - the function to run in a CbInstance wrapper
 */
Status_t Run_Protected(CbInstance_t cb);


/**
 * @brief Helper function to run user function with elevated 
 *   protection from interrupts.
 * 
 * @param isr_mask 
 * @param cb 
 * @return STATIC_INLINE 
 */
Status_t Run_Privileged(isr_state_t isr_mask, CbInstance_t cb);
#endif // ISR_HELPER_H
