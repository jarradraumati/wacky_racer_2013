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

#define SDA_PIO PIO_DEFINE(PORT_A, 5)
#define SCL_PIO PIO_DEFINE(PORT_A, 6)

#define EXTINT1_PIO PIO_DEFINE (PORT_A, 30)

i2c_t i2c_slave1;
extint_t extint1;

static void handler (void)
{
    // Disable interrupt temp
    extint_disable (extint1);
    irq_clear (AT91C_ID_IRQ1);
    pio_output_toggle (LED1_PIO);
    i2c_addr_t addr;
    uint8_t buffer[5];
    i2c_ret_t ret;
    uint8_t data1[] = {7, 1, 2, 3};


    ret = i2c_slave_read (i2c_slave1, buffer, sizeof (buffer), 10000);

    addr = buffer[0];

    if (addr >= sizeof (data1))
        addr = 0;

    if (ret == 1)
        ret = i2c_slave_write (i2c_slave1, &data1[0], sizeof (data1), 1000);
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
    .id = 0x32,
};

void
comms_task (void *data)
{
    
//     else if (ret == 5)
//         memcpy (&data1[addr], buffer + 1, sizeof(data1[0]));
}

int
main (void)
{
    


    pio_config_set (LED1_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LED2_PIO, PIO_OUTPUT_HIGH);
    i2c_slave1 = i2c_slave_init (&i2c_bus_cfg, &i2c_slave1_cfg);
    extint1 = extint_init (&extint1_cfg);
    extint_enable (extint1);
    while (1){
        //comms_task (i2c_slave1);
        continue;
    }
//     kernel_init ();
//     kernel_taskRegister (comms_task, 0, i2c_slave1, 1);
//     kernel_start ();
    
    return 0;
}

