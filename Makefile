######################################
# Target
######################################
TARGET = firmware

######################################
# Building variables
######################################
# Debug build?
DEBUG = 1
# Optimization
OPT = -Og

######################################
# Paths
######################################
# Build directory
BUILD_DIR = build

######################################
# Source files
######################################
C_SOURCES = \
src/main.c \
src/uart.c \
src/delay.c \
src/syscalls.c \
src/timer.c \
src/gpio.c \
src/crc.c \
src/isr.c \
src/ring_buffer.c \
src/utils.c \
src/user_button.c \
src/spi_dma.c \
startup/startup_stm32f446re.c

# ASM sources
ASM_SOURCES =

######################################
# Binaries
######################################
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

######################################
# CFLAGS
######################################
# CPU
CPU = -mcpu=cortex-m4

# FPU
FPU = -mfpu=fpv4-sp-d16

# Float ABI
FLOAT-ABI = -mfloat-abi=hard

# MCU
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# AS defines
AS_DEFS =

# C defines
C_DEFS = \
-DSTM32F446xx

# AS includes
AS_INCLUDES =

# C includes
C_INCLUDES = \
-Iinclude \
-ICMSIS/Include \
-ICMSIS/Device/ST/STM32F4xx/Include

# Compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

######################################
# LDFLAGS
######################################
# Link script
LDSCRIPT = STM32F446RETx_FLASH.ld

# Libraries
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# Default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

######################################
# Build the application
######################################
# List of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

# List of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@

######################################
# Clean
######################################
clean:
	-rm -fR $(BUILD_DIR)

######################################
# Flash
######################################
#openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"
#openocd -f interface/stlink-v2-1.cfg -f target/stm32f4x.cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"
#openocd -f /usr/share/openocd/scripts/interface/stlink-v2-1.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"
flash: all	
	/usr/bin/openocd -f /usr/share/openocd/scripts/interface/stlink.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"

######################################
# Dependencies
######################################
-include $(wildcard $(BUILD_DIR)/*.d)
