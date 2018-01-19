KVERS = $(shell uname -r)

# Kernel modules
obj-m += 10_3_3_tasklet.o	\
	 10_3_3_workqueue.o	\
	 10_5_1_timer.o		\
	 10_5_1_hrtimer.o	\
	 10_5_3_second.o
	 
	 
# Specify flags for the module compilation.
#EXTRA_CFLAGS=-g -O0

build: kernel_modules user_test

kernel_modules:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) modules
	
user_test:
	gcc -o second_test 10_5_3_second_test.c
	
clean:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) clean
	rm -rf *.bak second_test
