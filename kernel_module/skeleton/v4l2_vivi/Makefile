

ifneq ($(KERNELRELEASE),)



vivi_v4l2-objs	:= vivi.o module.o
obj-m		:= vivi_v4l2.o

else

PWD		:= $(shell pwd)

KDIR		:= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules



clean:
	rm -rf Module.symvers *.ko *.o *.mod.c .*.cmd .tmp_versions *.bak *.order

endif
