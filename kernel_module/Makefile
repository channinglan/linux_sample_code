mname := brook_modules
$(mname)-objs := 10Linux_module_sample.o 
obj-m := $(mname).o


#$(mname)-objs是告訴make這個module有哪些object files。
#obj-m是告訴make這個module的name是什麼。
#KERNELDIR是告訴make這個module的kernel所在的位置。
#後面就接兩個target(all/clean)，用於處理產生和清除module用。


KERNELDIR := /lib/modules/`uname -r`/build

all:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules

clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean
	rm -rf *.bak