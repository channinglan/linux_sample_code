
exam1.o:     file format elf32-littlearm


Disassembly of section .text:

00000000 <foo>:
   0:	f240 0300 	movw	r3, #0
   4:	f2c0 0300 	movt	r3, #0
   8:	f240 0200 	movw	r2, #0
   c:	f2c0 0200 	movt	r2, #0
  10:	6819      	ldr	r1, [r3, #0]
  12:	2000      	movs	r0, #0
  14:	6018      	str	r0, [r3, #0]
  16:	1c4b      	adds	r3, r1, #1
  18:	6013      	str	r3, [r2, #0]
  1a:	4770      	bx	lr
