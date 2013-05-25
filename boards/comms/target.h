/** @file   target.h
    @author M. P. Hayes, UCECE
    @date   02 June 2007
    @brief 
*/
#ifndef TARGET_H
#define TARGET_H

#include "mat91lib.h"

/* CPU clocks  */
#define F_XTAL 18432000.0
#define CPU_PLL_MUL 125
#define CPU_PLL_DIV 24
#define CPU_USB_DIV 2
/* 96 MHz  */
#define F_PLL (F_XTAL / CPU_PLL_DIV * CPU_PLL_MUL)
/* 48 MHz  */
#define F_CPU (F_PLL / 2)

/* While loop rate */
#define PACER_RATE 1000

/* USB pullup MOSFET */
#define UDP_PULLUP_PIO PIO_DEFINE(PORT_A, 23)

/* Leds */
#define LED_GREEN_PIO PIO_DEFINE(PORT_A, 7)
#define LED_ORANGE_PIO PIO_DEFINE(PORT_A, 8)
#define LED_RED_PIO PIO_DEFINE(PORT_A, 9)
#define LED_BLUE_PIO PIO_DEFINE(PORT_A, 10)

/* I2C */
#define SDA_PIO PIO_DEFINE(PORT_A, 3)
#define SCL_PIO PIO_DEFINE(PORT_A, 4)

/* Bluetooth */
#define BLUETOOTH_RESET PIO_DEFINE(PORT_A, 19)
#define BLUETOOTH_UART_RXD PIO_DEFINE(PORT_A, 21)
#define BLUETOOTH_UART_TXD PIO_DEFINE(PORT_A, 22)
#define BLUETOOTH_UART_RTS PIO_DEFINE(PORT_A, 24)
#define BLUETOOTH_UART_CTS PIO_DEFINE(PORT_A, 25)

/* Buttons */
#define BUTTON1_PIO PIO_DEFINE(PORT_A, 27)
#define BUTTON2_PIO PIO_DEFINE(PORT_A, 28)

/* Dip Switch */
#define DIP_SW1 PIO_DEFINE(PORT_A, 15)
#define DIP_SW2 PIO_DEFINE(PORT_A, 16)
#define DIP_SW3 PIO_DEFINE(PORT_A, 17)
#define DIP_SW4 PIO_DEFINE(PORT_A, 18)

#endif /* TARGET_H  */
