/* File:   i2cs_test1.c
   Author: M. P. Hayes, UCECE
   Date:   13 April 2013
   Descr: 
*/
#include "i2c_slave.h"
#include <string.h>
#include "pio.h"
#include "kernel.h"
#include "extint.h"
#include "irq.h"
#include "delay.h"
#include "ibp.h"
#include "usb_logging.h"
#include "steering.h"
#include "motor.h"


#define EXTINT1_PIO PIO_DEFINE (PORT_A, 30)

i2c_t i2c_slave1;
extint_t extint1;
uint8_t next_command = 0;
uint8_t comms_data[4];
uint8_t image[95]; /* Globals initialised to all zeros */



static void handler (void)
{
    // Disable interrupt temp
    extint_disable (extint1);
    irq_clear (AT91C_ID_IRQ1);
    
    uint8_t addr;
    uint8_t buffer[2];
    i2c_ret_t ret;

    
    DELAY_US (4);
    //pio_output_toggle (LED1_PIO);

    ret = i2c_slave_read (i2c_slave1, buffer, sizeof (buffer), 10000);

    addr = buffer[0];

    if (addr >= sizeof (comms_data))
        addr = 0;

    if (ret == 1)
        ret = i2c_slave_write (i2c_slave1, &comms_data[addr], 1, 1000);
    if (ret == 2)
    {
        if (buffer[0] == COMMS_COMMAND)
            next_command = buffer[1];
    }
    extint_enable (extint1);
}


static const extint_cfg_t extint1_cfg =
{
    .pio = EXTINT1_PIO,
    .handler = handler,
    .type = LOW_LEVEL_EXTINT
};


static const i2c_bus_cfg_t i2c_bus_cfg =
{
    .scl = SCL_PIO,
    .sda = SDA_PIO
};


static const i2c_slave_cfg_t i2c_slave1_cfg =
{
    .id = I2C_MOTOR_ADDR,
};

void
command_task (void *data)
{
    if (next_command)
    {
        switch (next_command){
            case MC_LEFT:
                steering_turn_left ();
                pio_output_toggle (LED0_PIO);
                break;
            case MC_RIGHT:
                steering_turn_right ();
                pio_output_toggle (LED1_PIO);
                break;
                
            case MC_FORWARD:
                motor_increase_speed ();
                pio_output_toggle (LED2_PIO);                
                break;
                
            case MC_BACK:
                motor_decrease_speed ();
                pio_output_toggle (LED3_PIO);                
                break;
            case MC_STOP:
                motor_brake ();
                break;
        }
        
        next_command = 0;
        
    }
}

void motor_task (void *data)
{
        motor_update ();   
}

void steering_task (void *data)
{
        steering_update ();     
}
int
main (void)
{
    


    pio_config_set (LED0_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LED1_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LED2_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LED3_PIO, PIO_OUTPUT_HIGH);
    i2c_slave1 = i2c_slave_init (&i2c_bus_cfg, &i2c_slave1_cfg);
    extint1 = extint_init (&extint1_cfg);
    
    /* initialise comms_data */
    comms_data[CD_PHOTO_READY] = 1;
    comms_data[CD_PHOTO_LINE] = 2;
    comms_data[CD_PHOTO_NEXT_LINE] = 3;
    comms_data[CD_FAULT] = 4;
    
    /* Setup USB */
    usb_logging_init ();
    
    extint_enable (extint1);
    steering_init ();
    motor_init ();
    
    kernel_init ();
    kernel_taskRegister (command_task, 0, &comms_data, 10);
    kernel_taskRegister (motor_task, 1, 0, 100);
    kernel_taskRegister (steering_task, 2, 0, 300);
    kernel_start ();
//     
    return 0;
}
