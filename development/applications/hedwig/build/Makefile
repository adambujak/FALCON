TARGET = FALCON
DEBUG = 1
BUILD_DIR = _build
BUILD_CONFIG ?= DEBUG
BOARD_VERSION ?= V1


FALCON_ROOT = ../../../..
DEV_ROOT = $(FALCON_ROOT)/development
LIB_ROOT = $(DEV_ROOT)/libraries
DRV_ROOT = $(DEV_ROOT)/drivers
PROJ_ROOT = ..
COMMON_ROOT = $(PROJ_ROOT)/../common
FLN_BSP_ROOT = $(DEV_ROOT)/system/boards/stm_dev_bsp
SDK_ROOT = $(FALCON_ROOT)/dependencies/stm/STM32F4_Cube
FREERTOS_ROOT = $(FALCON_ROOT)/dependencies/freertos/FreeRTOSv10.4.1/FreeRTOS/Source
FREERTOS_PORTABLE = $(FREERTOS_ROOT)/portable/GCC/ARM_CM4F
HAL_ROOT = $(SDK_ROOT)/Drivers/STM32F4xx_HAL_Driver
CMSIS_ROOT = $(SDK_ROOT)/Drivers/CMSIS
FALCON_PACKET_ROOT = $(LIB_ROOT)/falcon_packet
LIBCRC_ROOT = $(LIB_ROOT)/libcrc
GRINBUF_ROOT = $(LIB_ROOT)/grinbuf

FALCON_PACKET_SOURCES = \
	$(FALCON_PACKET_ROOT)/falcon_packet.c \
	$(FALCON_PACKET_ROOT)/fs_decoder.c \
	$(FALCON_PACKET_ROOT)/ff_encoder.c \
	$(FALCON_PACKET_ROOT)/fp_decoder.c \
	$(FALCON_PACKET_ROOT)/fp_decode.c \
	$(FALCON_PACKET_ROOT)/fp_encode.c \
	$(GRINBUF_ROOT)/grinbuf.c \
	$(LIBCRC_ROOT)/src/crc16.c \

RADIO_SOURCES = \
	$(LIB_ROOT)/frf/frf.c \
	$(DRV_ROOT)/nRF24L01/nRF24L01.c \
	$(COMMON_ROOT)/radio_common.c \

IMU_SOURCES = \
	$(LIB_ROOT)/fimu/fimu.c \
	$(LIB_ROOT)/flight_control/flightController.c \
	$(LIB_ROOT)/flight_control/flightController_data.c \
	$(DRV_ROOT)/ICM20948/common/inv_mems_drv_hook.c \
	$(DRV_ROOT)/ICM20948/dmp3a/dmp3Default_20648.c \
	$(DRV_ROOT)/ICM20948/dmp3/inv_mems_interface_mapping.c \
	$(DRV_ROOT)/ICM20948/driver/inv_mems_augmented_sensors.c \
	$(DRV_ROOT)/ICM20948/driver/inv_mems_base_control.c \
	$(DRV_ROOT)/ICM20948/driver/inv_mems_base_driver.c \
	$(DRV_ROOT)/ICM20948/driver/inv_mems_data_converter.c \
	$(DRV_ROOT)/ICM20948/driver/inv_mems_load_firmware.c \
	$(DRV_ROOT)/ICM20948/driver/inv_mems_mpu_fifo_control.c \
	$(DRV_ROOT)/ICM20948/driver/inv_mems_mpu_selftest.c \
	$(DRV_ROOT)/ICM20948/driver/inv_mems_secondary_transport.c \
	$(DRV_ROOT)/ICM20948/driver/inv_mems_slave_compass.c \
	$(DRV_ROOT)/ICM20948/driver/inv_mems_slave_pressure.c \
	$(DRV_ROOT)/ICM20948/driver/inv_mems_transport.c \
	$(DRV_ROOT)/ICM20948/invn/common/ml_math_func.c \
	$(DRV_ROOT)/ICM20948/invn/common/mlmath.c \

BARO_SOURCES = \
	$(DRV_ROOT)/MPL3115A2/MPL3115A2.c \
	$(LIB_ROOT)/fbaro/fbaro.c \

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
	$(HAL_ROOT)/Src/stm32f4xx_hal_cortex.c \
	$(HAL_ROOT)/Src/stm32f4xx_hal_gpio.c \
	$(HAL_ROOT)/Src/stm32f4xx_hal_rcc.c \
	$(HAL_ROOT)/Src/stm32f4xx_hal_tim.c \
	$(HAL_ROOT)/Src/stm32f4xx_hal_tim_ex.c \
	$(HAL_ROOT)/Src/stm32f4xx_hal_spi.c \
	$(HAL_ROOT)/Src/stm32f4xx_hal_uart.c \
	$(HAL_ROOT)/Src/stm32f4xx_hal_i2c.c \
	$(HAL_ROOT)/Src/stm32f4xx_ll_tim.c \
	$(HAL_ROOT)/Src/stm32f4xx_ll_rcc.c \
	$(HAL_ROOT)/Src/stm32f4xx_ll_utils.c \

