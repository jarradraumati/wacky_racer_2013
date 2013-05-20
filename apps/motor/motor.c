/*
 * main.c (motor board)
 * Wayne Laker
 *
 * */

/* System Libs */
#include <stdio.h>

#include "config.h"

/* MPH system libs */
#include <sys.h>
#include <pio.h>
#include <delay.h>

/* mmculibs */
#include "button.h"
#include "pacer.h"
#include "usb_cdc.h"

/* mat91libs */
#include "pit.h"
#include "extint.h"
#include "pwm.h"

/* wrlibs */
//#include "usb_logging.h"
#include "kernel.h"

#define ALL_LEDS LED_GREEN_PIO|LED_RED_PIO|LED0_PIO|LED1_PIO|LED2_PIO|LED3_PIO


#define BUTTON_POLL_RATE     100
#define BUTTON_POLL_PERIOD     10

#define PWM_MOTOR_FREQ_HZ         50
#define PWM_STEERING_FREQ_HZ     50

#define PWM_FREQ_MIN 0
#define PWM_FREQ_MAX 100e3


/* PWM DUTY in percent */
#define PWM_MOTOR_DUTY_MIN         	3.0
#define PWM_MOTOR_DUTY_NEUTRAL     	7.0
#define PWM_MOTOR_DUTY_MAX      	11.5
#define PWM_MOTOR_DUTY_RANGE_FWD 	(PWM_MOTOR_DUTY_MAX-PWM_MOTOR_DUTY_NEUTRAL)		
#define PWM_MOTOR_DUTY_RANGE_REV 	(PWM_MOTOR_DUTY_NEUTRAL-PWM_MOTOR_DUTY_MIN)		
#define MOTOR_RAMP_RATE				0.1			  

#define PWM_STEERING_DUTY_MIN     	6.8
#define PWM_STEERING_DUTY_NEUTRAL 	8.5
#define PWM_STEERING_DUTY_MAX 		10.0
#define PWM_STEERING_DUTY_RANGE_RIGHT	(PWM_STEERING_DUTY_MAX-PWM_STEERING_DUTY_NEUTRAL)		
#define PWM_STEERING_DUTY_RANGE_LEFT	(PWM_STEERING_DUTY_NEUTRAL-PWM_STEERING_DUTY_MIN)		
#define STEERING_RAMP_RATE			0.5			  

//#define PWM1_PIO PIO_DEFINE(PORT_A, 24)
//#define PWM2_PIO PIO_DEFINE(PORT_A, 25)

