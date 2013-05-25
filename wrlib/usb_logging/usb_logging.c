#include <stdio.h>
#include <stdarg.h>
#include "usb_logging.h"
#include "usb_cdc.h"
#include "sys.h"
#include "config.h"

/*
 * Initialises the SAM7 as a USB-CDC device and routes stdin, stdout and stderr to the USB-CDC pipe
 */
extern void usb_logging_init(void){
    /* Create a USB CDC object and initialise the USB connection */
    usb_cdc_t usb_cdc;
    usb_cdc = usb_cdc_init ();
    
    /* Redirect stdin, stdout and stderr to the USB CDC pipe */
    sys_redirect_stdin ((void *)usb_cdc_read, usb_cdc);
    sys_redirect_stdout ((void *)usb_cdc_write, usb_cdc);
    sys_redirect_stderr ((void *)usb_cdc_write, usb_cdc);
}

/** 
 * Sends text over the USB connection to a host device listening to the serial port
 */
extern void usb_logging_send_text(char* prefix, char* file, int line, const char* message, ...){
    /* Poll the USB connection */
    usb_cdc_update ();
    
    /* Print the prefix */
    printf(prefix, file, line);

    /* Print the message */
    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);

    /* Print a newline 
           See here for explanation of rail return :
            http://en.wikipedia.org/wiki/Newline#In_programming_languages
    */
    printf ("\n\r");
}
