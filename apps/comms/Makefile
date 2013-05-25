RUN_MODE = ROM

OPT = -Os

MMCULIB_DIR = ../../mmculib

WRLIB_DIR = ../../wrlib

PERIPHERALS = 

DRIVERS = busart bbi2c usb_cdc pacer

WR_DRIVERS = kernel usb_logging

TARGET = comms.bin

SRC = comms.c

include ../../boards/comms/board.mk
include $(WRLIB_DIR)/wrlib.mk
include $(MMCULIB_DIR)/mmculib.mk

