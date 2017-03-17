CROSS_COMPILEPATH=/home/share/PXM/ti-processor-sdk-linux-am57xx-evm-03.00.00.04
LINUX_SRC := ../linux-4.4.12+gitAUTOINC+3639bea54a-g3639bea54a


MODULE = pxm_ioint
ifneq ($(KERNELRELEASE),)

obj-m += $(MODULE).o

else


PWD := $(shell pwd)

modules:
	$(MAKE) ARCH=arm CROSS_COMPILE=${CROSS_COMPILEPATH}/linux-devkit/sysroots/x86_64-arago-linux/usr/bin/arm-linux-gnueabihf- -C $(LINUX_SRC) M=$(PWD) modules
	#cp $(MODULE).ko ../../tmp

clean:
	$(MAKE) -C $(LINUX_SRC) M=$(PWD) clean
	rm -rf *.bak
endif

