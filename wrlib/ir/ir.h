/*!
 * @file ir.h
 * @author Wayne Laker
 * @date May 25, 2013
 */

#ifndef IR_H_
#define IR_H_

#include <stdint.h>

#define IR_POLL_ATTEMPTS 	1000

#define IR_REMOTE_BTN_2 	0x15
#define IR_REMOTE_BTN_4 	0x18
#define IR_REMOTE_BTN_5 	0x1B
#define IR_REMOTE_BTN_6 	0x1E
#define IR_REMOTE_BTN_8 	0x21
#define IR_REMOTE_ADDRESS	0x07	// the pinnacle remote

typedef int16_t ir_toggle_t;
typedef int16_t ir_command_t;


typedef struct ir_message_struct
{
	ir_toggle_t toggle;
	ir_command_t command;
} ir_message_t;

typedef enum ir_ret 
{
	IR_SUCCESS = 0,
    IR_FAILURE = -1,
} ir_ret_t;

/* Function declarations */
void ir_init (void);
ir_ret_t ir_read_message(ir_message_t * msg);

#endif /*IR_H_*/
