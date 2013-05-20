/*!
 * @file motor.c
 * @author Wayne Laker
 * @date May 20, 2013
 * @brief Motor module for the SAM7.
 *
 * @see motor.h
 */

#include <stdlib.h>
#include <stdint.h>

#include "pio.h"
#include "pwm.h"
#include "config.h"
#include "delay.h"

#include "motor.h"


static const pwm_cfg_t motor_pwm_cfg =
{
    .pio = PWM1_PIO,
    .period = PWM_PERIOD_DIVISOR (PWM_MOTOR_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR (PWM_MOTOR_FREQ_HZ, PWM_MOTOR_DUTY_NEUTRAL),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH
};

motor_t motor;

void
motor_init (void)
{
	motor.pwm = pwm_init (&motor_pwm_cfg);
	motor.speed = 0;
	motor.state = MOTOR_STATE_NORMAL;
    pwm_start (motor.pwm);
}

void 
motor_set_speed(motor_speed_t speed)
{	
	double duty = 0;
	if (motor.speed != speed)
	{
		if (speed < 0)
			duty = PWM_MOTOR_DUTY_NEUTRAL + PWM_MOTOR_DUTY_RANGE_FWD*speed/100;
		else if (speed > 0)
			duty = PWM_MOTOR_DUTY_NEUTRAL + PWM_MOTOR_DUTY_RANGE_REV*speed/100;
		else
			duty = PWM_MOTOR_DUTY_NEUTRAL;

		if(duty <= PWM_MOTOR_DUTY_MIN)
			duty = PWM_MOTOR_DUTY_MIN;
		if(duty >= PWM_MOTOR_DUTY_MAX)
			duty = PWM_MOTOR_DUTY_MAX;
		
		pwm_duty_set(motor.pwm, PWM_DUTY_DIVISOR (PWM_MOTOR_FREQ_HZ, duty));	

		motor.speed = speed;
	}
}

void 
motor_update(void)
{
	if (motor.state == MOTOR_STATE_REVERSE_PENDING)
	{
		motor_set_speed (-20);  	// go well past mid point
		motor.state = MOTOR_STATE_REVERSE_SET;
	}
	else if (motor.state == MOTOR_STATE_REVERSE_SET)
	{
		motor_set_speed (0);			// set neutral
		motor.state = MOTOR_STATE_NEUTRAL_SET;		
	}
	else if (motor.state == MOTOR_STATE_NEUTRAL_SET)
	{
		motor_set_speed(-MOTOR_SPEED_STEP);
		motor.state = MOTOR_STATE_NORMAL;
	}	

}


void
motor_increase_speed(void)
{	
	motor_set_speed (motor.speed + MOTOR_SPEED_STEP);
}

void
motor_decrease_speed(void)
{	
	motor_speed_t new_speed = motor.speed - MOTOR_SPEED_STEP;
	
	if (motor.state == MOTOR_STATE_NORMAL)
	{
		if (motor.speed == 0 && new_speed < 0)
			motor.state = MOTOR_STATE_REVERSE_PENDING;
		else	
			motor_set_speed (new_speed);
	}
}


