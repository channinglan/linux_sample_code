
CONFIG_MODULE_FORCE_UNLOAD=y

ifneq ($(KERNELRELEASE),)

# debug build:
# "CFLAGS was changed ... Fix it to use EXTRA_CFLAGS."
EXTRA_CFLAGS=-Wall -Wmissing-prototypes -Wstrict-prototypes -g -O2


vivi_alsa-objs	:= vivi.o minivosc.o pci_skeleton.o
obj-m		:= vivi_alsa.o

else

PWD		:= $(shell pwd)

KDIR		:= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules



clean:
	rm -rf Module.symvers *.ko *.o *.mod.c .*.cmd .tmp_versions *.bak *.order

endif