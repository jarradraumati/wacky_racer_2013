/*!
 * @file sleep.c
 * @author Wayne Laker
 * @date May 25, 2013
 * @brief sleep module for the wacky racers
 *
 * @see sleep.h
 */

#include <stdlib.h>
#include <stdint.h>

#include "pio.h"
#include "config.h"
//#include "button.h"

#include "extint.h"

#include "sleep.h"

static const extint_cfg_t extint1_cfg =
{
    .pio = EXTINT1_PIO,
};

void
sleep_init (void)
{
//	button_init (&extint1_cfg);
	pio_config_set (EXTINT1_PIO, PIO_INPUT);	
}


void
sleep_now (void)
{
    extint_t extint;

    pio_shutdown (EXTINT1_PIO);

    /* Note, only two PIOs can be used on the SAM7 for external interrupts.  */
    extint = extint_init (&extint1_cfg);

    extint_sleep (extint);
    
    /* Force a CPU reset.  */
    cpu_reset ();

    /* Never get here.  */
    while (1)
        continue;
}

