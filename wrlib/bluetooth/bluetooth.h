/*!
 * @file bluetooth.h
 * @author Wayne Laker
 * @date May 20, 2013
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include "usart.h"

typedef usart_t bluetooth_t;

usart_t bluetooth_init (int baud_rate);
uint8_t bluetooth_receive_command (usart_t usart);
void bluetooth_send_command (usart_t usart, uint8_t command);
int bluetooth_read_ready_p (usart_t usart);
int bluetooth_write_ready_p (usart_t usart);



#endif /*BLUETOOTH_H_*/
