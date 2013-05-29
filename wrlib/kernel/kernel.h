/*!
 * @file kernel.h
 * @author Stephen Collinson
 * @date April 10, 2013
 * @brief Header file for kernel module for the ATMega8.
 *
 * @see kernel.c
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdint.h>


/* Minimum resolution for scheduling tasks.
 * 1s / 1000 gives 1ms resolution
 */
#define TASK_PERIOD_RES PIT_RATE / 1000

#define MAX_INT 0xfffff
#define ACCURACY_DIVIDER 3000

/*! @def MAX_TASKS
 * The maximum number of tasks supported by the kernel.
 */
#define MAX_TASKS 20

/*! @enum state
 * Enum for the state of a task.
 * @typedef state_t
 * Type definition for state enum.
 */
typedef enum state
{
    TASK_READY = 0,         ///< Task is ready.
    TASK_BLOCKED = 1        ///< Task is blocked.
} taskState_t;

/*! @typedef task_func_t
 * Type definition for a function pointer.
 */
typedef void (* task_func_t) (void *data);

/*! @struct _task_struct
 * @brief Task structure containing an identifier, function pointer, execution
 * period, blocked period and task state.
 * @typedef task_t
 * Type definition for task struct.
 */
typedef struct _task_struct
{
    uint8_t id; ///< Task unique identifier.
    task_func_t func; ///< Function to call.
    void *data; ///< Generic pointer for passing data
    uint32_t period; ///< Task period.
    int blocked_for; ///< For periodic blocking
    taskState_t state; ///< State of the tasks

} task_t;

/* Function declarations */
void kernel_init (void);
uint8_t kernel_taskRegister (void (*enter_task)(void *), uint8_t id, void *data, uint32_t period);
void kernel_start (void);
void kernelTaskPeriodSet (uint8_t task_id, uint32_t period);
void kernelTaskBlocked (uint8_t task_id);
void kernelTaskReady (uint8_t task_id);
#endif /*KERNEL_H_*/
