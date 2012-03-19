#Compilation du programme placé sur la carte:
arm-none-eabi-gcc -g -c -mcpu=cortex-m3 -mthumb main.c -o main.o
arm-none-eabi-gcc -g -c -mcpu=cortex-m3 -mthumb startup_stm32l1xx_md.s -o startup_stm32l1xx_md.o
arm-none-eabi-ld -T stm32_flash.ld  startup_stm32l1xx_md.o main.o -o main.elf
arm-none-eabi-objcopy -O binary main.elf main.bin
rm *.o

#Execution dans Qemu
#./src/arm-softmmu/qemu-system-arm -M stm32l152rbt6 -nographic -kernel ./main.bin -S -gdb tcp::51234
./src/arm-softmmu/qemu-system-arm -M stm32l152rbt6 -nographic -chardev socket,id=stm32_gpio,port=4242,host=localhost -kernel ./main.bin
