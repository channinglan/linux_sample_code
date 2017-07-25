 arm-none-eabi-as -mcpu=cortex-m3 -mthumb -gwarf2 -o add.o add.s
 arm-none-eabi-ld -Ttext=0x0 -Tdata=0x20000000 -o add.elf add.o
 arm-none-eabi-nm add.elf

Except for _start, stop, and pfnVectors are linker generated symbols. The address assignment for the labels _start is 0x00000048 and _estack (Top_Of_Stack) is 0x20004000.
