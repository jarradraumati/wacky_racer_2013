/*!
 * @file steering.c
 * @author Wayne Laker
 * @date May 20, 2013
 * @brief steering module for the SAM7.
 *
 * @see steering.h
 */

#include <stdlib.h>
#include <stdint.h>

#include "pio.h"
#include "pwm.h"
#include "config.h"

#include "steering.h"

static const pwm_cfg_t steering_pwm_cfg =
{
    .pio = PWM2_PIO,
    .period = PWM_PERIOD_DIVISOR (PWM_STEERING_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR (PWM_STEERING_FREQ_HZ, PWM_STEERING_DUTY_NEUTRAL),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH
};

steering_t steering;

void
steering_init (void)
{
	steering.pwm = pwm_init (&steering_pwm_cfg);
	steering.position = 0;
	steering.timeout = 0;
	steering.trim = 0.0;
    pwm_start (steering.pwm);
}

void 
steering_set_position(steering_position_t position, int8_t force_update)
{	
	double duty = 0;
	if (steering.position != position || force_update)
	{
		if (position < 0)
			duty = PWM_STEERING_DUTY_NEUTRAL + PWM_STEERING_DUTY_RANGE_LEFT*position/100;
		else if (position > 0)
			duty = PWM_STEERING_DUTY_NEUTRAL + PWM_STEERING_DUTY_RANGE_RIGHT*position/100;
		else
			duty = PWM_STEERING_DUTY_NEUTRAL;

		duty += steering.trim;

		if(duty <= PWM_STEERING_DUTY_MIN)
			duty = PWM_STEERING_DUTY_MIN;
		if(duty >= PWM_STEERING_DUTY_MAX)
			duty = PWM_STEERING_DUTY_MAX;
		
		pwm_duty_set(steering.pwm, PWM_DUTY_DIVISOR (PWM_STEERING_FREQ_HZ, duty));	

		steering.position = position;
	}
}


void
steering_set_centre(void)
{	
	steering_set_position (0, 1);
}

void 
steering_set_timeout (steering_timeout_t timeout)
{
	steering.timeout = timeout;
}

void
steering_turn_right(void)
{	
	steering_set_position (steering.position - STEERING_POSITION_STEP, 0);
	steering_set_timeout (STEERING_TIMEOUT);
}

void
steering_turn_left(void)
{	
	steering_set_position (steering.position + STEERING_POSITION_STEP, 0);
	steering_set_timeout (STEERING_TIMEOUT);	
}

void 
steering_update(void)
{
	if (steering.timeout)
	{
		steering.timeout--;		
		if (!steering.timeout)
		{
			steering_set_centre ();
		}	 
	}
}

void
steering_trim_left(void)
{
	steering.trim += STEERING_TRIM_STEP;
	steering_set_position (steering.position, 1);
}

void
steering_trim_right(void)
{
	steering.trim -= STEERING_TRIM_STEP;
	steering_set_position (steering.position, 1);
}

