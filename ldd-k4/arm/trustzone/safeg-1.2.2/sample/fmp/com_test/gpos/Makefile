.PHONY: clean posix all

CC = arm-linux-gnueabihf-gcc

LIBDIR = ../../../../com/src
INCLUDES = -I. -I$(LIBDIR) -I$(LIBDIR)/nt_linux -I$(LIBDIR)/target/socfpga_nt_linux -I../ -I../../../../monitor/libsafeg
all:  com_fifo_gtr.c com_fifo_reconnect.c com_fifo_rtg.c com_shmem_gtr.c com_shmem_rtg.c com_fifo_gtr.exe com_fifo_reconnect.exe com_fifo_rtg.exe com_shmem_gtr.exe com_shmem_rtg.exe com_fifo_init.c com_fifo_init.exe
	cp *.exe /home/honda/altera/nfs
	cp test.sh /home/honda/altera/nfs

com_fifo_gtr.exe: doscom_user.o doscom_kernel.o syscalls_api.o
	$(CC) -Wall -Wno-nonnull -mcpu=cortex-a9 -O2 -lrt -lpthread -lgcc -Wall  $(INCLUDES) -o com_fifo_gtr.exe com_fifo_gtr.c doscom_user.o doscom_kernel.o syscalls_api.o

com_fifo_rtg.exe: doscom_user.o doscom_kernel.o syscalls_api.o
	$(CC) -Wall -Wno-nonnull -mcpu=cortex-a9 -O2 -lrt -lpthread -lgcc -Wall  $(INCLUDES) -o com_fifo_rtg.exe com_fifo_rtg.c doscom_user.o doscom_kernel.o syscalls_api.o

com_shmem_rtg.exe: doscom_user.o doscom_kernel.o syscalls_api.o
	$(CC) -Wall -Wno-nonnull -mcpu=cortex-a9 -O2 -lrt -lpthread -lgcc -Wall  $(INCLUDES) -o com_shmem_rtg.exe com_shmem_rtg.c doscom_user.o doscom_kernel.o syscalls_api.o

com_shmem_gtr.exe: doscom_user.o doscom_kernel.o syscalls_api.o
	$(CC) -Wall -Wno-nonnull -mcpu=cortex-a9 -O2 -lrt -lpthread -lgcc -Wall  $(INCLUDES) -o com_shmem_gtr.exe com_shmem_gtr.c doscom_user.o doscom_kernel.o syscalls_api.o

com_fifo_reconnect.exe: doscom_user.o doscom_kernel.o syscalls_api.o
	$(CC) -Wall -Wno-nonnull -mcpu=cortex-a9 -O2 -lrt -lpthread -lgcc -Wall  $(INCLUDES) -o com_fifo_reconnect.exe com_fifo_reconnect.c doscom_user.o doscom_kernel.o syscalls_api.o

com_fifo_init.exe: doscom_user.o doscom_kernel.o syscalls_api.o
	$(CC) -Wall -Wno-nonnull -mcpu=cortex-a9 -O2 -lrt -lpthread -lgcc -Wall  $(INCLUDES) -o com_fifo_init.exe com_fifo_init.c doscom_user.o doscom_kernel.o syscalls_api.o

doscom_user.o: 
	$(CC) -c -Wall -Wno-nonnull -mcpu=cortex-a9 -O2 -lrt -lpthread  -Wall $(INCLUDES)  $(LIBDIR)/doscom_user.c

doscom_kernel.o: 
	$(CC) -c -Wall -Wno-nonnull -mcpu=cortex-a9 -O2 -lrt -lpthread  -Wall $(INCLUDES)  $(LIBDIR)/nt_linux/doscom_kernel.c

syscalls_api.o:
	$(CC) -c -Wall -Wno-nonnull -mcpu=cortex-a9 -O2 -lrt -lpthread  -Wall $(INCLUDES) ../../../../monitor/libsafeg/syscalls_api.c
	
clean:
	@exec echo -e "\n>> Cleaning... ";
	@find \( -name '*.[oa]' -or -name '*~*' -or -name '*.exe' -or -name 'a.out' \) -print -delete
	@rm -f $(LIBDIR)/non-trust/doscom_user_gpos.o
	@exec echo ">> [OK]"


