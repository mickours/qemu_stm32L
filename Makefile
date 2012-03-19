# Makefile for building basic STM32 projects
#
# Michael Hope <michaelh@juju.net.nz>, 2010
#

# Using the CodeSourcery 2009q3 Lite ARM EABI tools
CROSS_COMPILE = /home/mickours/App/CodeBench/bin/arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy

# Name of the startup file.  'hd' for high density
STARTUP = startup_stm32l1xx_md.s
# Build for the Cortex-M3 with basic optimisations and debugging
CFLAGS = -mcpu=cortex-m3 -mthumb -O -g

# List of all binaries to build
all: hello.bin

# Create a raw binary file from the ELF version
hello.bin: hello.elf
	$(OBJCOPY) -O binary $< $@

# Create the ELF version by mixing together the startup file,
# application, and linker file
hello.elf: $(STARTUP) hello.c
	$(CC) -o $@ $(CFLAGS) -nostartfiles -Wl,-Tstm32.ld $^

# Program the binary to the board using the builtin serial bootloader
program:
	stm32loader.py -p /dev/ttyUSB0 -ewv hello.bin

# Remove the temporary files
clean:
	rm -f *.o *.elf *.bin

