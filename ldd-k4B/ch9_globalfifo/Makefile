KVERS = $(shell uname -r)

# Kernel modules
obj-m += globalfifo.o

# Specify flags for the module compilation.
EXTRA_CFLAGS=-g -O0

build: kernel_modules user_test aio_test

kernel_modules:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) modules
user_test:
	gcc -o globalfifo_test globalfifo_test.c
	
aio_test:	
	#apt-get install libaio-dev
	gcc -o aior aior.c -laio
clean:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) clean
	rm globalfifo_test aior *.o  *.bak
