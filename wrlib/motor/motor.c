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

// Prototypes
void motor_set_timeout (motor_timeout_t timeout);
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
	motor.direction = MOTOR_UNDEFINED;
	motor.state = MOTOR_STATE_NORMAL;
	motor_set_timeout (0);	
    pwm_start (motor.pwm);
}

void 
motor_set_speed(motor_speed_t speed)
{	
	double duty = 0;

	if (speed < -100)
		speed = -100;
	if (speed > 100)
		speed = 100;

	if (motor.speed != speed)
	{
		if (speed < 0)
			duty = PWM_MOTOR_DUTY_NEUTRAL + PWM_MOTOR_DUTY_RANGE_FWD*speed/100;
		else if (speed > 0)
		{
			motor.direction = MOTOR_FORWARD;
			duty = PWM_MOTOR_DUTY_NEUTRAL + PWM_MOTOR_DUTY_RANGE_REV*speed/100;
		}
		else
			duty = PWM_MOTOR_DUTY_NEUTRAL;

		if(duty < PWM_MOTOR_DUTY_MIN)
			duty = PWM_MOTOR_DUTY_MIN;
		if(duty >= PWM_MOTOR_DUTY_MAX)
			duty = PWM_MOTOR_DUTY_MAX;
		
		pwm_duty_set(motor.pwm, PWM_DUTY_DIVISOR (PWM_MOTOR_FREQ_HZ, duty));	

		motor.speed = speed;
	}
	
	motor_keepalive ();
}

void 
motor_set_timeout (motor_timeout_t timeout)
{
	motor.timeout = timeout;
}

void 
motor_update(void)
{
	/* handle lost comms so we don't crash the car! */
	if (motor.state == MOTOR_STATE_NORMAL)
	{
		if (motor.braking && motor.direction == MOTOR_REVERSE)
		{	
			motor_set_speed (50);
			motor.state = MOTOR_STATE_FINISHED_BRAKING;
			motor.braking = 0;
			motor.direction = MOTOR_FORWARD;
		}
		else if (motor.speed > 0)
			motor_set_speed (motor.speed - 1);
		else if (motor.speed < 0)
			motor_set_speed (motor.speed + 1);
	}
	/* handle the servo switching into reverse */
	else if (motor.state == MOTOR_STATE_REVERSE_PENDING)
	{
		motor_set_speed (-20);  	// go well past mid point
		motor.state = MOTOR_STATE_REVERSE_SET;
	}
	else if (motor.state == MOTOR_STATE_REVERSE_SET)
	{
		motor_set_speed (0);		// set neutral
		motor.state = MOTOR_STATE_NEUTRAL_SET;		
	}
	else if (motor.state == MOTOR_STATE_NEUTRAL_SET)
	{
		if (motor.braking)		
		{
			motor_set_speed (-100);				
			motor.state = MOTOR_STATE_FINISHED_BRAKING;
			motor.braking = 0;
			motor.direction = MOTOR_REVERSE;
		}			
		else
		{
			motor_set_speed(-MOTOR_SPEED_STEP_REV);
			motor.state = MOTOR_STATE_NORMAL;
		}
	}
	else if (motor.state == MOTOR_STATE_FINISHED_BRAKING)	
	{
		motor.state = MOTOR_STATE_NORMAL;
		motor_set_speed (0);
	}

}

void
motor_brake (void)
{
	motor.braking = 1;
	if (motor.direction == MOTOR_FORWARD)
		motor.state = MOTOR_STATE_REVERSE_PENDING;
}

void
motor_increase_speed (void)
{
	motor.braking = 0;
	if (motor.speed >= 0)
		motor_set_speed (motor.speed + MOTOR_SPEED_STEP_FWD);
	else
		motor_set_speed (motor.speed + MOTOR_SPEED_STEP_REV);
}


void
motor_decrease_speed (void)
{	
	motor_speed_t new_speed = 0;
	motor.braking = 0;	
	
	if (motor.speed >= 0)
		new_speed = motor.speed - MOTOR_SPEED_STEP_FWD;
	else
		new_speed = motor.speed - MOTOR_SPEED_STEP_REV;
	
	if (motor.state == MOTOR_STATE_NORMAL)
	{
		if (motor.speed >= 0 && new_speed < 0 && motor.direction == MOTOR_FORWARD)
		{
			motor.direction = MOTOR_REVERSE;
			motor.state = MOTOR_STATE_REVERSE_PENDING;
		}
		else
		{
			motor_set_speed (new_speed);
		}				
	}	
}

void
motor_keepalive(void)
{
	//motor_set_timeout (MOTOR_TIMEOUT);
}

motor_speed_t
motor_speed_get(void)
{
	return motor.speed;
}

motor_speed_t
motor_pause(void)
{
	motor_speed_t current = motor.speed;
	motor_set_speed (0);
	return current;
}

void
motor_resume(motor_speed_t speed)
{
	motor_set_speed(speed);	
}

motor_direction_t
motor_direction_get(void)
{
	return motor.direction;
}
