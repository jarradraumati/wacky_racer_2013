/*
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
#include "ir_rc5_rx.h"

/* mat91libs */
#include "pit.h"
#include "extint.h"
#include "pwm.h"

/* wrlibs */
//#include "usb_logging.h"
#include "kernel.h"
#include "motor.h"
#include "steering.h"



#define BUTTON_POLL_RATE     10
#define BUTTON_POLL_PERIOD     100

#define ALL_LEDS LED_GREEN_PIO|LED_RED_PIO|LED0_PIO|LED1_PIO|LED2_PIO|LED3_PIO


typedef struct state_struct
{
    uint32_t led;
    uint8_t on;
} state_t;

typedef struct button_task_struct
{
    uint32_t counter;
} button_task_t;


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

button_task_t button_data = {0};

/* ID of every task. Global so each task
 * may interact with other tasks.
 *
 * Add a new value whenever you register a new task
 */
enum {LED0_FLASH, BUTTON_TASK, MOTOR_TASK, STEERING_TASK, TEST_TASK, IR_TASK};


void led_flash (void *data)
{
    state_t *state = data;
    pio_output_toggle (state->led);
}

#define IR_POLL_ATTEMPTS 1000
void ir_task (void *data)
{
	int16_t bits;
	int16_t addr;
	int16_t cmd;
	uint16_t i;
	
	/* Poll the IR driver.  */

	for(i=0; i< IR_POLL_ATTEMPTS; i++)
	{
		pio_output_high (LED3_PIO);

		bits = ir_rc5_rx_read ();
	
		if (bits > 0)
		{
			addr = (bits & 0x7C0) >>6;
			cmd = bits & 0x3F;

	//		led_flash (&led0_data);


#define BTN_2 0x15
#define BTN_4 0x18
#define BTN_6 0x1E
#define BTN_8 0x21

			if (addr == 7)	// the pinnacle remote
			{
				led_flash (&led1_data);
			}
			
			switch(cmd)
			{
				case BTN_2:
					motor_increase_speed();				
					break;

				case BTN_4:
					steering_turn_right();				
					break;

				case BTN_6:
					steering_turn_left();				
					break;

				case BTN_8:
					motor_decrease_speed();				
					break;
			
			}
			return;
		}
		pio_output_low (LED3_PIO);

	}
}

void button_task (void *data)
{
    button_poll_all();

    if (button_pushed_p (button1))
    {
        if (startup)
        {
            startup--;
            return;
        }
		motor_increase_speed();
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
		motor_decrease_speed();
		pio_output_toggle (LED3_PIO);
    }

    if (button_pushed_p (button3)) // this "button" also wakes up micro
    {
        led_flash(&led3_data);
    }

}


void motor_task (void *data)
{
	motor_update();
}

void test_task (void *data)
{
	static uint8_t mode = 0;
	
	if (mode < 5)
	{
//		motor_increase_speed();
//		steering_turn_right();
	}
	else if (mode < 15)
	{
//		motor_decrease_speed();
	//	steering_turn_left();
	}
	else if (mode < 20)
	{
	//	motor_increase_speed();
		//steering_turn_right();
	}
	else
	{
		mode = 0;
	}

	mode++;


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

}


int
main (void)
{
    //usb_logging_init ();
    /* Wait for usb to do its things*/

	init();

    kernel_init ();
	motor_init();
	steering_init();
	
    /* Initialise IR driver.  */
    ir_rc5_rx_init ();
	
	/* task period is in ms */
    kernel_taskRegister (button_task, BUTTON_TASK, &button_data, BUTTON_POLL_PERIOD); 
    kernel_taskRegister (led_flash, LED0_FLASH, &led0_data, 500); 
	//kernel_taskRegister (test_task, TEST_TASK, 0, 500); 
	kernel_taskRegister (motor_task, MOTOR_TASK, 0, 100);  
	kernel_taskRegister (ir_task, IR_TASK, 0, 1);  	
    
    kernel_start ();
    
	return 0;
}

  
