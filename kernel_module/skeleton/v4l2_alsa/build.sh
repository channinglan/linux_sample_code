#!/bin/bash


MODULE_NAME=vivi_alsa

modprobe -r -v ${MODULE_NAME}

#yum install -y kernel-headers-`uname -r`
#yum install -y kernel-devel-`uname -r`


mkdir /lib/modules/`uname -r`/kernel/drivers/media/pci/${MODULE_NAME}/

make  clean
make

cp -rf ${MODULE_NAME}.ko 	/lib/modules/`uname -r`/kernel/drivers/media/pci/${MODULE_NAME}/${MODULE_NAME}.ko

depmod -a

modprobe -v ${MODULE_NAME}
lsmod | grep ${MODULE_NAME}

