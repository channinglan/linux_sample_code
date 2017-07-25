.PHONY: clean posix all

#CC = arm-linux-gcc
#CC = arm-linux-gnueabi-gcc
CC = arm-linux-gnueabihf-gcc

# Top Dir 
SAFEGDIR = ../../../../

# Include global setting
include $(SAFEGDIR)/Makefile.config

LIBDIR = ../../../../com/src
INCLUDES = -I. -I$(LIBDIR) -I$(LIBDIR)/nt_linux -I$(LIBDIR)/target/$(MONITOR_TARGET)_nt_linux -I../
all:  gpos_app.c gpos_app.exe
	cp gpos_app.exe /home/honda/altera/nfs

gpos_app.exe: doscom_user.o doscom_kernel.o
	$(CC) -Wall -Wno-nonnull -mcpu=cortex-a9 -O2 -lrt -lpthread -lgcc -Wall  $(INCLUDES) -o gpos_app.exe gpos_app.c doscom_user.o doscom_kernel.o

doscom_user.o: 
	$(CC) -c -Wall -Wno-nonnull -mcpu=cortex-a9 -O2 -lrt -lpthread  -Wall $(INCLUDES)  $(LIBDIR)/doscom_user.c

doscom_kernel.o: 
	$(CC) -c -Wall -Wno-nonnull -mcpu=cortex-a9 -O2 -lrt -lpthread  -Wall $(INCLUDES)  $(LIBDIR)/nt_linux/doscom_kernel.c

clean:
	@exec echo -e "\n>> Cleaning... ";
	@find \( -name '*.[oa]' -or -name '*~*' -or -name '*.exe' -or -name 'a.out' \) -print -delete
	@rm -f $(LIBDIR)/non-trust/doscom_user_gpos.o
	@exec echo ">> [OK]"


