/* File:   motor.c
   Author: 
   Date:   15/04/2013
   Descr:
*/

#include <pio.h>
#include <stdint.h>

#include "kernel.h"

/* Globals */

/* ID of every task. Global so each task
 * may interact with other tasks.
 *
 * Add a new value whenever you register a new task
 */
enum {LED_FLASH, LED1_FLASH, LED2_FLASH};


typedef struct state_struct
{
        uint32_t led;
        uint8_t on;
} state_t;

void led_flash (void *data)
{
        state_t *state = data;
        pio_output_toggle (state->led);
}

int
main ()
{

        state_t led1_data = {LED1_PIO};
        state_t led2_data = {LED2_PIO};
        state_t led3_data = {LED3_PIO};
        pio_config_set (LED1_PIO, PIO_OUTPUT_LOW);
        pio_config_set (LED2_PIO, PIO_OUTPUT_LOW);
        pio_config_set (LED3_PIO, PIO_OUTPUT_LOW);
        kernel_init ();

        kernel_taskRegister (led_flash, LED_FLASH, &led1_data, 1000); /* period is in ms */
        kernel_taskRegister (led_flash, LED1_FLASH, &led2_data, 500);
        kernel_taskRegister (led_flash, LED2_FLASH, &led3_data, 250);

        kernel_start ();
        return 0;
}
