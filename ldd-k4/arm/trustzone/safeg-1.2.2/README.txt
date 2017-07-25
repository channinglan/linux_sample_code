
                           TOPPERS/SafeG: README
                           ---------------------
                         Release 1.2.1 2015/10/01

OVERVIEW
=======

This package contains the source code of TOPPERS/SafeG, a dual-OS
architecture designed to concurrently execute an RTOS (Real-Time Operating
System) and a GPOS (General-Purpose Operating System) on the same hardware
platform that uses ARM TrustZone security extensions.

Documentation can be found under the 'doc/' folder in several languages.

Please read the file 'COPYING.txt' to understand the license covering the
source code of TOPPERS/SafeG.


このパッケージにはTOPPERS/SafeGのソースコードが含まれています。SafeGは同一
ハードウェアプラットフォーム上で、汎用OSとRTOSを安全に同時実行できる高信頼
デュアルOSモニタです。SafeGでは汎用OSとRTOSの同時実行を実現するために、
ARMプロセッサのTrustZoneセキュリティ拡張機能を利用します。

各語のドキュメンテーションは'doc/'のディレクトーリの下に置いてあります。

TOPPERS/SafeGのライセンスは'COPYING.txt'の中で書いてあります。


SOURCE CODE OVERVIEW
====================

The current layout of SafeG's source code (might be outdated) has the
following layout:

    safeg/
    ├── com/                (COM communications library)
    │   ├── configurator/   (COM configurator tool)
    │   └── src/            (COM source code)
    ├── doc/                (Documentation)
    │   ├── en/             (English documentation)
    │   │   └ target/        (Target-dependent documentation)
    │   ├── jp/             (Japanese documentation)
    │   │   └ target/        (Target-dependent documentation)    
    │   └── logo/           (SafeG logo)
    ├── monitor/            (The SafeG TrustZone monitor)
    │   ├── common/         (Common definitions)
    │   ├── notifier/       (Notifiers module)
    │   ├── syscalls/       (System calls module)
    │   └── target/         (Monitor target-dependent code)
    │   └── libsafeg/       (SafeG monitor API library)     
    ├── patches/            (Patches and scripts)
    └── sample              (Example applications)
         └── fmp/            (TOPPERS/FMP applications)
    
Note that TOPPERS/SafeG does not include source code for the guest OSs 
(e.g., TOPPERS/FMP, U-Boot or Linux source code). However, 
instructions for downloading, patching and building them should be 
available in the documentation specific to your target board (check 
doc/jp/target/ or doc/en/target/).
