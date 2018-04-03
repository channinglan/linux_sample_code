#!/bin/bash


MODULE_NAME=pci_memory

rmmod ${MODULE_NAME}
#modprobe -r -v ${MODULE_NAME}
rmmod ${MODULE_NAME}
#yum install -y kernel-headers-`uname -r`
#yum install -y kernel-devel-`uname -r`


mkdir /lib/modules/`uname -r`/kernel/drivers/media/pci/${MODULE_NAME}/

make  clean
make

#cp -rf ${MODULE_NAME}.ko 	/lib/modules/`uname -r`/kernel/drivers/media/pci/${MODULE_NAME}/${MODULE_NAME}.ko
#ls -al /lib/modules/`uname -r`/kernel/drivers/media/pci/${MODULE_NAME}/${MODULE_NAME}.ko
#depmod -a
insmod ${MODULE_NAME}.ko
#modprobe -v ${MODULE_NAME}
lsmod | grep ${MODULE_NAME}



