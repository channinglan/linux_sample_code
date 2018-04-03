# Penbex Player.



EXEC = pci_test


OBJS2 =pci_test.o

export ROOTDIR:=$(CURDIR)
export BUILDDIR=$(ROOTDIR)
export OBJDIR=$(ROOTDIR)
#export TOOLSDIR=$(ROOTDIR)
LIBC_PATH	=
KERNEL_PATH	=

CROSS_COMPILE	= 


export CC		= $(CROSS_COMPILE)gcc
#export LD		= $(CC) -nostdlib
export LD		= $(CROSS_COMPILE)ld
#export CPP		= $(CC) -E
export CPP		= $(CROSS_COMPILE)cpp
export AR		= $(CROSS_COMPILE)ar
export AS		= $(CROSS_COMPILE)as
export OC		= $(CROSS_COMPILE)objcopy
export OBJDUMP		= $(CROSS_COMPILE)objdump
export STRIP		= $(CROSS_COMPILE)strip
export RANLIB		= $(CROSS_COMPILE)ranlib

export GCCOPTS= 
# <<Warning Options>>
#GCCOPTS +=-Wall -Wshadow -Wstrict-prototypes -Wundef

# <<Debugging Options>>
GCCOPTS += -g
# <<Optimization Options>>
#GCCOPTS += -Os -fomit-frame-pointer
#	GCCOPTS += -Os -fomit-frame-pointer -fno-defer-pop
# <<C Language Options>>
#GCCOPTS += -ffreestanding
#	GCCOPTS += -ffreestanding -fno-builtin




#LIBOPTS +=  -lieihsrcaes -lc -lpthread
LIBOPTS +=  -lc -lpthread
# C compiler flags
CFLAGS =$(GCCOPTS) $(INCLUDES)


# linker flags
#LDFLAGS = -L/usr/local/lib  $(CFLAGS)
LDFLAGS =  $(CFLAGS)

ifndef V
# Make Macro
  MAKEFLAGS += --no-print-directory
endif




.PHONY: all clean

all: clean $(EXEC)

$(EXEC):  $(OBJS2)
	@echo ----- $(EXEC): $(OBJS2)
	$(CC) $(LDFLAGS) -o $@ $^  $(LIBOPTS)
	cp $(EXEC) /usr/local/bin


%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(EXEC) $(OBJS2) 
	rm -rf *.bmp *.raw *.bak
#	rm -f *.o *.gdb
