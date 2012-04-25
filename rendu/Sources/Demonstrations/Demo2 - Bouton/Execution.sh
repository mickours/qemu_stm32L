#!/bin/bash

echo "################################"
echo "# Démonstration 2"
echo "################################"

#Compilation du programme de démonstration
echo "#Compilation du programme Démo 2 ..."
make
exitVal=$?
if [ $exitVal != "0" ] 
then
	exit 1
fi

#Déplacement vers le répertoire Qemu
echo "#Déplacement vers le répertoire Qemu"
cd ../../Qemu/src

##Compilation de Qemu
#echo "#Compilation du programme Qemu"
#make
#exitVal=$?
#if [ $exitVal != "0" ] 
#then
#	exit 1
#fi

#Execution de Qemu
echo "#Lancement de Qemu ..."
./arm-softmmu/qemu-system-arm -M stm32l152rbt6 -nographic -chardev socket,id=led_green,port=4242,host=localhost,nodelay -chardev socket,id=led_blue,port=4243,host=localhost,nodelay -chardev socket,id=user0,port=4244,host=localhost,nodelay -kernel ../../Demonstrations/Demo2\ -\ Bouton/main_qemu.bin
exit 0

