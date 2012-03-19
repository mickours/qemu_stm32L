#Compilation du programme placé sur la carte:
make

#Execution dans Qemu
#./src/arm-softmmu/qemu-system-arm -M stm32l152rbt6 -nographic -kernel ./blink_32L.bin -S -gdb tcp::51234
./src/arm-softmmu/qemu-system-arm -M stm32l152rbt6 -nographic -kernel ./hello.bin
