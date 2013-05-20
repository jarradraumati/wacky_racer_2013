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

#define UDP_PULLUP_PIO PIO_DEFINE(PORT_A, 23)

#define LED1_PIO PIO_DEFINE(PORT_A, 7)
#define LED2_PIO PIO_DEFINE(PORT_A, 8)
#define LED3_PIO PIO_DEFINE(PORT_A, 9)
#define LED4_PIO PIO_DEFINE(PORT_A, 10)

#define SDA_PIO PIO_DEFINE(PORT_A, 3)
#define SCL_PIO PIO_DEFINE(PORT_A, 4)

#endif /* TARGET_H  */
