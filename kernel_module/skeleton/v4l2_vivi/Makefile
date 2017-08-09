

ifneq ($(KERNELRELEASE),)


vivi_v4l2-objs	:= vivi.o	


obj-m		:= vivi_v4l2.o 

else

PWD		:= $(shell pwd)

KDIR		:= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	modprobe -r vivi_v4l2
	#mkdir /lib/modules/`uname -r`/kernel/drivers/media/pci/vivi_v4l2/
	cp -rf vivi_v4l2.ko /lib/modules/`uname -r`/kernel/drivers/media/pci/vivi_v4l2/
	depmod -a
	modprobe -v vivi_v4l2



clean:
	rm -rf Module.symvers *.ko *.o *.mod.c .*.cmd .tmp_versions *.bak *.order

endif
