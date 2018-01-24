#!/bin/sh

MODULE_NAME=vivi_alsa

modprobe -r -v $MODULE_NAME

test -e /lib/modules/`uname -r`/kernel/drivers/media/video/$MODULE_NAME/$MODULE_NAME.ko && SEL=1 || SEL=0

if [ $SEL -eq 1 ]; then
KO_PATH=/lib/modules/`uname -r`/kernel/drivers/media/video/$MODULE_NAME
else
KO_PATH=/lib/modules/`uname -r`/kernel/drivers/media/pci/$MODULE_NAME
fi

rm -rf $KO_PATH
