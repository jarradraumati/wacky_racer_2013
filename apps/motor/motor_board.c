/*
 * motor_board.c (motor board)
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
#include "usb_cdc.h"
#include "i2c_slave.h"

/* mat91libs */
#include "adc.h"
#include "extint.h"
#include "irq.h"

/* wrlibs */
#include "usb_logging.h"
#include "kernel.h"
#include "motor.h"
#include "steering.h"
#include "ir.h"
#include "sleep.h"
#include "ibp.h"


#define BUTTON_POLL_RATE     	10
#define BUTTON_POLL_PERIOD     100

#define ALL_LEDS LED_GREEN_PIO|LED_RED_PIO|LED0_PIO|LED1_PIO|LED2_PIO|LED3_PIO


#define ADC_SPEED_FORWARD	ADC_CHANNEL_4
#define ADC_SPEED_REVERSE	ADC_CHANNEL_5
#define ADC_BATTERY_SENSE	ADC_CHANNEL_6
#define BATTERY_LOW_VOLTAGE 6.5
#define SPEED_SLOW 2.4
#define SPEED_MEDIUM 1.7
#define SPEED_FAST 1.4
#define SPEED_BUFFER 0.7

i2c_t i2c_slave1;
extint_t extint1;
uint8_t next_command = 0;
uint8_t comms_data[3];

static void handler (void)
{
    // Disable interrupt temp
    extint_disable (extint1);
    irq_clear (AT91C_ID_IRQ1);
    
    uint8_t addr;
    uint8_t buffer[2];
    i2c_ret_t ret;

    DELAY_US (4);

    ret = i2c_slave_read (i2c_slave1, buffer, sizeof (buffer), 10000);

    addr = buffer[0]-MD_VALUES_START;

    if (addr >= sizeof (comms_data))
        addr = 0;

    if (ret == 1)
        ret = i2c_slave_write (i2c_slave1, comms_data[addr], 1, 1000);
    if (ret == 2)
    {
        if (buffer[0] == COMMS_COMMAND)
            next_command = buffer[1];
    }
    extint_enable (extint1);
}


static const extint_cfg_t extint1_cfg =
{
    .pio = EXTINT1_PIO,
    .handler = handler
};


static const i2c_bus_cfg_t i2c_bus_cfg =
{
    .scl = SCL_PIO,
    .sda = SDA_PIO
};


