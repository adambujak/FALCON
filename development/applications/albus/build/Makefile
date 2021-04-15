TARGET = ALBUS
BUILD_CONFIG ?= DEBUG
BOARD_VERSION ?= DISCO

# Build path
BUILD_DIR = _build
FALCON_ROOT = ../../../..
DEV_ROOT = $(FALCON_ROOT)/development
LIB_ROOT = $(DEV_ROOT)/libraries
DRV_ROOT = $(DEV_ROOT)/drivers
PROJ_ROOT = ..
COMMON_ROOT = $(PROJ_ROOT)/../common
SDK_ROOT = $(FALCON_ROOT)/dependencies/stm/STM32F4_Cube
FREERTOS_ROOT = $(FALCON_ROOT)/dependencies/freertos/FreeRTOSv10.4.1/FreeRTOS/Source
FREERTOS_PORTABLE = $(FREERTOS_ROOT)/portable/GCC/ARM_CM4F
HAL_ROOT = $(SDK_ROOT)/Drivers/STM32F4xx_HAL_Driver
CMSIS_ROOT = $(SDK_ROOT)/Drivers/CMSIS
FALCON_PACKET_ROOT = $(LIB_ROOT)/falcon_packet
LIBCRC_ROOT = $(LIB_ROOT)/libcrc
GRINBUF_ROOT = $(LIB_ROOT)/grinbuf

RADIO_SOURCES = \
	$(LIB_ROOT)/frf/frf.c \
	$(DRV_ROOT)/nRF24L01/nRF24L01.c \
	$(COMMON_ROOT)/radio_common.c \

FALCON_PACKET_SOURCES = \
	$(FALCON_PACKET_ROOT)/falcon_packet.c \
	$(FALCON_PACKET_ROOT)/fs_decoder.c \
	$(FALCON_PACKET_ROOT)/ff_encoder.c \
	$(FALCON_PACKET_ROOT)/fp_decoder.c \
	$(FALCON_PACKET_ROOT)/fp_decode.c \
	$(FALCON_PACKET_ROOT)/fp_encode.c \
	$(GRINBUF_ROOT)/grinbuf.c \
	$(LIBCRC_ROOT)/src/crc16.c \

FREERTOS_SOURCES = \
	$(FREERTOS_ROOT)/tasks.c \
	$(FREERTOS_ROOT)/queue.c \
	$(FREERTOS_ROOT)/list.c \
	$(FREERTOS_ROOT)/timers.c \
	$(FREERTOS_ROOT)/event_groups.c \
	$(FREERTOS_ROOT)/stream_buffer.c \
	$(FREERTOS_ROOT)/portable/MemMang/heap_4.c \
	$(FREERTOS_PORTABLE)/port.c \

HAL_SOURCES = \
	$(HAL_ROOT)/Src/stm32f4xx_hal.c \
	$(HAL_ROOT)/Src/stm32f4xx_hal_gpio.c \
	$(HAL_ROOT)/Src/stm32f4xx_hal_rcc.c \
	$(HAL_ROOT)/Src/stm32f4xx_hal_spi.c \
	$(HAL_ROOT)/Src/stm32f4xx_ll_gpio.c \
	$(HAL_ROOT)/Src/stm32f4xx_ll_usart.c \
	$(HAL_ROOT)/Src/stm32f4xx_ll_tim.c \
	$(HAL_ROOT)/Src/stm32f4xx_ll_dma.c \
	$(HAL_ROOT)/Src/stm32f4xx_ll_rcc.c \
	$(HAL_ROOT)/Src/stm32f4xx_ll_utils.c \
	$(HAL_ROOT)/Src/stm32f4xx_ll_exti.c \

# C sources
C_SOURCES =  \
	$(PROJ_ROOT)/src/main.c \
	$(PROJ_ROOT)/src/uart.c \
	$(PROJ_ROOT)/src/gpio.c \
	$(PROJ_ROOT)/src/spi.c \
	$(PROJ_ROOT)/src/device_com.c \
	$(PROJ_ROOT)/src/radio.c \
	$(PROJ_ROOT)/src/stm32f4xx_it.c \
	$(PROJ_ROOT)/src/system_stm32f4xx.c \
	$(LIB_ROOT)/fast_fifo/fifo.c \
	$(COMMON_ROOT)/logger/logger.c \
	$(COMMON_ROOT)/system_time/system_time.c \
	$(RADIO_SOURCES) \
	$(FALCON_PACKET_SOURCES) \
	$(HAL_SOURCES) \
	$(FREERTOS_SOURCES) \

# C includes
C_INCLUDES += \
	-I$(HAL_ROOT)/Inc \
	-I$(LIB_ROOT)/frf/ \
	-I$(LIB_ROOT)/fast_fifo/ \
	-I$(DRV_ROOT) \
	-I$(FALCON_PACKET_ROOT) \
	-I$(LIBCRC_ROOT)/include \
	-I$(GRINBUF_ROOT) \
	-I$(DRV_ROOT)/nRF24L01/ \
	-I$(PROJ_ROOT)/include \
	-I$(COMMON_ROOT) \
	-I$(COMMON_ROOT)/logger \
	-I$(COMMON_ROOT)/system_time \
	-I$(CMSIS_ROOT)/Device/ST/STM32F4xx/Include \
	-I$(CMSIS_ROOT)/Include \
	-I$(FREERTOS_ROOT)/include \
	-I$(FREERTOS_PORTABLE) \


ifeq ($(BOARD_VERSION), DISCO)
STARTUP_FILE = startup_stm32f412zx.s
else
STARTUP_FILE = startup_stm32f412rx.s
endif
# ASM sources
ASM_SOURCES += $(STARTUP_FILE)

# AS includes
AS_INCLUDES =


PREFIX = arm-none-eabi-
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

# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS =

ifeq ($(BOARD_VERSION), DISCO)
CHIPSET_DEF = STM32F412Zx
C_DEFS += -DUSE_STM32412G_DISCOVERY
C_DEFS += -DBOARD_DISCO
else
CHIPSET_DEF = STM32F412Rx
C_DEFS += -DBOARD_V1
endif

# C defines
C_DEFS +=  \
	-DALBUS \
	-DUSE_HAL_DRIVER \
	-DUSE_FULL_LL_DRIVER \
	-DLOG_LEVEL=LOG_LEVEL_DEBUG \
	-D$(CHIPSET_DEF) \
	-D$(BUILD_CONFIG) \
	-DHSE_VALUE=8000000 \
	-DHSE_STARTUP_TIMEOUT=100 \
	-DLSE_STARTUP_TIMEOUT=5000 \
	-DLSE_VALUE=32768 \
	-DEXTERNAL_CLOCK_VALUE=12288000 \
	-DHSI_VALUE=16000000 \
	-DLSI_VALUE=32000 \
	-DPREFETCH_ENABLE=1 \
	-DINSTRUCTION_CACHE_ENABLE=1 \

OPT = -O0 -g3


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
LDSCRIPT = $(PROJ_ROOT)/build/stm32_flash.ld

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
	@echo compiling $<
	@$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@echo assembling $<
	@$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	@echo linking $<
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@

clean:
	-rm -fR $(BUILD_DIR)

flash: all
	# retry st-flash on failure
	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x8000000 || st-flash write $(BUILD_DIR)/$(TARGET).bin 0x8000000
	st-flash reset

-include $(wildcard $(BUILD_DIR)/*.d)
