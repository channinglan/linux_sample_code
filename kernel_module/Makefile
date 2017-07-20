
obj-m := 10Linux_module_sample.o \
	161Network_Device_Driver_simple_snull.o \
	3_1_procfs_vector.o \
	3_2_procfs_symlink.o \
	3_0_procfs_r.o \
	3_0_procfs_w.o




#obj-m是告訴make這個module的name是什麼。
#KERNELDIR是告訴make這個module的kernel所在的位置。
#後面就接兩個target(all/clean)，用於處理產生和清除module用。


KERNELDIR := /lib/modules/`uname -r`/build

all:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules

clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean
	rm -rf *.bak