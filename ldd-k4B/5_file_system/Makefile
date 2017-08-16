KVERS = $(shell uname -r)

# Kernel modules

obj-m += 4_1_hello.o


# Specify flags for the module compilation.
#EXTRA_CFLAGS=-g -O0

build: kernel_modules test
test:
	gcc -o 5_1_file_op 5_1_file_op.c	


kernel_modules:
#	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) modules

clean:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) clean
	rm -rf *.bak 5_1_file_op
