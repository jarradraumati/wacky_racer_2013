USB_LOGGING_DIR = $(WRLIB_DIR)/usb_logging

VPATH += $(USB_LOGGING_DIR)
INCLUDES += -I$(USB_LOGGING_DIR)

PERIPHERALS += pit

DRIVERS += usb_cdc

SRC += usb_logging.c