static const pwm_cfg_t pwm_motor_cfg =
{
    .pio = PWM1_PIO,
    .period = PWM_PERIOD_DIVISOR (PWM_MOTOR_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR (PWM_MOTOR_FREQ_HZ, PWM_MOTOR_DUTY_NEUTRAL),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH
};

static const pwm_cfg_t pwm_steering_cfg =
{
    .pio = PWM2_PIO,
    .period = PWM_PERIOD_DIVISOR (PWM_STEERING_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR (PWM_STEERING_FREQ_HZ, PWM_STEERING_DUTY_NEUTRAL),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH
};


typedef struct state_struct
{
    uint32_t led;
    uint8_t on;
} state_t;

typedef struct button_task_struct
{
    uint32_t counter;
} button_task_t;

typedef struct motor_task_struct
{
    double setpoint;
    double current;
} motor_task_t;

typedef struct steering_task_struct
{
    double setpoint;
    double current;
} steering_task_t;


static const button_cfg_t button1_cfg =
{
    .pio = BUTTON1_PIO
};

static const button_cfg_t button2_cfg =
{
    .pio = BUTTON2_PIO
};

static const button_cfg_t button3_cfg =
{
    .pio = BUTTON3_PIO
};

static const extint_cfg_t extint1_cfg =
{
    .pio = BUTTON3_PIO,
};


/* Globals */
uint8_t startup = 1;  // used to stop buttons being pressed on startup

button_t button1;
button_t button2;
button_t button3;

state_t led0_data = {LED0_PIO,0};
state_t led1_data = {LED1_PIO,0};
state_t led2_data = {LED2_PIO,0};
state_t led3_data = {LED3_PIO,0};

pwm_t pwm_motor;
pwm_t pwm_steering;

button_task_t button_data = {0};
motor_task_t motor_data = {0, 0};
steering_task_t steering_data = {0, 0};

/* ID of every task. Global so each task
 * may interact with other tasks.
 *
 * Add a new value whenever you register a new task
 */
enum {LED0_FLASH, BUTTON_TASK, MOTOR_TASK, STEERING_TASK, TEST_TASK};

static void
sleep_setup (void)
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

void led_flash (void *data)
{
    state_t *state = data;
    pio_output_toggle (state->led);
}






void button_task (void *data)
{
//    steering_task_t* steering = &steering_data;

    button_poll_all();

    if (button_pushed_p (button1))
    {
        if (startup)
        {
            startup--;
            return;
        }

//		steering->setpoint +=50;
		pio_output_toggle (LED3_PIO);
    }

    if (button_pushed_p (button2))
    {
        if (startup)
        {
            startup--;
            return;
        }

        /* // sleep test
        if(button_data->counter++ > 2)
            sleep_setup ();
         */

//		steering->setpoint -=50;
		pio_output_toggle (LED3_PIO);
    }

    if (button_pushed_p (button3)) // this "button" also wakes up micro
    {
        led_flash(&led3_data);
    }

}


void motor_task (void *data)
{
    motor_task_t* motor = data;
	double duty = PWM_MOTOR_DUTY_NEUTRAL;
	
	if (motor->current < motor->setpoint)
		motor->current += MOTOR_RAMP_RATE;
	else if (motor->current > motor->setpoint)
		motor->current -= MOTOR_RAMP_RATE;
	else
		return;
	
	if (motor->setpoint < 0)
		duty = PWM_MOTOR_DUTY_NEUTRAL + PWM_MOTOR_DUTY_RANGE_FWD*motor->setpoint/100;
	else if (motor->setpoint > 0)
		duty = PWM_MOTOR_DUTY_NEUTRAL + PWM_MOTOR_DUTY_RANGE_REV*motor->setpoint/100;

	if(duty <= PWM_MOTOR_DUTY_MIN)
		duty = PWM_MOTOR_DUTY_MIN;
	if(duty >= PWM_MOTOR_DUTY_MAX)
		duty = PWM_MOTOR_DUTY_MAX;
	
	pwm_duty_set(pwm_motor, PWM_DUTY_DIVISOR (PWM_MOTOR_FREQ_HZ, duty));	
	
}

void steering_task (void *data)
{
    steering_task_t* steering = data;
	double duty = PWM_STEERING_DUTY_NEUTRAL;
	
	if (steering->current < steering->setpoint)
		steering->current += STEERING_RAMP_RATE;
	else if (steering->current > steering->setpoint)
		steering->current -= STEERING_RAMP_RATE;
	else
		return;
	
	if (steering->setpoint < 0)
		duty = PWM_STEERING_DUTY_NEUTRAL + PWM_STEERING_DUTY_RANGE_RIGHT*steering->setpoint/100;
	else if (steering->setpoint > 0)
		duty = PWM_STEERING_DUTY_NEUTRAL + PWM_STEERING_DUTY_RANGE_LEFT*steering->setpoint/100;

	if(duty <= PWM_STEERING_DUTY_MIN)
		duty = PWM_STEERING_DUTY_MIN;
	if(duty >= PWM_STEERING_DUTY_MAX)
		duty = PWM_STEERING_DUTY_MAX;
	
	pwm_duty_set(pwm_steering, PWM_DUTY_DIVISOR (PWM_STEERING_FREQ_HZ, duty));	
	
}


void test_task (void *data)
{
    motor_task_t* motor = &motor_data;
	steering_task_t* steering = &steering_data;
	
	static uint8_t mode = 0;
	
	if (mode == 0)
	{
		mode = 1;
		motor->setpoint = 50;
		steering->setpoint = 50;
	}
	else
	{
		mode = 0;
		motor->setpoint = 20;
		steering->setpoint = 0;		
	}
	
	
}

void init(void)
{
	/* startup_sequence */
    pio_config_set (ALL_LEDS, PIO_OUTPUT_LOW);
    delay_ms(1000);
    pio_output_high (ALL_LEDS);
    delay_ms(500);
    pio_output_low (ALL_LEDS);

	/* buttons */
	button1 = button_init (&button1_cfg);
    button2 = button_init (&button2_cfg);
    button3 = button_init (&button3_cfg); // this is on one of the dip switch pins with an IRQ for waking up
    pio_config_set (button1->pio|button2->pio|button3->pio, PIO_INPUT);
    button_poll_count_set (BUTTON_POLL_COUNT (BUTTON_POLL_RATE));

	/* pwm channels */
	pwm_motor = pwm_init (&pwm_motor_cfg);
    pwm_steering = pwm_init (&pwm_steering_cfg);
    pwm_start (pwm_motor);
    pwm_start (pwm_steering);
}


int
main ()
{
    //usb_logging_init ();
    /* Wait for usb to do its things*/

	init();

    kernel_init ();

	/* task period is in ms */
    kernel_taskRegister (button_task, BUTTON_TASK, &button_data, BUTTON_POLL_PERIOD); 
    kernel_taskRegister (led_flash, LED0_FLASH, &led0_data, 500); 
    kernel_taskRegister (motor_task, MOTOR_TASK, &motor_data, 100); 
    kernel_taskRegister (steering_task, STEERING_TASK, &steering_data, 100); 
	kernel_taskRegister (test_task, TEST_TASK, 0, 2000); 
    
    kernel_start ();
    
	return 0;
}

  
