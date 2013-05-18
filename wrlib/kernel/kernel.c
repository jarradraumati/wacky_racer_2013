/*!
 * @file kernel.c
 * @author Stephen Collinson
 * @author Jarrad Raumati
 * @date June 20, 2012
 * @brief Kernel module for the ATMega8.
 *
 * @see kernel.h
 */



#include <stdlib.h>
#include <stdint.h>

#include "pit.h"
#include "kernel.h"
#include "pio.h"
#include "config.h"
//#include "usb_logging.h"

#ifdef DEBUG
#include "debug/debug.h"
#endif

static task_t *tasks;
static uint8_t tasks_registered = 0;

/*! @brief Kernel initialiser.
 * Initialise the kernel by initialising the timer and setting up space for
 * tasks.
 * @return None.
 */
void
kernel_init (void)
{
	/* Setup timer */
	pit_init ();
	/* Setup task storage */
	tasks = (task_t *) malloc (MAX_TASKS * sizeof (task_t));

	#ifdef DEBUG
	if (!tasks)
		debug_error (ERR_KERNEL_MALLOC);
	#endif
}

/*! @brief Register a task with the kernel
 * Returns a task id of 0 if the task was not registered
 * @param enter_task Pointer to function for task
 * @param period period at which the task should run
 * @return number of tasks registered
 */
uint8_t
kernel_taskRegister (void (*enter_task)(void *), uint8_t id, void *data, uint32_t period)
{

	/* Check if trying to register too many tasks. */
	if (tasks_registered <= (MAX_TASKS - 1))
	{
		tasks[tasks_registered].id = id;
		tasks[tasks_registered].func = enter_task;
		tasks[tasks_registered].data = data;
		tasks[tasks_registered].period = period;
		tasks[tasks_registered].blocked_for = 0;
		tasks[tasks_registered].state = TASK_READY;
	}
	else
	{
		#ifdef DEBUG_1
		debug_error (ERR_KERNEL_NOT_REGISTERED);
		#endif
		return (0);
	}

	tasks_registered++;

	return (tasks_registered -1);

}

/*! @brief Kernel start.
 * Start the kernel running within an infinite loop.
 * @return None.
 */
void
kernel_start (void)
{

	uint32_t i;
	uint32_t tick = 0;

	uint32_t now = 0;
	uint32_t then = 0;
	uint32_t diff = 0;
	uint32_t extra = 0;

	then = pit_get ();

	while (1)
	{

		/* If there has been another tick then decrement
		 * the current block period of each task, unless it's already
		 * blocked for 0 time
		 */
		now = pit_get ();

		if (then > now)
			diff = (MAX_INT - then) + now; // Deal with wrap-around
		else
			diff = now - then;

		tick = tick + (diff / ACCURACY_DIVIDER);
		extra = diff % ACCURACY_DIVIDER;

		then = now - extra;


		if (tick > 0)
		{
			//USB_INFO ("tick = %d", tick);
			for (i = 0; i < tasks_registered; i++)
			{
				if (tasks[i].blocked_for > 0)
					tasks[i].blocked_for -= tick;

			}
			tick = 0;

			/* Run through the list of tasks round-robin style
			 * If a task is in a blocked state then it is not run
			 */
			for (i = 0; i < tasks_registered; i++)
			{
				if (tasks[i].state == TASK_READY)
				{
					/* If the tasked is blocked for 0 time then it can be run */
					if (tasks[i].blocked_for == 0)
					{

						/* Run the task */
						tasks[i].func (tasks[i].data);
						/* Block the task again for the period it specifies */
						tasks[i].blocked_for = tasks[i].period;
					}
				}

			}
		}
	}
}

/*! @brief Blocks a task.
 * Set a task to blocked state so that it won't be executed
 * @param task_id Task to block
 * @return None.
 */
void
kernelTaskBlocked (uint8_t task_id)
{
	tasks[task_id].state = TASK_BLOCKED;
}


/*! @brief Ready a task.
 * Set a task to ready state so that it can be executed
 * @param task_id Task to unblock
 * @return None.
 */
void
kernelTaskReady (uint8_t task_id)
{
	tasks[task_id].state = TASK_READY;
}

/*! @brief Obtains tasks' state.
 * Returns the state of the current task
 * @param task_id Task to query
 * @return Task's status
 */
taskState_t
kernelTaskState (uint8_t task_id)
{
	return (tasks[task_id].state);
}

/*! @brief Change the period of a task
 */
void kernelTaskPeriodSet (uint8_t task_id, uint32_t period)
{
	tasks[task_id].period = period;
}

