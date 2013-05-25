/*!
 * @file motor.h
 * @author Wayne Laker
 * @date May 20, 2013
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>

#define PWM_MOTOR_FREQ_HZ         	50

/* PWM DUTY in percent */
#define PWM_MOTOR_DUTY_MIN         	5.0
#define PWM_MOTOR_DUTY_NEUTRAL     	7.0
#define PWM_MOTOR_DUTY_MAX      	11.5
#define PWM_MOTOR_DUTY_RANGE_FWD 	(PWM_MOTOR_DUTY_MAX-PWM_MOTOR_DUTY_NEUTRAL)		
#define PWM_MOTOR_DUTY_RANGE_REV 	(PWM_MOTOR_DUTY_NEUTRAL-PWM_MOTOR_DUTY_MIN)		

/* speed jump for increase/decrease speed*/
#define MOTOR_SPEED_STEP_FWD 			5
#define MOTOR_SPEED_STEP_REV 			2

#define MOTOR_TIMEOUT 					20  // about 2 seconds if update called @ 10 Hz


typedef enum motor_state
{
    MOTOR_STATE_NORMAL = 0,
	MOTOR_STATE_REVERSE_PENDING,
	MOTOR_STATE_REVERSE_SET,
	MOTOR_STATE_NEUTRAL_SET
} motor_state_t;


typedef int32_t motor_speed_t;
typedef int32_t motor_timeout_t;

typedef struct motor_struct
{
    pwm_t pwm;
	motor_speed_t speed;
	motor_state_t state;
	motor_timeout_t timeout;
} motor_t;

/* Function declarations */
void motor_init (void);
void motor_increase_speed(void);
void motor_decrease_speed(void);
void motor_update(void); 			/*  call motor update about every 100ms */
void motor_stop (void);
void motor_keepalive(void);

#endif /*MOTOR_H_*/
