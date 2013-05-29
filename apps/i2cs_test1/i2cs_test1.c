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
#include "tcm8230.h"
//#include "usb_logging.h"

// Tasks registered with the kernel
enum {COMMAND_TASK, CAPTURE_TASK};

i2c_t i2c_slave1;
extint_t extint1;
uint8_t next_command = 0;
uint8_t *comms_data[4];
uint8_t test_data[4];
static uint8_t image[SQCIF_WIDTH * SQCIF_HEIGHT * 2] = {0}; // Store returned photo




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

    addr = buffer[0] - ARRAY_OFFSET;

    if (addr >= sizeof (comms_data))
        addr = 0;

    if (ret == 1)
    {
        if (buffer[0] != CD_PHOTO_LINE)
            ret = i2c_slave_write (i2c_slave1, comms_data[addr], 130, 1000);
        else
        {
            ret = i2c_slave_write (i2c_slave1, comms_data[CD_PHOTO_NEXT_LINE-ARRAY_OFFSET], 130, 1000);
            comms_data[CD_PHOTO_NEXT_LINE-ARRAY_OFFSET] += 128;
        }
        
    }
    if (ret == 2)
    {
        if (buffer[0] == COMMS_COMMAND)
            next_command = buffer[1];
    }
    extint_enable (extint1);
}

/* Camera configuration data */
static const tcm8230_cfg_t cfg =
{
    .picsize = TCM8230_PICSIZE_SQCIF
};

/* I2C interrupt configuration data */
static const extint_cfg_t extint1_cfg =
{
    .pio = EXTINT1_PIO,
    .handler = handler,
    .type = LOW_LEVEL_EXTINT
};

/* I2C Bus configuration data */
static const i2c_bus_cfg_t i2c_bus_cfg =
{
    .scl = SCL_PIO,
    .sda = SDA_PIO
};

/* I2C Slave configuration data */
static const i2c_slave_cfg_t i2c_slave1_cfg =
{
    .id = I2C_CAMERA_ADDR,
};

void
capture_task (void *data)
{
    int ret;
    
    while ( tcm8230_frame_ready_p ())
        continue;    
    
    while (! tcm8230_frame_ready_p ())
        continue;
    
    ret = tcm8230_capture (image, sizeof(image), 200);
    
    if (ret < 0) // Capturing failed
        *comms_data[CD_PHOTO_READY-ARRAY_OFFSET] = 0;
    else
    {
        *comms_data[CD_PHOTO_READY-ARRAY_OFFSET] = 1;        
        // Re-block task
        kernelTaskBlocked (CAPTURE_TASK);
    }
}
    
    
    
void
command_task (void *data)
{
    if (next_command)
    {
        switch (next_command){
            case CC_CAPTURE:
                kernelTaskReady (CAPTURE_TASK);
                pio_output_toggle (LED1_PIO);
                break;
                
            case CC_SLEEP_CAM:
                //pio_output_toggle (LED1_PIO);
                break;
                
            case CC_WAKE_CAM:
                //pio_output_toggle (LED2_PIO);
                break;
                
            case CC_SLEEP:
                //pio_output_toggle (LED3_PIO);
                break;
                
        }
        
        next_command = 0;
        
    }
}

int
main (void)
{
    // Configire LEDS
    pio_config_set (LED0_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LED1_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LED2_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LED3_PIO, PIO_OUTPUT_HIGH);
    
    i2c_slave1 = i2c_slave_init (&i2c_bus_cfg, &i2c_slave1_cfg); // Initialise as I2C slave
    extint1 = extint_init (&extint1_cfg);       // Initialise I2C Interrupt
    // Initialise comms_data
    uint8_t cd_photo_ready = 0;
    uint8_t cd_photo_next_line = image;
    uint8_t cd_fault = 0;
    comms_data[CD_PHOTO_READY-ARRAY_OFFSET] = &cd_photo_ready;
    comms_data[CD_PHOTO_LINE-ARRAY_OFFSET] = 0;
    comms_data[CD_PHOTO_NEXT_LINE-ARRAY_OFFSET] = &cd_photo_next_line;
    comms_data[CD_FAULT-ARRAY_OFFSET] = &cd_fault;
    
    extint_enable (extint1);    // Enable I2C Interrupt
    tcm8230_init (&cfg);        // Initialise Camera
    kernel_init ();             // Initialise Kernel
    
    // Register Tasks for kernel
    kernel_taskRegister (command_task, COMMAND_TASK, &comms_data, 200);
    kernel_taskRegister (capture_task, CAPTURE_TASK, 0, 500);
    
    // Block any currently uneeded task
    kernelTaskBlocked (CAPTURE_TASK);
    
    
    // Finally start the kernel, no code beyone here will be run
    kernel_start ();

    return 0;
}
