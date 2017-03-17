CROSS_COMPILEPATH=../../linux-devkit/sysroots/x86_64-arago-linux/usr/bin/arm-linux-gnueabihf-
LINUX_SRC := ../../../board-support/linux-4.4.12+gitAUTOINC+3639bea54a-g3639bea54a


MODULE = power_fail
ifneq ($(KERNELRELEASE),)

obj-m += $(MODULE).o

else


PWD := $(shell pwd)

modules:
	$(MAKE) ARCH=arm CROSS_COMPILE=${CROSS_COMPILEPATH} -C $(LINUX_SRC) M=$(PWD) modules
	#cp $(MODULE).ko ../../tmp

clean:
	$(MAKE) -C $(LINUX_SRC) M=$(PWD) clean
	rm -rf *.bak
endif

