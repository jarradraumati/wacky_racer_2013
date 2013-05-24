/* File:   comms.c
   Author: 
   Date:   15/04/2013
   Descr:
*/

#include <pio.h>
#include <stdint.h>
#include "usart.h"

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

void bt_send_command (usart_t usart, int8_t command)
{
	/* Wait until we are able to transmit it back. */
	while(!usart_write_ready_p(usart)) continue;
	usart_putc(usart, command);
	led_flash(LED_BLUE_PIO);
}

int8_t bt_receive_command (usart_t usart)
{
	/* Wait for a character to be sent. */
	while (!usart_read_ready_p(usart)) continue;
	led_flash(LED_BLUE_PIO);
	return usart_getc(usart);

}

void bt_init ()
{
	/* Enable the bluetooth module by resetting */
	pio_config_set (BLUETOOTH_RESET, PIO_OUTPUT_HIGH);
	pio_output_high (BLUETOOTH_RESET);
	pio_output_low (BLUETOOTH_RESET);
}

int
main ()
{

	state_t led1_data = {LED_GREEN_PIO};
	state_t led2_data = {LED_ORANGE_PIO};
	state_t led3_data = {LED_RED_PIO};
	state_t led4_data = {LED_BLUE_PIO};
	pio_config_set (LED_GREEN_PIO, PIO_OUTPUT_HIGH);
	pio_config_set (LED_ORANGE_PIO, PIO_OUTPUT_HIGH);
	pio_config_set (LED_RED_PIO, PIO_OUTPUT_HIGH);
	pio_config_set (LED_BLUE_PIO, PIO_OUTPUT_HIGH);

	bt_init();

	/* Set up the USART to talk to the WT12 module on channel 1. This assumes
	 * it is set to its default baud rate of 115200. */
	usart_t usart = usart_init(1, USART_BAUD_DIVISOR(115200));

	/* Put the WT12 in control mode 0 which tells it to only give us the data,
	 * and not bother with control commands. */
	usart_puts(usart, "SET CONTROL ECHO 0");

	/* Clear any messages already in the buffer. */
	while (usart_read_ready_p(usart)) usart_getc(usart);

	while (1)
	{
		/* Wait for a character to be sent. */
		while (!usart_read_ready_p(usart)) continue;

		/* Read in that character. */
		int8_t ch = usart_getc(usart);

		/* Wait until we are able to transmit it back. */
		while(!usart_write_ready_p(usart)) continue;

		/* Echo it back to the sender. */
		usart_putc(usart, ch);
	}
}
