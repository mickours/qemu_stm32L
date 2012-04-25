all:main_qemu.bin main_hw.bin


#Génération de l'éxecutable pour Qemu
main_qemu.bin: main_qemu.elf
	arm-none-eabi-objcopy -O binary main_qemu.elf main_qemu.bin
	
main_qemu.elf: main.o startup_stm32l1xx_md.o stm32_flash_Qemu.ld
	arm-none-eabi-ld -T stm32_flash_Qemu.ld  startup_stm32l1xx_md.o main.o -o main_qemu.elf



#Génération de l'éxecutable pour le Hardware
main_hw.bin: main_hw.elf
	arm-none-eabi-objcopy -O binary main_hw.elf main_hw.bin
	
main_hw.elf: main.o startup_stm32l1xx_md.o stm32_flash_HW.ld
	arm-none-eabi-ld -T stm32_flash_HW.ld  startup_stm32l1xx_md.o main.o -o main_hw.elf



#Génération des fichiers Objets (Commun à Qemu et HW)
main.o: main.c
	arm-none-eabi-gcc -g -c -mcpu=cortex-m3 -mthumb main.c -o main.o

startup_stm32l1xx_md.o: startup_stm32l1xx_md.s
	arm-none-eabi-gcc -g -c -mcpu=cortex-m3 -mthumb startup_stm32l1xx_md.s -o startup_stm32l1xx_md.o



#Clean
clean:
	rm -f *.o

mrproper: clean
	rm -f *.elf *.bin
