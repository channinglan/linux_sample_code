

ifneq ($(KERNELRELEASE),)

MODULE_NAME=pci_memory

${MODULE_NAME}-objs	:= pci_core.o 	pci_cdev.o  


obj-m		:= ${MODULE_NAME}.o 

else

PWD		:= $(shell pwd)

KDIR		:= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules


clean:
	rm -rf Module.symvers *.ko *.o *.mod.c .*.cmd .tmp_versions *.bak *.order

endif
