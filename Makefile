CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

DEF_CFLAGS=-g -mlittle-endian -mthumb -ffreestanding -nostdlib -nostdinc

# to run from SRAM
DEF_CFLAGS+=-Wl,-Ttext,0x20000000 -Wl,-e,0x20000000

CFLAGS_L=$(DEF_CFLAGS) -mcpu=cortex-m3 -DCONFIG_STM32L_DISCOVERY

all: blink_32L.elf

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

blink_32L.elf: main.c
	$(CC) $(CFLAGS_L) $^ -o $@

clean:
	rm -rf *.elf
	rm -rf *.bin

.PHONY: all clean
