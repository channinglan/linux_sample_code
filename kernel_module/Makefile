obj-m := 1_0_hello.o	\
	1_0_Linux_module_sample.o \
	1_1_hellop.o \
	1_1_module_parameters.o \
	2_0_reg_char_dev.o \
	7_0_timing.o \
	7_1_timer.o \
	7_2_tasklet.o	\
	7_3_workqueue.o
	
#	3_0_procfs_r.o \
#	3_0_procfs_w.o \
#	3_1_procfs_vector.o \
#	3_2_procfs_symlink.o \


KERNELDIR := /lib/modules/`uname -r`/build

all:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules

clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean
	rm -rf *.bak
