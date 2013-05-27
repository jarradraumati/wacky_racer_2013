RUN_MODE = ROM

OPT = -Os

MMCULIB_DIR = ../../mmculib

WRLIB_DIR = ../../wrlib

PERIPHERALS = adc

DRIVERS = button usb_cdc

WR_DRIVERS = kernel motor steering ir sleep usb_logging

TARGET = motor_board.bin

SRC = motor_board.c

include ../../boards/motor/board.mk
include $(WRLIB_DIR)/wrlib.mk
include $(MMCULIB_DIR)/mmculib.mk


