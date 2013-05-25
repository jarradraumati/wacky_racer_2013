/* File:   comms.c
   Author: 
   Date:   15/04/2013
   Descr:
*/

#include <pio.h>
#include <stdint.h>
#include "usart.h"
#include "i2c_master.h"
#include "usb_cdc.h"

#include "usb_logging.h"
#include <stdio.h>
#include "sys.h"
#include "pacer.h"

#include "kernel.h"

/* Globals */

/* ID of every task. Global so each task
 * may interact with other tasks.
 *
 * Add a new value whenever you register a new task
 */
enum {LED_FLASH, LED1_FLASH, LED2_FLASH};

/* Commands sent from the comms board over bluetooth */
#define CMD_NONE				0x00
#define CMD_FORWARD          	0x01
#define CMD_BACK             	0x02
#define CMD_LEFT             	0x04
#define CMD_RIGHT            	0x08
#define CMD_STOP             	0x10
#define CMD_ASSUME_CTRL      	0x20
#define CMD_CAM_OFF          	0x40
#define CMD_CAM_ON           	0x80

#define PACER_RATE 1000

static const i2c_bus_cfg_t i2c_bus_cfg =
{
    .scl = SCL_PIO,
    .sda = SDA_PIO
};


static const i2c_slave_cfg_t i2c_camera_cfg =
{
    .id = 0x32,
};

static const i2c_slave_cfg_t i2c_motor_cfg =
{
    .id = 0x16,
};

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

	/* Initialise usb */
    usb_cdc_t usb_cdc;

    usb_cdc = usb_cdc_init ();

    sys_redirect_stdin ((void *)usb_cdc_read, usb_cdc);
    sys_redirect_stdout ((void *)usb_cdc_write, usb_cdc);
    sys_redirect_stderr ((void *)usb_cdc_write, usb_cdc);


    /* Wait until USB configured.  */
    usb_cdc_update ();


	/* Initialise i2c */
    i2c_t i2c_motor;
    i2c_t i2c_camera;

    i2c_motor = i2c_master_init (&i2c_bus_cfg, &i2c_motor_cfg);
    i2c_camera = i2c_master_init (&i2c_bus_cfg, &i2c_camera_cfg);

	/* Initialise bluetooth */
	bt_init();

	/* Set up the USART to talk to the WT12 module on channel 1. This assumes
	 * it is set to its default baud rate of 115200. */
	usart_t usart = usart_init(1, USART_BAUD_DIVISOR(115200));

	/* Put the WT12 in control mode 0 which tells it to only give us the data,
	 * and not bother with control commands. */
	usart_puts(usart, "SET CONTROL ECHO 0");
	usart_puts(usart, "SET BT AUTH * 0987");
	usart_puts(usart, "SET BT NAME GROUP2");

	/* Clear any messages already in the buffer. */
	while (usart_read_ready_p(usart)) usart_getc(usart);

	pacer_init (PACER_RATE);

	while (1)
	{
		uint8_t command[1];


		/* Read the command. */
		command[0] = usart_getc(usart);
		printf("Command received from bluetooth: %x\n", command[0]);
		/* Set i2c addr */
		i2c_addr_t addr = 1;

		/* Determine what the command is. */
		switch (command[0])
		{
		case CMD_NONE:
			break;
		case CMD_FORWARD:
			i2c_master_addr_write (i2c_motor, addr, 1, command, sizeof(command));
			break;
		case CMD_BACK:
			i2c_master_addr_write (i2c_motor, addr, 1, command, sizeof(command));
			break;
		case CMD_LEFT:
			i2c_master_addr_write (i2c_motor, addr, 1, command, sizeof(command));
			break;
		case CMD_RIGHT:
			i2c_master_addr_write (i2c_motor, addr, 1, command, sizeof(command));
			break;
		case CMD_STOP:
			i2c_master_addr_write (i2c_motor, addr, 1, command, sizeof(command));
			break;
		case CMD_ASSUME_CTRL:
			break;
		case CMD_CAM_OFF:
			i2c_master_addr_write (i2c_camera, addr, 1, command, sizeof(command));
			break;
		case CMD_CAM_ON:
			i2c_master_addr_write (i2c_camera, addr, 1, command, sizeof(command));
			break;
		}

		/* Check is USB disconnected.  */
		usb_cdc_update ();
	}
}
