/* File:   i2cm_test1.c
   Author: M. P. Hayes, UCECE
   Date:   13 April 2013
   Descr: 
*/
#include "i2c_master.h"
#include "pacer.h"
#include "delay.h"

#define SDA_PIO PIO_DEFINE(PORT_A, 3)
#define SCL_PIO PIO_DEFINE(PORT_A, 4)

#define PACER_RATE 1

static const i2c_bus_cfg_t i2c_bus_cfg =
{
    .scl = SCL_PIO,
    .sda = SDA_PIO
};


static const i2c_slave_cfg_t i2c_slave1_cfg =
{
    .id = 0x32,
};


int
main (void)
{
    i2c_t i2c_slave1;

    i2c_slave1 = i2c_master_init (&i2c_bus_cfg, &i2c_slave1_cfg);

    pacer_init (PACER_RATE);


    while (1)
    {
        uint8_t tx[] = {1, 2, 3, 4};
        uint8_t rx[] = {0, 0, 0, 0};
        i2c_addr_t addr = 1;

        pacer_wait ();
        pio_config_set (i2c_bus_cfg.scl, PIO_OUTPUT_LOW);
        DELAY_US (3);
        // Set clock back ready for i2c
        pio_config_set (i2c_bus_cfg.scl, PIO_PULLUP);
        DELAY_US (10);
        //i2c_master_addr_write (i2c_slave1, addr, 1, tx, sizeof(tx));
        // Trigger interrupt to inform slave an i2c transaction is going to occur

        
        i2c_master_addr_read (i2c_slave1, 4, 1, rx, sizeof(rx));
        /* TODO: check if rx matches tx.  */
    }
}

