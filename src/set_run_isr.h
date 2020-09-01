/**
 * @file set_run_isr.h
 * @author Bryce Deary (github@bd2357.org)
 * @brief header for a simple embedded tasker
 * @version 0.1
 * @date 2020-08-18
 * 
 * @details
 * Idea is traditional setup section and run section where the run section is a
 * a loop that runs fixed or queued functions either directly or via timer.
 * 
 * It is expected that any ISRs would be setup in the setup section and sent
 * any working object structure that can be exchanged with the User space.
 * 
 * 
 */
#ifndef SET_RUN_ISR_H
#define SET_RUN_ISR_H

#include <config.h>
#include <genQ.h>

#define TASK_RUNNERS   20
#define FUTURE_RUNNERS 20

extern bool system_run;
extern volatile uint32_t time_tick;

/**
 * @brief Schedule a function to run in the User Loop
 * 
 * @param cb - the CbInstance_t 
 * @return Status_t - Ok or failure reason
 */
Status_t Run_Task(CbInstance_t cb);

/**
 * @brief Schedule a function to run once
 * 
 * @param cb 
 * @return Status_t 
 */
Status_t Run_Task_Once(CbInstance_t cb);


/**
 * @brief Schedule a function to be run in the future
 * 
 * @param cb The CbInstance to queue up (instance not pointer)
 * @param future the time in the future from current in ticks
 * @return Status_t - Return Ok if successful, else an error reason.
 */
Status_t Run_Later(CbInstance_t cb, uint32_t future);

/**
 * @brief Schedule a function to be run continuously until it self
 *   terminates by return non-zero.
 * 
 * @param cb The CbInstance to queue up (instance not pointer)
 * @param interval The interval in ticks, 
 *          positive values will reschedule the interval time from current
 *          negative values will reschedule at fixed increments and may
 *          pile up if there are delays elsewhere.
 *   
 * @return Status_t - Return Ok if successful, else an error reason
 */
Status_t Run_Periodically(CbInstance_t cb, int32_t interval);

/**
 * @brief Prototype for the user provided Setup
 * 
 */
void User_Setup(void);  

/**
 * @brief Main body User tasks
 * 
 */

void User_Loop(void);

/**
 * @brief User provided user functions run at top of User_Loop
 * 
 */
void ProcessUser(void);


#endif // SET_RUN_ISR_H
