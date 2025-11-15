TARGET_EXEC ?= news
CURRENT_TARGET := coco

# Needed because of using sed on error messages
SHELL = /bin/bash -o pipefail

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

CC = cmoc

# Include external makefiles
include ../makefiles/fujinet-lib.mk

# Default tasks (from included makefiles)
# If the included files define ALL_TASKS, we’ll add them to the default target.
ALL_TASKS ?=

SRCS := $(shell find $(SRC_DIRS) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

OUTFLAGS = -o$(TARGET_EXEC).bin

LDFLAGS += $(foreach lib, $(LIBS), -L $(dir $(lib)))
LIBFLAGS := $(foreach lib,$(LIBS),-l$(notdir $(lib)))


INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Default target includes all tasks and final build
.PHONY: all $(ALL_TASKS)
all: $(ALL_TASKS) $(BUILD_DIR)/$(TARGET_EXEC)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(ORGFLAGS) $(OUTFLAGS) $(OBJS) $(LDFLAGS) $(LIBFLAGS)
	$(RM) $(TARGET_EXEC)
	decb dskini $(TARGET_EXEC).dsk
	writecocofile $(TARGET_EXEC).dsk $(TARGET_EXEC).bin
	cp $(TARGET_EXEC).dsk ~/tnfs/

# Compile source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	$(RM) -rf $(TARGET_EXEC)* build

-include $(DEPS)

MKDIR_P ?= mkdir -p
