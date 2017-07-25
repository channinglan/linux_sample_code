
                SafeG User Samples: notifier sample program

              2013 (C) Shinya Honda(honda@ertl.jp) 

--------
Overview
--------

This sample application contains code for running notifier sample program .

=================
[Altera SoC]
=================
------------------
Monitor configuration
------------------
Set enable notifiler in Makefile and rebuild monitor.

MONITOR_ENABLE_NOTIFIERS  ?= 1


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

