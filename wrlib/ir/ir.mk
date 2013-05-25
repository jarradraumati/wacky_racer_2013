IR_DIR = $(WRLIB_DIR)/ir

VPATH += $(IR_DIR)
INCLUDES += -I$(IR_DIR)

DRIVERS += ir_rc5 

SRC += ir.c
