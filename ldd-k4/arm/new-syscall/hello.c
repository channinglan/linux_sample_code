#include <stdio.h>

#define	sys_oabi_hello() __asm__ __volatile__ ("swi 0x900000+388\n\t")
#define	sys_eabi_hello() __asm__ __volatile__ ("mov r7,#388\n\t" "swi 0\n\t" )

int main(void)
{
	printf("start hello\n");
	sys_oabi_hello();
	sys_eabi_hello();
	printf("end hello\n");
}

