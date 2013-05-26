BLUETOOTH_DIR = $(WRLIB_DIR)/bluetooth

VPATH += $(BLUETOOTH_DIR)
INCLUDES += -I$(BLUETOOTH_DIR)

DRIVERS += busart

SRC += bluetooth.c
