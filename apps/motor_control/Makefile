RUN_MODE = ROM

OPT = -Os

MMCULIB_DIR = ../../mmculib

WRLIB_DIR = ../../wrlib

PERIPHERALS = pit extint pwm

DRIVERS = button pacer usb_cdc

WR_DRIVERS = kernel

TARGET = motor_control.bin

SRC = motor_control.c

include ../../boards/motor/board.mk
include $(WRLIB_DIR)/wrlib.mk
include $(MMCULIB_DIR)/mmculib.mk


