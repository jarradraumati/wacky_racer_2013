/**
 * @file   USB.h
 * @author Ashok Fernandez <azf10@uclive.ac.nz>
 * @date   16th March, 2013
 *
 * @brief  USB Logging Module
 *
 * Handles USB communication from the SAM7 to a connected PC. This allows
 * the boards to log messages over USB to aid in debugging and to get feedback
 * on the current operations being performed by the board.
 *
 * Messages can be logged at three different levels: 'info' for general information,
 * 'warning' for operations which are allowed but are potentially dangerous, and 'error'
 * for operations which are illegal
 *
 * On the SAM7 development board, this should be able to be connected to USB and print
 * info, warning and error messages to a connected PC
 */
#ifndef USB_LOGGING_H
#define USB_LOGGING_H

#include <stdarg.h> // Included for va_list

/** Prefixes to define different levels of logging */
#define INFO_PREFIX    "INFO    [file %s in line %d]  "
#define WARNING_PREFIX "WARNING [file %s in line %d]  "
#define ERROR_PREFIX   "ERROR   [file %s in line %d]  "

/** Convenience macros to log message at different levels and insert the file and line that the message was sent from */
#define USB_INFO(message, ...) usb_logging_send_text (INFO_PREFIX, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define USB_WARNING(message, ...) usb_logging_send_text (WARNING_PREFIX, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define USB_ERROR(message, ...) usb_logging_send_text (ERROR_PREFIX, __FILE__, __LINE__, message, ##__VA_ARGS__)

/**
 * @name    Init
 * @ingroup usb logging
 * @brief   Initialises the USB SAM7 as a USB device
 * Intialises the SAM7 as a USB-CDC device and redirects calls to stdout, stdin and stderr to the USB pipe
 *
 * Example Usage:
 * @code
 *    usb_logging_init() // Initialise the USB communication
 * @endcode
 */
extern void usb_logging_init(void);

/**
 * @name    Send text
 * @ingroup usb logging
 * @brief   Logs a message at the 'info' level
 *
 * @param [char *] 			prefix	Format string for printf that should contain %d and %s tokens for the line number a file respectivly
 * @param [char *] 			file	String containing the file name (insert __FILE__ and the compiler will fill this for you)
 * @param [int]	   			line 	Line number (insert __LINE__ and the compiler will fill this out for you)
 * @param [const char *] 	message	Main message to log, this can have printf style tokens such as %i, %s and %f
 * @param [va_list] args 	args 	Variables to replace printf tokens in the message
 *
 * Example Usage:
 * @code
 *    // Logs and info message over USB, recording the line and file
 *    usb_logging_send_text("INFO from %s on line %d", __FILE__, __LINE__, Initialising ADC%i as free running on PA%i", 1, 3); 
 * @endcode
 */
extern void usb_logging_send_text(char* prefix, char* file, int line, const char* message, ...);

#endif /* USB_LOGGING_H */