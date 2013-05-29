/*!
 * @file motor.h
 * @author Wayne Laker
 * @date May 20, 2013
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>
#include "pwm.h"
#define PWM_MOTOR_FREQ_HZ         	50

/* PWM DUTY in percent */
#define PWM_MOTOR_DUTY_MIN         	5.0
#define PWM_MOTOR_DUTY_NEUTRAL     	7.0
#define PWM_MOTOR_DUTY_MAX      	11.5
#define PWM_MOTOR_DUTY_RANGE_FWD 	(PWM_MOTOR_DUTY_MAX-PWM_MOTOR_DUTY_NEUTRAL)		
#define PWM_MOTOR_DUTY_RANGE_REV 	(PWM_MOTOR_DUTY_NEUTRAL-PWM_MOTOR_DUTY_MIN)

/* Set some limits for speed */
#define MAX_FOR_SPEED 20
#define MAX_REV_SPEED 100

/* speed jump for increase/decrease speed*/
#define MOTOR_SPEED_STEP_FWD 			5
#define MOTOR_SPEED_STEP_REV 			2

#define MOTOR_TIMEOUT 					20  // about 2 seconds if update called @ 10 Hz


typedef enum motor_state
{
    MOTOR_STATE_NORMAL = 0,
	MOTOR_STATE_REVERSE_PENDING,
	MOTOR_STATE_REVERSE_SET,
	MOTOR_STATE_NEUTRAL_SET,
	MOTOR_STATE_FINISHED_BRAKING	
} motor_state_t;

typedef enum motor_direction
{
    MOTOR_UNDEFINED = 0,
	MOTOR_FORWARD,
	MOTOR_REVERSE
} motor_direction_t;


typedef int32_t motor_speed_t;
typedef int32_t motor_timeout_t;

typedef struct motor_struct
{
    pwm_t pwm;
	motor_speed_t speed;
	uint8_t braking;
	motor_direction_t direction;
	motor_state_t state;
	motor_timeout_t timeout;
} motor_t;

/* Function declarations */
void motor_init (void);
void motor_increase_speed(void);
void motor_decrease_speed(void);
void motor_update(void); 			/*  call motor update about every 100ms */
void motor_brake (void);
void motor_keepalive(void);
void motor_stop (void);
motor_speed_t motor_pause(void);
void motor_resume(motor_speed_t speed);
motor_direction_t motor_direction_get(void);
motor_speed_t motor_speed_get(void);
#endif /*MOTOR_H_*/
