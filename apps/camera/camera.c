/* File:   main.c
   Author: Stephen Collinson
   Date:   15/04/2013
   Descr:
*/

#include <pio.h>
#include <stdint.h>

#include "kernel.h"
#include "i2c_slave.h"
#include "ibp.h"
#include "tcm8230.h"
#include "extint.h"
#include "delay.h"

#define I2C_COMMS_DATA_SIZE 20


/* Globals */

i2c_t i2c_camera;
uint8_t buffer[2];
uint8_t* comms_data[I2C_COMMS_DATA_SIZE];
extint_t extint1;

/* ID of every task. Global so each task
 * may interact with other tasks.
 *
 * Add a new value whenever you register a new task
 */
enum {LED_FLASH, LED1_FLASH, LED2_FLASH};

static const i2c_bus_cfg_t i2c_bus_cfg = {.scl = SCL_PIO,.sda = SDA_PIO};

static const i2c_slave_cfg_t i2c_camera_cfg = {.id = I2C_CAMERA_ADDR,};

/* Comms tasks */
void comms_talk_handler (void)
{
    i2c_ret_t ret;
    i2c_addr_t addr;
    ret = i2c_slave_read(i2c_camera, buffer, 2, 10000);
    
    if (ret >= 0)
        pio_output_toggle (LED1_PIO);
    if (ret < I2C_OK) return;
       
    
    addr = buffer[0];
    if (ret == 1)
    {
        if (addr != CD_PHOTO_LINE)
                i2c_slave_write (i2c_camera, comms_data[addr], 1, IBP_I2C_TIMEOUT);
        else if (addr == CD_PHOTO_LINE)
        {
            ret = i2c_slave_write (i2c_camera,
                            comms_data[*comms_data[CD_PHOTO_NEXT_LINE] * SQCIF_WIDTH * 2],
                            SQCIF_WIDTH * 2, IBP_I2C_TIMEOUT);
            if (ret < 0 )
            {
                #ifdef USB_CDC_H
                printf(stderr, "Did not send photo line\n");
                #endif
            }
            else if (ret != SQCIF_WIDTH * 2)
            {
                #ifdef USB_CDC_H
                printf(stderr, "Sent %d bytes\n", ret);
                #endif
            }
            else
            {
                int current_line = *comms_data[CD_PHOTO_NEXT_LINE];
                current_line ++;

                if (current_line > SQCIF_HEIGHT)
                {
                    current_line = 0;
                    comms_data[CD_PHOTO_READY] = 0;
                }
                *comms_data[CD_PHOTO_NEXT_LINE] = current_line;
            }
        }
    }
}

static const extint_cfg_t extint1_cfg = {.pio = EXTINT1_PIO,.handler = comms_talk_handler,.type=LOW_LEVEL_EXTINT};

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
        extint1 = extint_init (&extint1_cfg);
        extint_enable (extint1);

        uint8_t photo_ready = 7;
        comms_data[CD_PHOTO_READY] = &photo_ready;
        i2c_camera = i2c_slave_init (&i2c_bus_cfg, &i2c_camera_cfg);
        uint8_t data[] = {7,1,2,3};


        kernel_taskRegister (led_flash, LED_FLASH, &led1_data, 1000); /* period is in ms */
        kernel_taskRegister (led_flash, LED1_FLASH, &led2_data, 500);
        kernel_taskRegister (led_flash, LED2_FLASH, &led3_data, 250);
        //kernel_taskRegister(comms_talk_task, COMMS_TALK_TASK, &comms_data, 10);

        kernel_start ();
        return 0;
}
