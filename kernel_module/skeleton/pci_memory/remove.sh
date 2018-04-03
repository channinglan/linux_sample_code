#!/bin/sh

MODULE_NAME=pci_v4l2

modprobe -r -v $MODULE_NAME

test -e /lib/modules/`uname -r`/kernel/drivers/media/video/$MODULE_NAME/$MODULE_NAME.ko && SEL=1 || SEL=0

if [ $SEL -eq 1 ]; then
KO_PATH=/lib/modules/`uname -r`/kernel/drivers/media/video/$MODULE_NAME
else
KO_PATH=/lib/modules/`uname -r`/kernel/drivers/media/pci/$MODULE_NAME
fi
