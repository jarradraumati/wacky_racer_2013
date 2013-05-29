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

#define IMAGE_SIZE (95 * 128 * 2)
/* Globals */
uint8_t ch;

int image_bytes_left = IMAGE_SIZE;

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
    // Read in every buffered command and just remember the 
    // last
    while (bluetooth_read_ready_p (btconn))
        ch = bluetooth_receive_command (btconn);

    // Wait until we can send it back
    //while(!bluetooth_write_ready_p(btconn)) continue;
    
    /* Echo it back to the sender. */
    //bluetooth_send_command (btconn, ch);


}

void
action_commands_task (void *data)
{
    uint8_t rx[130] = {0};
    int i;
    i2c_ret_t ret;
    int to_read;
    
    bluetooth_t btconn = data;
    
    if (ch != CMD_NONE)
    {
    	// Motor commands.
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
        else if (ch >= MD_VALUES_START && ch <= MD_VALUES_STOP)
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
        else if (ch == CD_PHOTO_READY)
        {
            // Send interrupt to slaves
            pio_config_set (i2c_bus_cfg.scl, PIO_OUTPUT_LOW);
            DELAY_US (3);
            // Set clock back ready for i2c
            pio_config_set (i2c_bus_cfg.scl, PIO_PULLUP);
            DELAY_US (20);
            i2c_master_addr_read (i2c_camera, ch, 1, rx, 1);
            
            // Transmit if photo ready over bluetooth
            bluetooth_send_command (btconn, rx[0]);
        }
        else if (ch == CD_PHOTO_LINE)
        {
            to_read = image_bytes_left < 128 ? image_bytes_left : 128;
            // Send interrupt to slaves
            pio_config_set (i2c_bus_cfg.scl, PIO_OUTPUT_LOW);
            DELAY_US (3);
            // Set clock back ready for i2c
            pio_config_set (i2c_bus_cfg.scl, PIO_PULLUP);
            DELAY_US (20);
            ret = i2c_master_addr_read (i2c_camera, ch, 1, rx, to_read);  
            image_bytes_left -= ret;
            if (image_bytes_left <= 0)
                image_bytes_left = IMAGE_SIZE;
            // Transmit back data over bluetooth
            for (i = 0; i < ret; i++){
                bluetooth_send_command (btconn, rx[i]);
            }
        }
        else if (ch == CD_FAULT)
        {
            // Send interrupt to slaves
            pio_config_set (i2c_bus_cfg.scl, PIO_OUTPUT_LOW);
            DELAY_US (3);
            // Set clock back ready for i2c
            pio_config_set (i2c_bus_cfg.scl, PIO_PULLUP);
            DELAY_US (20);
            i2c_master_addr_read (i2c_camera, ch, 1, rx, 1);
            
            // Transmit if photo ready over bluetooth
            bluetooth_send_command (btconn, rx[0]);
        }
            
        ch = CMD_NONE;
    }
}


int
main ()
{
    pio_config_set (LED_GREEN_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LED_ORANGE_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LED_RED_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LED_BLUE_PIO, PIO_OUTPUT_HIGH);

    /* Initialise i2c */
    i2c_motor = i2c_master_init (&i2c_bus_cfg, &i2c_motor_cfg);
    i2c_camera = i2c_master_init (&i2c_bus_cfg, &i2c_camera_cfg);

    /* Initialise bluetooth */
    bluetooth_t btconn = bluetooth_init(115200);
    kernel_init ();
    
    
    kernel_taskRegister (bluetooth_comms_task, BLUETOOTH_COMMS_TASK, btconn, 10);
    kernel_taskRegister (action_commands_task, ACTION_COMMANDS_TASK, btconn, 100);
    
    kernel_start ();
    
    return 0;
   
}
