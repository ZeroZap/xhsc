# /******************************************************************************
#  * Copyright (C) 2021, Xiaohua Semiconductor Co., Ltd. All rights reserved.
#  *
#  * This software component is licensed by XHSC under BSD 3-Clause license
#  * (the "License"); You may not use this file except in compliance with the
#  * License. You may obtain a copy of the License at:
#  *                    opensource.org/licenses/BSD-3-Clause
#  *
#  ******************************************************************************/
#
# /******************************************************************************
#  * @file   Makefile
#  *
#  * @brief  Makefile for HC32L19x series MCU (GCC)
#  *
#  * @author MADS Team
#  *
#  ******************************************************************************/

# ==============================================================================
# Project Configuration
# ==============================================================================
TARGET   := project
BUILD_DIR := build

# ==============================================================================
# Toolchain Configuration
# ==============================================================================
# GNU Arm Embedded Toolchain
# Download from: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm
PREFIX     := arm-none-eabi-
CC         := $(PREFIX)gcc
AS         := $(PREFIX)gcc -x assembler-with-cpp
CP         := $(PREFIX)objcopy
SZ         := $(PREFIX)size
AR         := $(PREFIX)ar
GDB        := $(PREFIX)gdb

# ==============================================================================
# Target Device Configuration
# ==============================================================================
# CPU: Cortex-M0+
CPU      := -mcpu=cortex-m0plus

# FPU: None (Cortex-M0+ has no FPU)
FPU      :=

# Float-abi: soft
FLOAT-ABI := -mfloat-abi=soft

# Thumb mode
THUMB    := -mthumb

# MCU specific flags
MCU      := $(CPU) $(FPU) $(FLOAT-ABI) $(THUMB)

# ==============================================================================
# Compiler Flags
# ==============================================================================
# C sources
CFLAGS    = $(MCU)
CFLAGS   += -Wall -Wextra -Wpedantic
CFLAGS   += -ffunction-sections -fdata-sections
CFLAGS   += -g3 -gdwarf-2
CFLAGS   += -O0

# Optimization level (change as needed)
# CFLAGS   += -O0    # No optimization
# CFLAGS   += -O1    # Optimize for size
# CFLAGS   += -O2    # Optimize for speed
# CFLAGS   += -Os    # Optimize for size
# CFLAGS   += -Ofast  # Fast optimization

# Include paths
CFLAGS   += -I$(PROJECT_ROOT)/mcu/common
CFLAGS   += -I$(PROJECT_ROOT)/driver/inc
CFLAGS   += -I$(PROJECT_ROOT)/example/$(EXAMPLE)

# ASM sources
ASFLAGS   = $(MCU)
ASFLAGS  += -Wall
ASFLAGS  += -ffunction-sections -fdata-sections
ASFLAGS  += -g3 -gdwarf-2
ASFLAGS  += -O0

# Linker flags
LDFLAGS   = $(MCU)
LDFLAGS  += -specs=nano.specs
LDFLAGS  += -Wl,--gc-sections
LDFLAGS  += -Wl,--print-memory-usage
LDFLAGS  += -T $(LDSCRIPT)
LDFLAGS  += -lm
LDFLAGS  += -lc
LDFLAGS  += -lnosys

# Linker script
LDSCRIPT := $(PROJECT_ROOT)/mcu/GCC/hc32l19x.ld

# ==============================================================================
# Source Files
# ==============================================================================
# DDL Driver sources
C_SOURCES += $(wildcard $(PROJECT_ROOT)/driver/src/*.c)

# MCU common sources
C_SOURCES += $(PROJECT_ROOT)/mcu/common/system_hc32l19x.c
C_SOURCES += $(PROJECT_ROOT)/mcu/common/interrupts_hc32l19x.c

# Startup file
ASM_SOURCES += $(PROJECT_ROOT)/mcu/GCC/startup_hc32l19x.S

# Example source (set EXAMPLE variable when invoking make)
EXAMPLE ?= template
C_SOURCES += $(wildcard $(PROJECT_ROOT)/example/$(EXAMPLE)/source/*.c)

# ==============================================================================
# Build Rules
# ==============================================================================
# Get list of C and ASM sources
C_FILES   := $(notdir $(C_SOURCES))
ASM_FILES := $(notdir $(ASM_SOURCES))

# Object files
C_OBJECTS  := $(addprefix $(BUILD_DIR)/,$(C_FILES:.c=.o))
ASM_OBJECTS := $(addprefix $(BUILD_DIR)/,$(ASM_FILES:.S=.o))
OBJECTS    := $(C_OBJECTS) $(ASM_OBJECTS)

# Dependency files
DEPS       := $(OBJECTS:.o=.d)

# ==============================================================================
# Default Target
# ==============================================================================
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

# ==============================================================================
# Build Rules
# ==============================================================================
# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link target
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) | $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS)
	$(SZ) $@

# Hex file
$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(CP) -O ihex $< $@

# Bin file
$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(CP) -O binary -S $< $@

# Compile C files
$(BUILD_DIR)/%.o: $(PROJECT_ROOT)/driver/src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(PROJECT_ROOT)/mcu/common/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(PROJECT_ROOT)/example/$(EXAMPLE)/source/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile ASM files
$(BUILD_DIR)/%.o: $(PROJECT_ROOT)/mcu/GCC/%.S | $(BUILD_DIR)
	$(AS) $(ASFLAGS) -c -o $@ $<

# Clean build
clean:
	rm -rf $(BUILD_DIR)

# Flash using OpenOCD
flash: $(BUILD_DIR)/$(TARGET).elf
	openocd -f interface/cmsis-dap.cfg -f target/hc32l19x.cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"

# Debug with GDB
debug: $(BUILD_DIR)/$(TARGET).elf
	$(GDB) $<

# Include dependency files
-include $(DEPS)

# ==============================================================================
# Help
# ==============================================================================
help:
	@echo "HC32L19x GCC Makefile"
	@echo ""
	@echo "Usage:"
	@echo "  make [EXAMPLE=<example_name>] [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all     - Build ELF, HEX, and BIN files (default)"
	@echo "  clean   - Remove build files"
	@echo "  flash   - Flash to target using OpenOCD"
	@echo "  debug   - Start GDB debugging session"
	@echo "  help    - Show this help"
	@echo ""
	@echo "Examples:"
	@echo "  make EXAMPLE=template"
	@echo "  make EXAMPLE=gpio/gpio_output"
	@echo "  make EXAMPLE=uart/uart_poll clean"
	@echo "  make EXAMPLE=gpio/gpio_output flash"

.PHONY: all clean flash debug help
