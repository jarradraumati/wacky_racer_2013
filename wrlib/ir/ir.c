/*!
 * @file i.c
 * @author Wayne Laker
 * @date May 25, 2013
 * @brief IR module for the SAM7.
 *
 * @see ir.h
 */

#include <stdlib.h>
#include <stdint.h>

#include "pio.h"
#include "pwm.h"
#include "config.h"
#include "delay.h"
#include "ir_rc5_rx.h"

#include "ir.h"

void
ir_init (void)
{
    /* Initialise IR driver.  */
    ir_rc5_rx_init ();
}

ir_ret_t 
ir_read_message(ir_message_t * msg)
{	
	int16_t address;
	int16_t bits;
	uint16_t i;
	
	for(i=0; i< IR_POLL_ATTEMPTS; i++)
	{
		bits = ir_rc5_rx_read ();
	
		if (bits > 0)
		{
			address = (bits & 0x07C0) >> 6;
			msg->toggle = (bits & 0x3800) >> 13;
			msg->command = bits & 0x003F;
			
			if (address == IR_REMOTE_ADDRESS)	
				return IR_SUCCESS;
		}
	}
	return IR_FAILURE;
}
