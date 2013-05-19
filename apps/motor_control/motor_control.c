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
#define PWM_MOTOR_DUTY_MIN         0.0
#define PWM_MOTOR_DUTY_DEFAULT     7.0
#define PWM_MOTOR_DUTY_MAX         11.5
//#define PWM_MOTOR_DIVISOR         10.0

#define PWM_STEERING_DUTY_MIN         6.8
#define PWM_STEERING_DUTY_DEFAULT     8.5
#define PWM_STEERING_DUTY_MAX         10.0
//#define PWM_STEERING_DIVISOR         10.0

#define PWM1_PIO PIO_DEFINE(PORT_A, 24)
#define PWM2_PIO PIO_DEFINE(PORT_A, 25)

static const pwm_cfg_t pwm_motor_cfg =
{
    .pio = PWM1_PIO,
    .period = PWM_PERIOD_DIVISOR (PWM_MOTOR_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR (PWM_MOTOR_FREQ_HZ, PWM_MOTOR_DUTY_DEFAULT),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH
};

static const pwm_cfg_t pwm_steering_cfg =
{
    .pio = PWM2_PIO,
    .period = PWM_PERIOD_DIVISOR (PWM_STEERING_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR (PWM_STEERING_FREQ_HZ, PWM_STEERING_DUTY_DEFAULT),
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
    double motor_pwm_duty;
    double steering_pwm_duty;
} motor_task_t;


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
button_t button1;
button_t button2;
button_t button3;

state_t led0_data = {LED0_PIO};
state_t led1_data = {LED1_PIO};
state_t led2_data = {LED2_PIO};
state_t led3_data = {LED3_PIO};

pwm_t pwm_motor;
pwm_t pwm_steering;

button_task_t button_data = {0};
motor_task_t motor_data = {    PWM_MOTOR_DUTY_DEFAULT,
                            PWM_STEERING_DUTY_DEFAULT};

/* ID of every task. Global so each task
 * may interact with other tasks.
 *
 * Add a new value whenever you register a new task
 */
enum {LED0_FLASH, LED1_FLASH, LED2_FLASH, BUTTON_TASK, MOTOR_TASK};

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

uint8_t startup = 1;



void button_task (void *data)
{
    //button_task_t* button_data = data;

    button_poll_all();

    if (button_pushed_p (button1))
    {
        if (startup)
        {
            startup--;
            return;
        }

        motor_data.motor_pwm_duty+=0.1;
        if(motor_data.motor_pwm_duty >= PWM_MOTOR_DUTY_MAX)
            motor_data.motor_pwm_duty = PWM_MOTOR_DUTY_MAX;
        pwm_duty_set(pwm_motor,
             PWM_DUTY_DIVISOR (PWM_MOTOR_FREQ_HZ, motor_data.motor_pwm_duty));

  //      motor_data.steering_pwm_duty+=0.1;
  //      if(motor_data.steering_pwm_duty >= PWM_STEERING_DUTY_MAX)
  //          motor_data.steering_pwm_duty = PWM_STEERING_DUTY_MAX;
  //      pwm_duty_set(pwm_steering,
  //             PWM_DUTY_DIVISOR (PWM_STEERING_FREQ_HZ, motor_data.steering_pwm_duty));
		
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

        motor_data.motor_pwm_duty-=0.1;
        if(motor_data.motor_pwm_duty <= PWM_MOTOR_DUTY_MIN)
            motor_data.motor_pwm_duty = PWM_MOTOR_DUTY_MIN;
        pwm_duty_set(pwm_motor,
            PWM_DUTY_DIVISOR (PWM_MOTOR_FREQ_HZ, motor_data.motor_pwm_duty));

  //      motor_data.steering_pwm_duty-=0.1;
  //      if(motor_data.steering_pwm_duty <= PWM_STEERING_DUTY_MIN)
  //          motor_data.steering_pwm_duty = PWM_STEERING_DUTY_MIN;
  //      pwm_duty_set(pwm_steering,
  //          PWM_DUTY_DIVISOR (PWM_STEERING_FREQ_HZ, motor_data.steering_pwm_duty));
		
		pio_output_toggle (LED3_PIO);
    }

    if (button_pushed_p (button3)) // this "button" also wakes up micro
    {
        led_flash(&led3_data);
    }

}


void motor_task (void *data)
{
    motor_task_t* motor_data = data;

}


int
main ()
{
    //usb_logging_init ();
    /* Wait for usb to do its things*/

    pio_config_set (ALL_LEDS, PIO_OUTPUT_LOW);

    delay_ms(1000);
    pio_output_high (ALL_LEDS);
    delay_ms(500);
    pio_output_low (ALL_LEDS);

    button1 = button_init (&button1_cfg);
    button2 = button_init (&button2_cfg);
    button3 = button_init (&button3_cfg);
    pio_config_set (button1->pio|button2->pio|button3->pio, PIO_INPUT);
    button_poll_count_set (BUTTON_POLL_COUNT (BUTTON_POLL_RATE));


    pwm_motor = pwm_init (&pwm_motor_cfg);
    pwm_steering = pwm_init (&pwm_steering_cfg);

    pwm_start (pwm_motor);
    pwm_start (pwm_steering);


    kernel_init ();


    kernel_taskRegister (button_task, BUTTON_TASK, &button_data, BUTTON_POLL_PERIOD); /* period is in ms */
    kernel_taskRegister (led_flash, LED0_FLASH, &led0_data, 500); /* period is in ms */
    //kernel_taskRegister (motor_task, MOTOR_TASK, &motor_data, 100); /* period is in ms */



    kernel_start ();
    return 0;
}

  
