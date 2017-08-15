KVERS = $(shell uname -r)

# Kernel modules

obj-m += 4_1_hello.o \
	4_3_module_sample.o \
	4_5_module_parameters.o \
	4_6_export_symb.o \
	4_10_export_symb.o \
	4_10_hello.o	 

# Specify flags for the module compilation.
#EXTRA_CFLAGS=-g -O0

build: kernel_modules

kernel_modules:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) modules

clean:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) clean
	rm -rf *.bak
