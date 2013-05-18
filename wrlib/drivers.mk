DRIVER_DIR = $(WRLIB_DIR)

include $(foreach driver, $(WR_DRIVERS), $(DRIVER_DIR)/$(driver)/$(driver).mk)

# Perform second pass for that drivers that depend on other drivers
include $(foreach driver, $(WR_DRIVERS), $(DRIVER_DIR)/$(driver)/$(driver).mk)

VPATH += $(DRIVER_DIR)/utility $(DRIVER_DIR)

INCLUDES += -I$(DRIVER_DIR)

