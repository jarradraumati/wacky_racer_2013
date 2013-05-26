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

/* speed jump for increase/decrease speed*/
#define STEERING_POSITION_STEP 		15
#define STEERING_TIMEOUT 			2

#define STEERING_TRIM_STEP 			0.05

typedef int32_t steering_position_t;
typedef int32_t steering_timeout_t;
typedef double steering_trim_t;

typedef enum steering_state
{
    STEERING_STATE_NORMAL = 0,
} steering_state_t;


typedef struct steering_struct
{
    pwm_t pwm;
	steering_position_t position;
	steering_trim_t trim;
	steering_timeout_t timeout;
} steering_t;


/* Function declarations */
void steering_init(void);
void steering_turn_right(void);
void steering_turn_left(void);
void steering_update(void); 		/*  call steering update about every 100ms */
void steering_trim_right(void);
void steering_trim_left(void);

#endif /*STEERING_H_*/
