.globl _start
_start:
    /*
     * The uart tx register in the versatilepb qemu target is
     * at address 0x101f1000
     */
    ldr r0,=0x101f1000
    mov r1,#0
loop:
    add r1,r1,#1
    and r1,r1,#7
    add r1,r1,#0x30
    str r1,[r0]
    mov r2,#0x0D
    str r2,[r0]
    mov r2,#0x0A
    str r2,[r0]
    b loop