# C sources
C_SOURCES = \
	$(FLN_BSP_ROOT)/bsp.c \
	$(PROJ_ROOT)/src/main.c \
	$(PROJ_ROOT)/src/stm32f4xx_it.c \
	$(PROJ_ROOT)/src/system_stm32f4xx.c \
	$(PROJ_ROOT)/src/leds.c \
	$(PROJ_ROOT)/src/uart.c \
	$(PROJ_ROOT)/src/system_time.c \
	$(PROJ_ROOT)/src/device_com.c \
	$(PROJ_ROOT)/src/motors.c \
	$(PROJ_ROOT)/src/sensors.c \
	$(PROJ_ROOT)/src/flight_control.c \
	$(COMMON_ROOT)/logger/logger.c \
	$(FALCON_PACKET_SOURCES) \
	$(RADIO_SOURCES) \
	$(IMU_SOURCES) \
	$(BARO_SOURCES) \
	$(FREERTOS_SOURCES) \
	$(HAL_SOURCES) \

# ASM sources
ifeq ($(BOARD_VERSION), DISCO)
STARTUP_FILE = startup_stm32f412zx.s
else
STARTUP_FILE = startup_stm32f412rx.s
endif
ASM_SOURCES += $(STARTUP_FILE) 

# AS includes
AS_INCLUDES =

# C includes
C_INCLUDES += \
	-I$(HAL_ROOT)/Inc \
	-I$(FLN_BSP_ROOT)/ \
	-I$(LIB_ROOT)/frf/ \
	-I$(FALCON_PACKET_ROOT) \
	-I$(LIBCRC_ROOT)/include \
	-I$(GRINBUF_ROOT)/ \
	-I$(DRV_ROOT) \
	-I$(DRV_ROOT)/nRF24L01/ \
	-I$(PROJ_ROOT)/include \
	-I$(COMMON_ROOT) \
	-I$(COMMON_ROOT)/logger \
	-I$(CMSIS_ROOT)/Device/ST/STM32F4xx/Include \
	-I$(CMSIS_ROOT)/Include \
	-I$(FREERTOS_ROOT)/include \
	-I$(FREERTOS_PORTABLE) \
	-I$(DRV_ROOT)/ICM20948 \
	-I$(DRV_ROOT)/ICM20948/invn/common \
	-I$(DRV_ROOT)/ICM20948/driver \
	-I$(DRV_ROOT)/ICM20948/dmp3a \
	-I$(DRV_ROOT)/ICM20948/dmp3 \
	-I$(DRV_ROOT)/ICM20948/common \
	-I$(LIB_ROOT)/fimu \
	-I$(DEV_ROOT)/drivers/MPL3115A2 \
	-I$(LIB_ROOT)/fbaro \
	-I$(LIB_ROOT)/flight_control

# macros for gcc
# AS defines
AS_DEFS =

ifeq ($(BOARD_VERSION), DISCO)
CHIPSET_DEF = STM32F412Zx
C_DEFS += -DBOARD_DISCO
else
CHIPSET_DEF = STM32F412Rx
C_DEFS += -DBOARD_V1
endif

# C defines
C_DEFS +=  \
	-DHEDWIG \
	-DUSE_HAL_DRIVER \
	-DUSE_FULL_LL_DRIVER \
	-D$(CHIPSET_DEF) \
	-D$(BUILD_CONFIG) \
	-DMEMS_20648 \
	-DMEMS_SECONDARY_DEVICE \
	-DMEMS_AUGMENTED_SENSORS \

OPT = -O0 -g3

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

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -std=gnu11

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

#######################################
# clean up
#######################################
FLASH_SCRIPT = $(realpath $(DEV_ROOT))/scripts/falcon_flash.py
TARGET_PATH  = $(realpath $(BUILD_DIR)/$(TARGET).bin)

clean:
	-rm -fR $(BUILD_DIR)

flash: all
	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x8000000
	st-flash reset

pyflash: all
	@python3 $(FLASH_SCRIPT) $(TARGET_PATH) 0x8000000 

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***BUILDDIR = _build
