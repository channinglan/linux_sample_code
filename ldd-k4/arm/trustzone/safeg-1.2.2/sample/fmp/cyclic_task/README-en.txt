
                SafeG User Samples: FMP Cyclic Task

              2013 (C) Daniel Sangorrin(daniel.sangorrin@gmail.com)
              2013 (C) Shinya Honda(honda@ertl.jp) 

--------
Overview
--------

This sample application contains code for running a simple periodic task
in the T and NT OS.

    + T-OS: FMP
        - main task: put initial message and start cyclic handler.
        - task: periodic task displaying an incrementing counter.
        - btask: low priority task executing the SWITCH system call.
    + NT-OS: FMP
        - main task: put initial message and start cyclic handler.
        - task: periodic task displaying an incrementing counter.

=================
[Altera SoC]
=================
------------------
Build instructions
------------------
Set Makefile.config at top folder.

T-OS
$ cd trust
$ make realclean &&
$ make fmp_s.bin

NT-OS
$ cd non-trust
$ make realclean &&
$ make fmp_ns.bin

=================
[Vexpress9 - QEMU target]
=================
------------------
Build instructions
------------------

See safeg/doc/en/target/vexpress/README.txt and
fmp/target/vexpress_gcc/README.txt for details.

$ cd trust
$ make realclean &&
$ make TARGET=vexpressa9_gcc SAFEG=SECURE ENABLE_QEMU=true PRC_NUM=1 fmp_t.bin &&
$ sudo cp fmp_t.bin /var/lib/tftpboot/t.bin &&

$ cd non-trust
$ make realclean &&
$ make TARGET=vexpressa9_gcc SAFEG=NONSECURE ENABLE_QEMU=true PRC_NUM=1 fmp_nt.bin &&
$ sudo cp fmp_nt.bin /var/lib/tftpboot/nt.bin

-----------------
Load instructions
-----------------

See safeg/doc/en/target/vexpress/README.txt for details.

$ /home/dsl/qemu/qemu-tz/src/arm-softmmu/qemu-system-arm -cpu cortex-a9 \
-M vexpress-a9 -smp 1 -serial vc:80Cx40C -serial vc:80Cx40C -serial \
vc:80Cx40C -serial vc:80Cx40C -no-reboot -m 1024M -tftp /var/lib/tftpboot/ \
-kernel /home/dsl/uboot/u-boot-vexpressa9-patched/u-boot -icount auto

(QEMU) Ctrl+Alt+5
(QEMU) VExpress# run loadsafeg

------------------
Debug instructions
------------------

See safeg/doc/en/target/vexpress/README.txt for details.

$ /home/dsl/qemu/qemu-tz/src/arm-softmmu/qemu-system-arm -cpu cortex-a9 \
-M vexpress-a9 -smp 1 -serial vc:80Cx40C -serial vc:80Cx40C -serial \
vc:80Cx40C -serial vc:80Cx40C -no-reboot -m 1024M -tftp /var/lib/tftpboot/ \
-kernel /home/dsl/uboot/u-boot-vexpressa9-patched/u-boot -icount auto -s -S

$ ddd --debugger arm-none-eabi-gdb safeg/monitor/monitor.elf
(gdb) target remote localhost:1234
(gdb) break smc_vector
(gdb) break fiq_nt_vector
(gdb) c

(QEMU) Ctrl+Alt+5
(QEMU) VExpress# run loadsafeg