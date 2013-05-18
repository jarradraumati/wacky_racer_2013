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


#define LED1_PIO PIO_DEFINE(PORT_A, 28)
#define LED2_PIO PIO_DEFINE(PORT_A, 29)

#define TCM8230_RESET_PIO PIO_DEFINE(PORT_A, 26)

#define TCM8230_VD_PIO PIO_DEFINE(PORT_A, 13)
#define TCM8230_HD_PIO PIO_DEFINE(PORT_A, 14)
#define TCM8230_DCLK_PIO PIO_DEFINE(PORT_A, 15)
#define TCM8230_EXTCLK_PIO PIO_DEFINE(PORT_A, 25)

#define TCM8230_DATA_PIOBUS PIOBUS_DEFINE(PORT_A, 16, 23)

#define TCM8230_SDA_PIO PIO_DEFINE(PORT_A, 3)
#define TCM8230_SCL_PIO PIO_DEFINE(PORT_A, 4)

#define UDP_PULLUP_PIO PIO_DEFINE(PORT_A, 31)

#endif /* TARGET_H  */
