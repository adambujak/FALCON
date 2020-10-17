TARGET = FALCON
DEBUG = 1
OPT =
BUILD_DIR = _build
BUILD_CONFIG ?= DEBUG


FALCON_ROOT = ../../../..
DEV_ROOT = $(FALCON_ROOT)/development
LIB_ROOT = $(DEV_ROOT)/libraries
PROJ_DIR = ..
FLN_BSP_DIR = $(DEV_ROOT)/system/boards/stm_dev_bsp
SDK_ROOT = $(FALCON_ROOT)/dependencies/stm/STM32F4_Cube
FREERTOS_ROOT = $(FALCON_ROOT)/dependencies/freertos/FreeRTOSv10.4.1/FreeRTOS/Source
FREERTOS_PORTABLE = $(FREERTOS_ROOT)/portable/GCC/ARM_CM4F
HAL_DIR = $(SDK_ROOT)/Drivers/STM32F4xx_HAL_Driver
BSP_DIR = $(SDK_ROOT)/Drivers/BSP/STM32412G-Discovery
CMSIS_DIR = $(SDK_ROOT)/Drivers/CMSIS

######################################
# source
######################################
# C sources
C_SOURCES += \
$(HAL_DIR)/Src/stm32f4xx_hal.c \
$(HAL_DIR)/Src/stm32f4xx_hal_cortex.c \
$(HAL_DIR)/Src/stm32f4xx_hal_gpio.c \
$(HAL_DIR)/Src/stm32f4xx_hal_rcc.c \
$(HAL_DIR)/Src/stm32f4xx_hal_tim.c \
$(HAL_DIR)/Src/stm32f4xx_hal_tim_ex.c \
$(HAL_DIR)/Src/stm32f4xx_hal_uart.c \
$(BSP_DIR)/stm32412g_discovery.c \
$(FLN_BSP_DIR)/bsp.c \
$(FREERTOS_ROOT)/tasks.c \
$(FREERTOS_ROOT)/queue.c \
$(FREERTOS_ROOT)/list.c \
$(FREERTOS_ROOT)/timers.c \
$(FREERTOS_ROOT)/event_groups.c \
$(FREERTOS_ROOT)/stream_buffer.c \
$(FREERTOS_ROOT)/portable/MemMang/heap_4.c \
$(FREERTOS_PORTABLE)/port.c \
$(PROJ_DIR)/src/main.c \
$(PROJ_DIR)/src/stm32f4xx_it.c \
$(PROJ_DIR)/src/system_stm32f4xx.c \
$(PROJ_DIR)/src/leds.c \
$(PROJ_DIR)/src/logger.c \

# ASM sources
ASM_SOURCES = startup_stm32f412zx.s

# AS includes
AS_INCLUDES =

# C includes
C_INCLUDES += \
			-I$(HAL_DIR)/Inc \
			-I$(BSP_DIR)/ \
			-I$(FLN_BSP_DIR)/ \
			-I$(PROJ_DIR)/include \
			-I$(CMSIS_DIR)/Device/ST/STM32F4xx/Include \
			-I$(CMSIS_DIR)/Include \
 			-I$(FREERTOS_ROOT)/include \
 			-I$(FREERTOS_PORTABLE)

# macros for gcc
# AS defines
AS_DEFS =

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F412Zx \
-DUSE_STM32412G_DISCOVERY \
-D$(BUILD_CONFIG)




#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)


# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -Werror -fdata-sections -ffunction-sections -std=gnu11

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = $(PROJ_DIR)/build/stm32_flash.ld

# libraries
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -u _printf_float -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
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

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)

flash:
	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x8000000
	st-flash reset

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***BUILDDIR = _build
