IBP_DIR = $(WRLIB_DIR)/ibp

VPATH += $(IBP_DIR)
INCLUDES += -I$(IBP_DIR)

DRIVERS += bbi2c

SRC += ibp.c
