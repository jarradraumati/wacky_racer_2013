# Need to define:
# MCU name of microcontroller
# DRIVERS list of drivers to build
# PERIPHERALS list of peripherals to build
# SRC list of additional C source files

ifndef MCU
$(error MCU undefined, this needs to be defined in the Makefile)
endif

# Include Files in this folder
INCLUDES += -I$(WRLIB_DIR)/

# Include the architecture independent driver dependencies.
include $(WRLIB_DIR)/drivers.mk


