BIN=myprogram

TOOLS_PATH=/usr/
TOOLS_PREFIX=arm-none-eabi-
TOOLS_VERSION=4.8.4

CFLAGS=-c -mcpu=cortex-m3 -mthumb -Wall -O2 -mapcs-frame -D__thumb2__=1 
CFLAGS+=-msoft-float -gdwarf-2 -mno-sched-prolog -fno-hosted -mtune=cortex-m3 
CFLAGS+=-march=armv7-m -mfix-cortex-m3-ldrd -ffunction-sections -fdata-sections 
CFLAGS+=-I./cmsis -I./stm32_lib -I.
ASFLAGS=-mcpu=cortex-m3 -I./cmsis -I./stm32_lib -gdwarf-2 -gdwarf-2
LDFLAGS=-static -mcpu=cortex-m3 -mthumb -mthumb-interwork -Wl,--start-group 
LDFLAGS+=-L$(TOOLS_PATH)/lib/gcc/arm-none-eabi/$(TOOLS_VERSION)/thumb2 
LDFLAGS+=-L$(TOOLS_PATH)/arm-none-eabi/lib/thumb2 -lc -lg -lgcc -lm 
#LDFLAGS+=--section-start=.text=0x8000000
LDFLAGS+=-Wl,--end-group -Xlinker -Map -Xlinker $(BIN).map -Xlinker 
LDFLAGS+=-T ./stm32_lib/device_support/gcc/stm32f100rb_flash.ld -nostartfiles -Wl,--gc-sections -o $(BIN).elf

CC=$(TOOLS_PATH)/bin/$(TOOLS_PREFIX)gcc-$(TOOLS_VERSION)
AS=$(TOOLS_PATH)/bin/$(TOOLS_PREFIX)as
SIZE=$(TOOLS_PATH)/bin/$(TOOLS_PREFIX)size
OBJCOPY=$(TOOLS_PATH)/bin/$(TOOLS_PREFIX)objcopy

CMSISSRC=./cmsis/core_cm3.c
STM32_LIBSRC=./stm32_lib/system_stm32f10x.c ./stm32_lib/stm32f10x_it.c
STM32_LIBSRC+=./stm32_lib/stm32f10x_rcc.c ./stm32_lib/stm32f10x_gpio.c ./stm32_lib/stm32f10x_usart.c ./stm32_lib/stm32f10x_adc.c ./stm32_lib/stm32f10x_tim.c ./stm32_lib/misc.c ./stm32_lib/stm32f10x_spi.c ./stm32_lib/stm32f10x_dma.c ./stm32_lib/stm32f10x_dac.c

FATSRC=./fatfs/ff.c ./fatfs/sd_spi_stm32.c

SRC=main.c

OBJ=core_cm3.o system_stm32f10x.o startup_stm32f10x_md_vl.o
OBJ+=stm32f10x_rcc.o stm32f10x_gpio.o stm32f10x_usart.o  stm32f10x_adc.o stm32f10x_tim.o misc.o 
OBJ+=usart.o queue.o main.o adc.o dac_play.o xprintf.o sqrt.o stm32f10x_spi.o stm32f10x_dma.o stm32f10x_dac.o wav_file.o
FATFSOBJ1=$(subst .c,.o,$(FATSRC)) 
FATFSOBJ=$(subst ./fatfs/,,$(FATFSOBJ1))

all: $(BIN).elf


$(BIN).elf: ccmsis cstm32_lib cc ldall fatfs
	$(SIZE) -B $(BIN).elf

ccmsis: $(CMSISSRC)
	$(CC) $(CFLAGS) $(CMSISSRC)

cstm32_lib: $(STM32_LIBSRC)
	$(CC) $(CFLAGS) $(STM32_LIBSRC)
	$(AS) $(ASFLAGS) ./stm32_lib/device_support/gcc/startup_stm32f10x_md_vl.S -o startup_stm32f10x_md_vl.o

fatfs: $(FATFSOBJ)

$(FATFSOBJ): $(FATSRC)
	$(CC) $(CFLAGS) $(FATSRC)

cc: $(SRC)
	$(CC) $(CFLAGS) $(SRC)

ldall: $(OBJ) $(FATFSOBJ)
	$(CC) $(OBJ) $(FATFSOBJ) $(LDFLAGS)

%.bin:%.elf
	$(OBJCOPY) -Obinary $(*).elf $(*).bin 

%.o:%.c
	$(CC) $(CFLAGS) $<

fatfs/ff.c: fatfs/ffconf.h fatfs/ff.h

.PHONY: clean load

clean:
	rm -f 	$(OBJ) \
		$(BIN).map \
		$(BIN).elf
	make clean -C ./stlink/build/

load: $(BIN).elf
	./flashing_stm32vldiscovery.sh $(BIN).elf
