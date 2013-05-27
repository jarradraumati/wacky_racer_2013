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
#include "string.h"
#include "delay.h"
#include "kernel.h"
#include "ibp.h"

/* Globals */
uint8_t ch;

/* ID of every task. Global so each task
 * may interact with other tasks.
 *
 * Add a new value whenever you register a new task
 */
enum {BLUETOOTH_COMMS_TASK, ACTION_COMMANDS_TASK};

/* Commands sent from the comms board over bluetooth */
//enum {CMD_NONE = 0x00, CMD_LEFT = 0x30, CMD_RIGHT, CMD_FORWARD, CMD_BACK,
//        CMD_STOP, CMD_ASSUME_CTRL, CMD_CAM_OFF, CMD_CAM_ON};

#define PACER_RATE 1000

static const i2c_bus_cfg_t i2c_bus_cfg =
{
    .scl = SCL_PIO,
    .sda = SDA_PIO
};


static const i2c_slave_cfg_t i2c_camera_cfg =
{
    .id = I2C_CAMERA_ADDR,
};

static const i2c_slave_cfg_t i2c_motor_cfg =
{
    .id = I2C_MOTOR_ADDR,
};

i2c_t i2c_camera;
i2c_t i2c_motor;

typedef struct state_struct
{
        uint32_t led;
        uint8_t on;
} state_t;

void 
led_flash (void *data)
{
    state_t *state = data;
    pio_output_toggle (state->led);
}

void
bluetooth_comms_task (void *data)
{
    bluetooth_t btconn = data;
    // Check if there is anything to be read
    if (bluetooth_read_ready_p (btconn))
        ch = bluetooth_receive_command (btconn);

    // Wait until we can send it back
    //while(!bluetooth_write_ready_p(btconn)) continue;
    
    /* Echo it back to the sender. */
    //bluetooth_send_command (btconn, ch);


}

void
action_commands_task (void *data)
{
    
    if (ch != CMD_NONE)
    {
        if (ch >= MC_VALUES_START && ch <= MC_VALUES_STOP)
        {
            // Send interrupt to slaves
            pio_config_set (i2c_bus_cfg.scl, PIO_OUTPUT_LOW);
            DELAY_US (3);
            // Set clock back ready for i2c
            pio_config_set (i2c_bus_cfg.scl, PIO_PULLUP);
            DELAY_US (20);
            i2c_master_addr_write (i2c_motor, COMMS_COMMAND, 1, &ch, 1);
            
            // Ensure clock is left in a pullup state
            pio_config_set (i2c_bus_cfg.scl, PIO_PULLUP);

        }
        else if (ch >= CC_VALUES_START && ch <= CC_VALUES_STOP)
        {
            // Send interrupt to slaves
            pio_config_set (i2c_bus_cfg.scl, PIO_OUTPUT_LOW);
            DELAY_US (3);
            // Set clock back ready for i2c
            pio_config_set (i2c_bus_cfg.scl, PIO_PULLUP);
            DELAY_US (20);
            i2c_master_addr_write (i2c_camera, COMMS_COMMAND, 1, &ch, 1);
            
            // Ensure clock is left in a pullup state
            pio_config_set (i2c_bus_cfg.scl, PIO_PULLUP);
        }
        
        ch = CMD_NONE;
    }
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

    int empty = 0;
    

    
    /* Initialise i2c */
    i2c_motor = i2c_master_init (&i2c_bus_cfg, &i2c_motor_cfg);
    i2c_camera = i2c_master_init (&i2c_bus_cfg, &i2c_camera_cfg);

    /* Initialise bluetooth */
    bluetooth_t btconn = bluetooth_init(115200);
    kernel_init ();
    
    
    kernel_taskRegister (bluetooth_comms_task, BLUETOOTH_COMMS_TASK, btconn, 10);
    kernel_taskRegister (action_commands_task, ACTION_COMMANDS_TASK, &led1_data, 15);
    
    kernel_start ();
    
    return 0;
   
}