static const i2c_slave_cfg_t i2c_slave1_cfg =
{
    .id = I2C_MOTOR_ADDR,
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

typedef struct monitor_task_struct
{
    double battery_voltage;
	double motor_speed;
} monitor_task_t;

static const button_cfg_t button2_cfg =
{
    .pio = BUTTON2_PIO
};


/* Globals */
uint8_t startup = 1;  // used to stop buttons being pressed on startup

button_t button1;
button_t button2;

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
enum {LED0_FLASH, BUTTON_TASK, MOTOR_TASK, STEERING_TASK, IR_TASK, USB_TASK, MONITOR_TASK, COMMAND_TASK};


void led_flash (void *data)
{
    state_t *state = data;
    pio_output_toggle (state->led);
}

void monitor_task (void *data)
{
	monitor_task_t* monitor = data;
	adc_sample_t value = 0;
	motor_direction_t direction;
	static uint8_t stopped = 1;
	
	 // this needs to be done when motors are stopped, maybe
	 //        in separate task enabled when motors off?
	if ( adc_read_wait (ADC_BATTERY_SENSE, &value) )
	{
		monitor->battery_voltage = value * 3.3 * 3 / 1024;
		//comms_data[MD_CHARGE-MD_VALUES_START] = (uint8_t)(monitor->battery_voltage*10); // change to percentage
		comms_data[MD_CHARGE-MD_VALUES_START] = 5;
		
		if (monitor->battery_voltage < BATTERY_LOW_VOLTAGE)
		//if (value < 620)
			pio_output_high (LED_RED_PIO);	
		else
			pio_output_low (LED_RED_PIO);
	}
	
	// decide which ADC to read and read it
	direction = motor_direction_get();
	switch (direction)
	{
		case MOTOR_FORWARD:
			 adc_read_wait (ADC_SPEED_FORWARD, &value);
			 break;
		case MOTOR_REVERSE:
			 adc_read_wait (ADC_SPEED_REVERSE, &value);
			 break;
		default:
		break;
		
	}
	
	if (direction)
	{
		monitor->motor_speed = value * 3.3 / 1024;
		
		if (monitor->motor_speed < 0.4 || monitor->motor_speed > SPEED_SLOW)
		{
			stopped = 1;
			pio_output_low (LED3_PIO);	
			pio_output_low (LED2_PIO);	
			pio_output_low (LED1_PIO);
		}
		else if (monitor->motor_speed < SPEED_SLOW && monitor->motor_speed > SPEED_MEDIUM)
		{
			stopped = 0;
			pio_output_low (LED3_PIO);
			pio_output_low (LED2_PIO);
			pio_output_high (LED1_PIO);	
		}
		else if (monitor->motor_speed < SPEED_MEDIUM && monitor->motor_speed > SPEED_FAST && !stopped)
		{
			pio_output_low (LED3_PIO);
			pio_output_high (LED2_PIO);	
			pio_output_high (LED1_PIO);	
		}
		else if (monitor->motor_speed < SPEED_FAST && monitor->motor_speed > SPEED_BUFFER && !stopped)
		{
			pio_output_high (LED3_PIO);	
			pio_output_high (LED2_PIO);	
			pio_output_high (LED1_PIO);	
		}
	}

	if (!monitor->motor_speed)
		comms_data[MD_SPEED-MD_VALUES_START] = 0;
	else
	{
/*		comms_data[MD_SPEED-MD_VALUES_START] = 100 - (uint8_t) ((monitor->motor_speed - 0.8) / 0.017 ) ;
		
		if (direction == MOTOR_REVERSE)
			comms_data[MD_SPEED-MD_VALUES_START] 
		comms_data[MD_SPEED-MD_VALUES_START] += 100;		
*/

		monitor->motor_speed = 100 - ((monitor->motor_speed - 0.8) / 0.017 ) ;
		if (direction == MOTOR_REVERSE)
			monitor->motor_speed = -monitor->motor_speed; 
		comms_data[MD_SPEED-MD_VALUES_START] = (uint8_t)(monitor->motor_speed + 100);		
	}
}


void
command_task (void *data)
{
    if (next_command)
    {
		switch (next_command){

            case MC_LEFT:
                steering_turn_left ();
                pio_output_toggle (LED0_PIO);
                break;
            case MC_RIGHT:
                steering_turn_right ();
                pio_output_toggle (LED1_PIO);
                break;
                
            case MC_FORWARD:
                motor_increase_speed ();
                pio_output_toggle (LED2_PIO);                
                break;
                
            case MC_REVERSE:
                motor_decrease_speed ();
                pio_output_toggle (LED3_PIO);                
                break;
            case MC_STOP_MOTOR:
                motor_stop ();
                break;

     }

        next_command = 0;
        
    }
}


void ir_task (void *data)
{

	ir_message_t msg;
	
	if (ir_read_message (&msg) == IR_SUCCESS)
	{
		//pio_output_toggle (LED1_PIO);

		switch(msg.command)
		{
			case IR_REMOTE_BTN_2:
				motor_increase_speed();				
				break;

			case IR_REMOTE_BTN_4:
				steering_turn_left ();				
				break;

			case IR_REMOTE_BTN_5:
				motor_brake();				
				break;

			case IR_REMOTE_BTN_6:
				steering_turn_right();				
				break;

			case IR_REMOTE_BTN_8:
				motor_decrease_speed();				
				break;

			case IR_REMOTE_BTN_REWIND:
				steering_trim_left();				
				break;

			case IR_REMOTE_BTN_FASTFWD:
				steering_trim_right();				
				break;
				
			default:
				pio_output_toggle (LED3_PIO);			
				break;
		}
	}
}

void button_task (void *data)
{
    button_poll_all();

    if (button_pushed_p (button2))
    {
		usb_logging_send_text("INFO from %s on line %d", __FILE__, __LINE__, "sleeping..."); 	
        sleep_now ();
         
    }

}

void motor_task (void *data)
{
    motor_update ();   
}

void steering_task (void *data)
{
	steering_update ();     
}



void init(void)
{
	/* led init */
    pio_config_set (ALL_LEDS, PIO_OUTPUT_LOW);
	
	/* button init */
    button2 = button_init (&button2_cfg);
    pio_config_set (button2->pio, PIO_INPUT);
    button_poll_count_set (BUTTON_POLL_COUNT (BUTTON_POLL_RATE));

	adc_init (0);

	/* wrlibs */
    usb_logging_init ();    
	motor_init ();
	steering_init ();
	ir_init ();
	sleep_init ();	
    kernel_init ();
	
	/* startup_sequence */
    delay_ms(1000);
    pio_output_high (ALL_LEDS);
    delay_ms(500);
    pio_output_low (ALL_LEDS);
	
}


int
main (void)
{
	init ();
	
    i2c_slave1 = i2c_slave_init (&i2c_bus_cfg, &i2c_slave1_cfg);
    extint1 = extint_init (&extint1_cfg);

    /* initialise comms_data */
    comms_data[MD_SPEED-MD_VALUES_START] = 0;
    comms_data[MD_FAULT-MD_VALUES_START] = 0;
    comms_data[MD_CHARGE-MD_VALUES_START] = 10;

	extint_enable (extint1);

	/* task period is in ms */
    kernel_taskRegister (button_task, BUTTON_TASK, &button_data, BUTTON_POLL_PERIOD); 
    kernel_taskRegister (led_flash, LED0_FLASH, &led0_data, 500); 
	kernel_taskRegister (motor_task, MOTOR_TASK, 0, 100);  
	kernel_taskRegister (steering_task, STEERING_TASK, 0, 200);  
	kernel_taskRegister (ir_task, IR_TASK, 0, 1);
	kernel_taskRegister (monitor_task, MONITOR_TASK, 0, 200);
    kernel_taskRegister (command_task, 0, &comms_data, 10);
    
    kernel_start ();
    
	return 0;
}

  
