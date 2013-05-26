/*!
 * @file bluetooth.c
 * @author Wayne Laker
 * @date May 20, 2013
 * @brief Bluetooth module for the SAM7.
 *
 * @see bluetooth.h
 */

#include <stdlib.h>
#include <stdint.h>

#include "usart.h"
#include "pio.h"

int
bluetooth_write_ready_p (usart_t usart)
{
    return usart_write_ready_p(usart);
}
int
bluetootht_read_ready_p (usart_t usart)
{
    return usart_read_ready_p(usart);
}

void 
bluetooth_send_command (usart_t usart, uint8_t command)
{
    /* Wait until we are able to transmit */
    while(!usart_write_ready_p(usart)) continue;
    usart_putc(usart, command);
}

uint8_t 
bluetooth_receive_command (usart_t usart)
{
    /* Wait for a character to be sent. */
    while (!usart_read_ready_p(usart)) continue;
    return usart_getc(usart);
}

usart_t 
bluetooth_init (int baud_rate)
{
    /* Enable the bluetooth module by resetting */
    pio_config_set (BLUETOOTH_RESET_PIO, PIO_OUTPUT_HIGH);
    pio_output_high (BLUETOOTH_RESET_PIO);
    pio_output_low (BLUETOOTH_RESET_PIO);
    
    /* Set up the USART to talk to the WT12 module on channel 1.*/
    usart_t usart_con = usart_init(1, USART_BAUD_DIVISOR(baud_rate));
    /* Clear any messages already in the buffer. */
    while (usart_read_ready_p(usart_con)) usart_getc(usart_con);
    
    /* Put the WT12 in control mode 0 which tells it to only give us the data,
     * and not bother with control commands. */
    usart_puts(usart_con, "SET CONTROL ECHO 0");
    usart_puts(usart_con, "SET BT AUTH * 0987");
    usart_puts(usart_con, "SET BT NAME GROUP2");
    
    return usart_con;
}
