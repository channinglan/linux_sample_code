

ifneq ($(KERNELRELEASE),)

MODULE_NAME=pci_v4l2

${MODULE_NAME}-objs	:= ../v4l2_vivi/vivi.o pci_skeleton.o	 


obj-m		:= ${MODULE_NAME}.o 

else

PWD		:= $(shell pwd)

KDIR		:= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules


clean:
	rm -rf Module.symvers *.ko *.o *.mod.c .*.cmd .tmp_versions *.bak *.order

endif
