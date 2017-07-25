@ This is Linux/arm EABI system call sample program.
@
@ Build with following command:
@ as -o arm-mmap.o arm-mmap.s
@ ld -o arm-mmap arm-mmap.o
@ 
@ You can see the program issues system calls by following command:
@ strace ./arm-mmap
@

	.section	.rodata
	.align	2
.LC0:
	.ascii	"/dev/mem\000"
	.text
	.align	2
	.global _start	
	.type	_start, %function
_start:
	ldr	r0, .L3
	mov	r1, #2
	mov	r7, #5
	swi	#0 @ open("/dev/mem", O_RDWR)                = 3
	mov	r4, r0
	mov	r0, #0
	mov	r1, #256
	mov	r2, #1
	mov	r3, #1
	mov	r5, #1280
	mov	r7, #192
	swi	#0 @ mmap2(NULL, 256, PROT_READ, MAP_SHARED, 3, 0x500) = 0x40000000
	mov	r0, #0
	mov	r7, #1
	swi	#0 @ exit(0)                                 = ?
.L4:
	.align	2
.L3:
	.word	.LC0
	.size	_start, .-_start

