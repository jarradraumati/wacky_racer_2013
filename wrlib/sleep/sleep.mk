SLEEP_DIR = $(WRLIB_DIR)/sleep

VPATH += $(SLEEP_DIR)
INCLUDES += -I$(SLEEP_DIR)

PERIPHERALS += extint

SRC += sleep.c
