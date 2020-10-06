BUILDDIR = _build
TARGET = STM32F4xx

SDK_ROOT = ../../../../../..
PROJ_DIR = ..
HAL_DIR = $(SDK_ROOT)/Drivers/STM32F4xx_HAL_Driver
BSP_DIR = $(SDK_ROOT)/Drivers/BSP/STM32412G-Discovery
CMSIS_DIR = $(SDK_ROOT)/Drivers/CMSIS

SOURCES += \
		   $(HAL_DIR)/Src/stm32f4xx_hal.c \
		   $(HAL_DIR)/Src/stm32f4xx_hal_cortex.c \
		   $(HAL_DIR)/Src/stm32f4xx_hal_rcc.c \
		   $(HAL_DIR)/Src/stm32f4xx_hal_gpio.c \
		   $(BSP_DIR)/stm32412g_discovery.c \
		   $(PROJ_DIR)/Src/main.c \
		   $(PROJ_DIR)/Src/stm32f4xx_it.c \
		   $(PROJ_DIR)/Src/system_stm32f4xx.c \
		   $(PROJ_DIR)/build/startup_stm32f412zx.s

INCLUDES += -I$(HAL_DIR)/Inc \
			-I$(BSP_DIR)/ \
			-I$(PROJ_DIR)/Inc \
			-I$(CMSIS_DIR)/Device/ST/STM32F4xx/Include \
			-I$(CMSIS_DIR)/Include 

# name for output binary files
PROJECT ?= led

# compiler, objcopy (should be in PATH)
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# path to st-flash (or should be specified in PATH)
ST_FLASH ?= st-flash

# specify compiler flags
CFLAGS = -g -O2 -Wall
CFLAGS += -T$(PROJ_DIR)/build/stm32_flash.ld
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -DSTM32F412Zx -DUSE_STM32412G_DISCOVERY -DUSE_STDPERIPH_DRIVER
CFLAGS += -Wl,--gc-sections
CFLAGS += $(INCLUDES) 

OBJS = $(SOURCES:.c=.o)

all: $(PROJECT).elf

# compile
$(PROJECT).elf: $(SOURCES)
	$(CC) $(CFLAGS) $^ -o $@
	$(OBJCOPY) -O ihex $(PROJECT).elf $(PROJECT).hex
	$(OBJCOPY) -O binary $(PROJECT).elf $(PROJECT).bin

# remove binary files
clean:
	rm -f *.o *.elf *.hex *.bin

# flash
flash:
	sudo $(ST_FLASH) write $(PROJECT).bin 0x8000000
