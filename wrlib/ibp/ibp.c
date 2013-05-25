/*
 * Interboard Protocol for Wacky Racers
 *
 * @author Stephen Collinson
 *
 */

/* System Libs */
#ifdef DEBUG
#include <stdio.h>
#endif

#include "config.h"

/* MPH system libs */
//#include <sys.h>
//#include <pio.h>
//#include <delay.h>

/* mmculibs */
//#include "pacer.h"
#include "i2c_master.h"
#include "i2c_slave.h"

/* mat91libs */
//#include "pit.h"
//#include "pwm.h"

/* wrlibs */
#include "ibp.h"


enum {CAMERA, COMMS, MOTOR};

#define CAMERA_ADDRESS 0x32
#define MOTOR_ADDRESS 0x23



static const i2c_bus_cfg_t i2c_bus_cfg =
{
    .scl = SCL_PIO,
    .sda = SDA_PIO,
};

static const i2c_slave_cfg_t i2c_camera_cfg = 
{
    .id = CAMERA_ADDRESS,
};

static const i2c_slave_cfg_t i2c_motor_cfg = 
{
    .id = MOTOR_ADDRESS,
};

static i2c_t i2c_camera;
static i2c_t i2c_motor;

static i2c_t this_board;




/* Request information from another board
 * 
 * @param board Select which board we want info from
 * @param data Which data is required
 * @param buffer place to store requested info
 *
 */
ibp_master_read (i2c_t board, ibp_info data, int *buffer)
{
    
    return i2c_master_addr_read (board, data, 1, buffer, sizeof(buffer);
}

ibp_slave_write (
   
    ibp_info buff[1];
    
    i2c_slave_read (i2c_t

/* Send information to another board
 * 
 * @param board Select which board to send data to
 * @param data_id Which data to send
 * @param data The actual data to send
 */
ibp_send (i2c_t board, ibp_info)
{
    
    
}

/* Initialise Interboard Protocol
 * 
 * @param board Define which board to initialise for
 * 
 * TODO: Finish initialisation
 */
int ibp_init (int board)
{
    
    int ret = 1;
    
    if (board == COMMS) // This is the master board
    {
        // Do master stuff
        i2c_camera = i2c_master_init (&i2c_bus_cfg, &i2c_camera_cfg);
        i2c_motor = i2c_master_init (&i2c_bus_cfg, &i2c_motor_cfg);
        #ifdef DEBUG   
        USB_INFO("Initialised comms board as master");
        #endif
        
    }
    else if (board == CAMERA)
    {
        // Do Slave stuff
        i2c_camera = i2c_slave_init (&i2c_bus_cfg, &i2c_camera_cfg);
        i2c_this_board = i2c_camera;
        #ifdef DEBUG   
        USB_INFO("Initialised camera board as slave);
        #endif
    }
    else if (board == MOTOR)
    {
        // Do Slave stuff
        i2c_motor = i2c_slave_init (&i2c_bus_cfg, &i2c_motor_cfg);
        i2c_this_board = i2c_camera;
        #ifdef DEBUG   
        USB_INFO("Initialised motor board as slave");
        #endif
    }
    else
    {
        #ifdef DEBUG   
        USB_ERROR("Invalid board specified for Interboard Protocol");
        #endif
        ret = -1;
    }

    
    return ret;
}
    
