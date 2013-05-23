/*!
 * @file steering.h
 * @author Wayne Laker
 * @date May 20, 2013
 */

#ifndef STEERING_H_
#define STEERING_H_

#include <stdint.h>

#define PWM_STEERING_FREQ_HZ     	50

#define PWM_STEERING_DUTY_MIN     	6.8
#define PWM_STEERING_DUTY_NEUTRAL 	8.5
#define PWM_STEERING_DUTY_MAX 		10.0
#define PWM_STEERING_DUTY_RANGE_RIGHT	(PWM_STEERING_DUTY_MAX-PWM_STEERING_DUTY_NEUTRAL)		
#define PWM_STEERING_DUTY_RANGE_LEFT	(PWM_STEERING_DUTY_NEUTRAL-PWM_STEERING_DUTY_MIN)		
#define STEERING_RAMP_RATE			0.5			  

/* speed jump for increase/decrease speed*/
#define STEERING_POSITION_STEP 		10

typedef int32_t steering_position_t;

typedef struct steering_struct
{
    pwm_t pwm;
	steering_position_t position;
} steering_t;


/* Function declarations */
void steering_init(void);
void steering_turn_right(void);
void steering_turn_left(void);

#endif /*STEERING_H_*/
