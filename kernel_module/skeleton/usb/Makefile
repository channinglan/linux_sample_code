

ifneq ($(KERNELRELEASE),)


usb_test-objs	:= usb_skeleton.o	

obj-m		:= usb_test.o

else

PWD		:= $(shell pwd)

KDIR		:= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules



clean:
	rm -rf Module.symvers *.ko *.o *.mod.c .*.cmd .tmp_versions *.bak *.order

endif
