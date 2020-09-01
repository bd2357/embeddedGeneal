/**
 * @file isr_abstraction.h
 * @author Bryce Deary (github@bd2357.org)
 * @brief This is a module to convert from machine depended isr overhead
 *  to our simple model.  
 * 
 * @details this may be just some defines that remap some vendor package
 *  but we are defining a module in case we need to add glue code.
 *  This module is project or at least vendor specific.
 * 
 * 
 * 
 * 
 * 
 * @date 2020-08-22
 * Copyright 2020, bdPrime
 * 
 */
#ifndef ISR_ABSTRACTION_H
#define ISR_ABSTRACTION_H

#include <config.h>

typedef struct IsrAbstraction
{
    void *entry;    // for now just pointer to isr table entry
} IsrAbstraction;

/**
 * @brief hold an isr state.
 */
typedef register_t isr_state_t;

/**
 * @brief Disable all interrupts
 * 
 * @return isr_state_t - the previous state to enable nesting
 */
isr_state_t Isr_Disable(void);
/**
 * @brief Reenable interrupts to previous state.
 * 
 * @param state - value retured from prior call to Isr_Disable.
 */
void Isr_Enable(isr_state_t state);

/**
 * @brief Set Isr mask to a specific value, ues in systems
 *   with prioritized interrupt levels.
 * 
 * @param new The mask to be instituted 
 * @return isr_state_t The current (prior) state
 */
isr_state_t Isr_Mask   (isr_state_t new);
/**
 * @brief Restore the isr state from prior call to Isr_Mask
 * 
 * @param state 
 */
void        Isr_Restore(isr_state_t state);  // probable just alias to Isr_Mask








#endif // ISR_ABSTRACTION_H
