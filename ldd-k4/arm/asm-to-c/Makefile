CROSS_COMPILE ?= arm-linux-gnueabi

AOPS = --warn --fatal-warnings -mcpu=arm926ej-s 
COPS = -g -Wall -Werror -O0 -nostdlib -nostartfiles -mcpu=arm926ej-s

hello_world.bin : startup.s main.c linker.ld
	$(CROSS_COMPILE)-as $(AOPS) startup.s -o startup.o
	$(CROSS_COMPILE)-gcc $(COPS) main.c -c -o main.o
	$(CROSS_COMPILE)-ld main.o startup.o -T linker.ld -o hello_world.elf
	$(CROSS_COMPILE)-objdump -D hello_world.elf > hello_world.list
	$(CROSS_COMPILE)-objcopy hello_world.elf -O binary hello_world.bin
clean:
	rm *.bin *.o *.elf *.list
