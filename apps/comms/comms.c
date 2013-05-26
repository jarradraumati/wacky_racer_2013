/* File:   comms.c
   Author: 
   Date:   15/04/2013
   Descr:
*/

#include <pio.h>
#include <stdint.h>
#include "i2c_master.h"
#include "bluetooth.h"
#include <stdio.h>
#include "sys.h"
#include "pacer.h"
#include "string.h"

#include "kernel.h"

/* Globals */

/* ID of every task. Global so each task
 * may interact with other tasks.
 *
 * Add a new value whenever you register a new task
 */
enum {LED_FLASH, LED1_FLASH, LED2_FLASH};

/* Commands sent from the comms board over bluetooth */
enum {CMD_NONE = 0x00, CMD_LEFT = 0x30, CMD_RIGHT, CMD_FORWARD, CMD_BACK,
        CMD_STOP, CMD_ASSUME_CTRL, CMD_CAM_OFF, CMD_CAM_ON};

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

    /* Initialise i2c */
    i2c_t i2c_motor;
    i2c_t i2c_camera;
    
    uint8_t ch;

    i2c_motor = i2c_master_init (&i2c_bus_cfg, &i2c_motor_cfg);
    i2c_camera = i2c_master_init (&i2c_bus_cfg, &i2c_camera_cfg);

    /* Initialise bluetooth */
    usart_t btconn = bluetooth_init(115200);
    //kernel_init ();
   
    pacer_init (PACER_RATE);

    while (1)
    {
        
        /* Wait for a character to be sent. */
        ch = bluetooth_receive_command (btconn);
        
        /* Wait until we are able to transmit it back. */
        while(!bluetooth_write_ready_p(btconn)) continue;
        
        switch (ch) 
        {
            case CMD_LEFT:
                pio_output_toggle (LED_GREEN_PIO);
                break;
            case CMD_RIGHT:
                pio_output_toggle (LED_ORANGE_PIO);
                break;
            case CMD_FORWARD:
                pio_output_toggle (LED_RED_PIO);
                break;
            case CMD_BACK:
                pio_output_toggle (LED_BLUE_PIO);
                break;
            case CMD_STOP:
                break;
                // Do stop stuff
        };
        
        /* Echo it back to the sender. */
        bluetooth_send_command (btconn, ch);
    }
}
