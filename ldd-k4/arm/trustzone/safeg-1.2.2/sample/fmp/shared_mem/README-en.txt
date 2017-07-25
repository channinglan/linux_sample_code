
                SafeG User Samples: FMP sample program

              2013 (C) Shinya Honda(honda@ertl.jp) 

--------
Overview
--------

This sample application contains code for running a FMP sample program 
(sample1) in the T and NT OS.

It is added new command 'R' which reboot NT-OS.

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

