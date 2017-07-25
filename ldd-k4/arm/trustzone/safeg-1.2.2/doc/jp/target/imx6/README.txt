
		TOPPERS/SafeG
		FreeScale iMX6 ユーザーズマニュアル

		対応バージョン: Release 1.2.0
		最終更新: 2015年09月17日

このドキュメントは，TOPPERS/SafeGをFreeScale i.MX6で使用するために必要
な事項を説明するものである．

=====================================================================
注意事項
=====================================================================
iMX6でSafeGを動作させるためには，以下のセキュリティマニュアルに記載さ
れている情報が必要である．

Security Reference Manual for i.MX 6

このマニュアルの記載内容は取り扱いが不明であるため，本リリースには対象
部分のソースコードを除いている．具体的には，./monitor/imx6/target.c か
ら呼び出す，csu_init()の実装がリリースに含まれていない．

csu_init()は，NT-Linuxで使用するデバイスをNon-Secureに設定する処理を行
う必要があるため，上記のマニュアルを入手して設定を，security.hというフ
ァイルに記載すればよい．

=====================================================================
概要
=====================================================================
本ドキュメントは，以下のボードを対象とする

   販売元        ボード名    本ドキュメントでの略称
 FreeScale社  : Sabre SD   : Sabre

SafeGによりTrust側では，FMPカーネルを動作させる(T-FMP)．

一方，Non-Trust側では，次のOSを動作させることが可能である．

・FMPカーネル
  以下，NT-FMP実行 と呼ぶ
  
・Linux
  以下，NT-Linux実行 と呼ぶ
  
また，SafeGの実行方法として，次をサポートしている．

・デバッガ(ICE)を用いた実行
  (必要な機材：ホスト，ターゲットボード，デバッガ(ICE))
  
・SDカードからのブート
  (必要な機材：ホスト，ターゲットボード, SDカード)


現状サポートしている組み合わせは次の通りである．

 Non-Trust    ICE        SD
NT-FMP実行     ○        ○
NT-Linux実行   ○        ○

=====================================================================
目次
=====================================================================
以下の順に解説を行う．プリビルドバイナリを使用する場合はビルドをスキップ
できる．

(1) 関連ファイル一覧
(2) 動作確認バージョン
(3) 開発環境の構築
(4) ビルド
(5) NT-FMP実行
(6) NT-Linux実行
(7) COMの使用方法
(8) その他
(9) 制限事項
(10)変更履歴

ドキュメント中のパスの記述について：
 <SAFEG_PACKAGE> はSafeGのパッケージを展開したフォルダに読み替える．
 <FMP_PACKAGE>   はFMPカーネルのパッケージを展開したフォルダに読み替える．
 <FSL_HOME>      yoctoを実行するルートフォルダ 
 <NT-LINUX>      はNon-Trust用のLinuxのトップフォルダに読み替える．
                 <FSL_HOME>/build-x11/tmp/work/imx6qsabresd-poky-linux-gnueabi/linux-imx/3.10.53-r0/git
 <NT-UBOOT>      はNon-Trust用のu-bootのトップフォルダに読み替える．
                 <FSL_HOME>/build-x11/tmp/work/imx6qsabresd-poky-linux-gnueabi/u-boot-imx/2014.04-r0/git  
 <TOOLCHAIN>     はビルドツールチェーンをインストールした
                 ディレクトリに読み替える．
 <LINALO_GCC>    はLinalo GCCのバイナリがあるディレクトリに読み替える．

 
=====================================================================
(1)関連ファイル一覧
=====================================================================
./doc/jp/target/socfpga/KMC : Partner-Jet用のファイル．
./monitor/target/socfpga : SafeG 依存部
./patches/target/socfpga : Linux 向けパッチ
./com/src/target/socfpga_nt_linux : COMのLinux用ファイル
./com/src/target/socfpga_t_fmp    : COMのFMP用ファイル

=====================================================================
(2)動作確認バージョン
=====================================================================
・FMPカーネル i.MX6 簡易パッケージ 20150917以降
   fmp_socfpga_gcc-20150917.tar.gz

・Linux
  kernel version 3.10.53

・JTAG-ICE
  京都マイクロコンピュータ Partner-Jet2 6.00i  
    
・ホスト PC
  Xubuntu 14.04.01 64bit
  
・Yocto環境  
  下記ドキュメントに従って環境を構築
  
  Freescale_Yocto_Project_User's_Guide.pdf
  
  ダウンロード先とバージョン  
  http://www.freescale.com/ja/products/arm-processors/i.mx-applications-processors-based-on-arm-cores/i.mx-6-processors/i.mx6qp/i.mx-6quad-processors-high-performance-3d-graphics-hd-video-arm-cortex-a9-core:i.MX6Q?fpsp=1&tab=Documentation_Tab#nogo
  
  i.MX 6Quad, i.MX 6Dual, i.MX 6DualLite, i.MX 6Solo and i.MX 
  6Sololite Linux BSP Documentation. Includes Release Notes, (REV 
  L3.10.53_1.1.0) 

    
=====================================================================
(1) 開発環境の構築
=====================================================================
○ビルド環境

●FMPカーネルビルド環境

FMPカーネルのi.MX6ターゲット依存部のマニュアル
(./target/imx6_gcc/target_user.txt)を参照のこと．
    
●SafeGのビルド環境 

以下のバージョンのGCCで動作確認は行った．

  gcc-arm-none-eabi-4_9-2015q2
  
  入手先
  launchpad (https://launchpad.net/gcc-arm-embedded) 
   
●u-boot/Linuxのビルド環境

  Yocto付属の
      
○ターゲットボード

●UART用のケーブルの増設

T-FMPのコンソール出力用にUART3を使用する．標準ではUART3は，GPS用に使わ
れている．GPSは実装されていないためこのパターンにUSB-Serialを接続する．

確認したUSB-Serialは次のもの
  https://strawberry-linux.com/catalog/items?code=15076
極性は
  GND:青 
  RX :グリーン
  TX :赤
     
      CPU           : GPSパターン : UART
・EIM_D25 UART3_RXD : GPS30       : TX :赤        
・EIM_D24 UART3_TXD : GPS31       : RX :グリーン
                      GPS33       : GND:青
    
また，GPSパターンの真横の R192，R194は実装されていないため半田でショー
トさせる
    
●SDの作成と挿入

"(2)ビルド"を参照してSDを作成して，SD3のスロットに挿入する．
      
○開発環境のインストール

●FMPカーネルの取得
  本パッケージにはFMPカーネル本体のソースコードを含まないため，
  TOPPERSプロジェクトのwebサイトから
  「FMP socfpga(GCC)簡易パッケージ」を入手して，<SAFEG_PACKAGE>と同じ階層
  のディレクトリに展開する．

 $ wget https://www.toppers.jp/download.cgi/fmp_imx6_gcc-20150917.tar.gz
 $ tar zxvf fmp_imx6_gcc-(ToDo).tar.gz -C <FMP_PACKAGE>

●コンフィギュレータの取得
  FMPカーネルのビルドにはTOPPERS新世代カーネル用コンフィギュレータが
  必要である．
  コンフィギュレータの実行ファイルを"<FMP_PACKAGE>/cfg/cfg/cfg"にコピーする． 

 $ wget https://www.toppers.jp/download.cgi/cfg-linux-static-1_9_4.gz
 $ gunzip cfg-linux-static-1_9_4.gz
 $ chmod +x cfg-linux-static-1_9_4
 $ mkdir <FMP_PACKAGE>/cfg && mkdir <FMP_PACKAGE>/cfg/cfg
 $ mv cfg-linux-static-1_9_4 <FMP_PACKAGE>/cfg/cfg/cfg


=====================================================================
(2) ビルド
=====================================================================
○SafeG

<SAFEG_PACKAGE>/Makefile.config の下記の行を有効にする．

 MONITOR_TARGET=imx6
 
また，以下の行を展開したFMPのフォルダ名に合わせる．
 
 FMPDIR=../fmp_1.3.0

<SAFEG_PACKAGE>/monitor に移動してビルドする．

 $cd <SAFEG_PACKAGE>/monitor
 $make

ビルドに成功すると，以下のファイルが生成される．

 monitor.elf
 monitor.bin

○Trust FMPカーネル(T-FMP)

Trust側でFMPカーネルとFMPカーネル付属のサンプルプログラムを動作させる
ためにビルドについて説明する．

 <SAFEG_PACKAGE>/sample/fmp/sample1/trust/ に移動してビルドする．
 
 $cd <SAFEG_PACKAGE>/sample/fmp/sample1/trust/
 $make depend
 $make
 
ビルドに成功すると，以下のファイルが生成される．

 fmp_t.elf
 fmp_t.bin
 
○Non-Trust FMPカーネル

Non-Trust側でFMPカーネルとFMPカーネル付属のサンプルプログラムを動作さ
せるためにビルドについて説明する．

 <SAFEG_PACKAGE>/sample/fmp/sample1/non-trust/ に移動してビルドする．
 
 $cd <SAFEG_PACKAGE>/sample/fmp/sample1/non-trust/
 $make depend
 $make
 
ビルドに成功すると，以下のファイルが生成される．

 fmp_nt.elf
 fmp_nt.bin
 

○YoctoによるLinux/u-bootのビルド

NonTrust用のu-bootやLinuxをビルドする前準備として以下の手順を実施する．
下記にもあるように，全体のビルドはリソースを要するので既存のユーザーラ
ンドを使用する場合は，u-bootのみ，Linuxのみを実施すればよい．

●環境構築

Freescale_Yocto_Project_User's_Guide.pdf の以下のセクションを参照して
環境を構築する．

  3 Host Setup
  4 Yocto Project Setup
  5.6 Build Scenarios
  ・5.6直下のrepo sync までを実施する．
  
●全体のビルド

おおよそ60GB程度のディスク容量と一晩程度の時間が必要．
既存のユーザーランドを使用する場合はスキップ可能．
xxxは使用するユーザーランドのタイプに応じて分ける．qt5で動作を確認した．

  $ MACHINE=imx6qsabresd source fsl-setup-release.sh -b build-x11 -e x11
  $ bitbake fsl-image-xxx
  
SDへの書き込み

SDイメージをそのまま書き込むと，FATパーティションが8Mと小さい．ある程
度大きい方が都合が良いため，FATパーティションを大きめにしておいた方が
よい．

4G以上のSDに対して，fdisk等で第1パーティションをFATとして128M等で作成．
第2パーティションをext3を2G以上のサイズで作成．

各パーティションをフォーマット．（以下それぞれのパーティションを
sdb1/sdb2とする）

 $ mkfs.fat  /dev/sdb1
 $ mkfs.ext3 /dev/sdb2

第2パーティションをマウントする．

$ mount /dev/sdb2 /mnt/imx6_rootfs

rootfsを展開する．
 $ cd /mnt/imx6_rootfs
 $ tar xvfj <FSL_HOME>/build-x11/tmp/deploy/images/imx6qsabresd/fsl-image-qt5-imx6qsabresd.tar.bz2

第1パーティションをマウントする．
 $ mount /dev/sdb1 /mnt/imx6_first

u-bootを書き込む  
 $ dd if=:<FSL_HOME>/build-x11/tmp/deploy/images/imx6qsabresd/u-boot-imx6qsabresd.imx of=/dev/sdb bs=1k seek=1
 $ sync
              
Linuxを書き込む
  $ cp <FSL_HOME>/build-x11/tmp/deploy/images/imx6qsabresd/zImage /mnt/imx6_first/
  $ cp <FSL_HOME>/build-x11/tmp/deploy/images/imx6qsabresd/imx6q-sabresd.dtb /mnt/imx6_first/
                
●u-bootのみビルド

U-Bootのみをビルドする場合は以下のコマンドを実行する．
  $ cd <FSL_HOME>
  $ MACHINE=imx6qsabresd source fsl-setup-release.sh -b build-x11 -e x11
  $ bitbake u-boot-imx
  
SDへの書き込み   
  $ dd if=<FSL_HOME>/build-x11/tmp/work/imx6qsabresd-poky-linux-gnueabi/u-boot-imx/2014.04-r0/git/u-boot.imx of=/dev/sdb1 bs=1k seek=1
  $ sync
  
●Linuxのみ

Linuxのみをビルドする場合は以下のコマンドを実行する．
  $ cd <FSL_HOME>
  $ MACHINE=imx6qsabresd source fsl-setup-release.sh -b build-x11 -e x11
  $ bitbake linux-imx
  
LinuxのイメージとデバイスツリーファイルをSDのFATパーティションへの書き込み．
  $ cp <FSL_HOME>/build-x11/tmp/work/imx6qsabresd-poky-linux-gnueabi/linux-imx/3.10.53-r0/git/arch/arm/boot/zImage /mnt/imx6_first/
  $ cp <FSL_HOME>/build-x11/tmp/work/imx6qsabresd-poky-linux-gnueabi/linux-imx/3.10.53-r0/git/arch/arm/boot/dts/imx6q-sabresd.dtb /mnt/imx6_first/
  $ sync  

○Non-Trust uboot(NT-uboot)

上記の"u-bootのみビルド"を実施する．

<NT-UBOOT>に移動してパッチをあてる．
 $ cd <NT-UBOOT>
 $ patch -p1 < <SAFEG_PACKAGE>/patches/target/imx6/1_NT_uboot.patch

ビルドする．
 $ cd <FSL_HOME>
 $ MACHINE=imx6qsabresd source fsl-setup-release.sh -b build-x11 -e x11
 $ bitbake u-boot-imx -c compile -f

ビルドに成功すると，以下のファイルが更新される．
 <NT-UBOOT>u-boot
 <NT-UBOOT>u-boot.imx 

○Non-Trust Linux(NT-Linux)

<NT-LINUX>に移動してパッチをあてる．
 $ cd <NT-LINUX>
 $ patch -p1 < <SAFEG_PACKAGE>/patches/target/imx6/1_NT_linux.patch
 $ patch -p1 < <SAFEG_PACKAGE>/patches/target/imx6/2_NT_linux_com.patch
 
COM用のファイルをコピーする． 
 $ mkdir <NT-LINUX>/safeg/
 $ cp <SAFEG_PACKAGE>/com/src/target/imx6_nt_linux/doscom_driver_hw.h <NT-LINUX>/safeg/
 $ cp <SAFEG_PACKAGE>/com/src/target/imx6_nt_linux/Kconfig            <NT-LINUX>/safeg/
 $ cp <SAFEG_PACKAGE>/com/src/target/imx6_nt_linux/Makefile           <NT-LINUX>/safeg/
 $ cp <SAFEG_PACKAGE>/com/src/linux-driver/doscom.c <NT-LINUX>/safeg/
 
コンフィグを呼び出す．
 $ cd <NT-LINUX>
 $ make ARCH=arm meuconfig 
 
Cryptographic API の Hardware crypto devices を無効にする．
  
ビルドする．
 $ cd <FSL_HOME>
 $ MACHINE=imx6qsabresd source fsl-setup-release.sh -b build-x11 -e x11
 $ bitbake linux-imx -c compile -f
 
ビルドに成功すると，以下のファイルが更新される．
 <NT-LINUX>/arch/arm/boot/zImage 

  
=====================================================================
(5) NT-FMP実行
=====================================================================
○デバッガ(ICE)を用いた実行

1.ボードの設定
u-bootを書き込んだSDメモリをボードに接続する．

2.ターミナルの接続
UART1とUART3にターミナルを115200bpsで開く．

3.Partner-Jetによるロード
 1.Partner-Jet2付属の i.MX6Q用のプロジェクト(imx61_sabre)を基に新
   規プロジェクトをAMPで作成する．
 
 2.コア0のデバッガを起動してターゲットをリセットする．
   u-bootが実行されて停止する．
   
 3.以下のコマンドにより各バイナリをダウンロード． 
 L   "<SAFEG_PACKAGE>/sample/fmp/sample1/trust/fmp_t.elf"
 LA  "<SAFEG_PACKAGE>/sample/fmp/sample1/non-trust/fmp_nt.elf"
 LA  "<SAFEG_PACKAGE>/monitor/monitor.elf"
 
 5.go により実行する．
  
○SDカードからのブート

1.SDカードへコピー
SDカードのファットパーティションに，以下のファイルを書き込む．

 "<SAFEG_PACKAGE>/sample/fmp/sample1/trust/fmp_t.bin"
 "<SAFEG_PACKAGE>/sample/fmp/sample1/non-trust/fmp_nt.bin"
 "<SAFEG_PACKAGE>/monitor/monitor.bin"

2.ターミナルの接続 
デバッガ(ICE)を用いた実行と同様．

3.SDカードからのブート
上記のSDを評価ボードに差してSDカードからブートする．

4.u-bootによるロード
u-bootが起動したら，Linuxのブートを止めてコマンドプロンプトにする．
以下のコマンドを実行してSDカードからロードして実行する．

 mmc rescan
 fatload mmc 1:1 0x4f000000 monitor.bin
 fatload mmc 1:1 0x4f100000 fmp_t.bin
 fatload mmc 1:1 0x17800000 fmp_nt.bin
 go 0x4f000000

上記内容をファイル(boot.txt)にして，u-boot用のブートファイル(boot.scr)に変換．

mkimage -A arm -T script -d boot.txt boot.scr

boot.scrをSDのFATパーティションに書き込むと，起動時にu-bootにより実行
される．
 
=====================================================================
(6) NT-Linux実行
=====================================================================
○デバッガ(ICE)を用いた実行

1.ボードの設定
・ボード出荷時のSDメモリ(u-bootが起動するもの)にNT-LinuxのzImage
  (<NT-LINUX>\arch\arm\boot\zImage)を
  zImage_nt としてコピーする

・ボード出荷時のSDメモリ(u-bootが起動するもの)にNT-Linuxの
  デバイスファイル(<NT-LINUX>/arch/arm/boot/dts/imx6q-sabresd.dtb)を
  imx6q-sabresd_nt.dtbとしてコピーする

・SDカードをボードのSD3に接続する

2.ターミナルの接続
 UART1とUART3にターミナルを115200bpsで開く．

3.Partner-Jetによるロード
 1.Partner-Jet2付属の i.MX6Q用のプロジェクト(imx61_sabre)を基に新
   規プロジェクトをAMPで作成する．
 
 2.コア0のデバッガを起動してターゲットをリセットする．
   u-bootが実行されて停止する．
 
 3.u-bootが起動して，Linuxの起動のカウントダウンになったら，デバッガに
   よりターゲットを停止する．
   
   L   "<NT-UBOOT>\u-boot"
   LA  "<SAFEG_PACKAGE>/sample/fmp/sample1/trust/fmp_t.elf"
   LA  "<SAFEG_PACKAGE>/monitor/monitor.elf"

 4.go により実行する．
 
 5.FMPカーネルのコア0実行するべきタスクが存在しなくなるとu-bootが実行
    される．
    
 6.u-bootからLinuxがブートされる．

○SDカードからのブート

次の流れでブートを行う．

1.u-bootの実行．
2.u-bootによる以下のファイルのロードとSafeGの実行．
  ・monitor.bin(SafeG)
  ・fmp_t.bin(T-FMPカーネル)
  ・u-boot_nt.bin(NT-u-boot)
3.SafeGの実行．
4.T-FMPカーネルの実行．
5.NT-u-bootの実行
6.NT-Linuxの実行

1.ボードの設定
ユーザーランドとu-bootを書き込んだSDメモリのFAT領域に以下をコピーする．

・NT-Linuxの<NT-LINUX>/arch/arm/boot/zImage を 
  zImage_nt としてコピー．
・NT-Linuxの<NT-LINUX>/arch/arm/boot/dts/imx6q-sabresd.dtb を 
  imx6q-sabresd_nt.dtb としてコピー．
・NT-ubootの"<NT-UBOOT>\u-boot.bin"をu-boot_nt.binとしてコピー．
・ビルドしたセキュア用のfmpのバイナリをfmp_t.binとしてコピー．
・"<SAFEG_PACKAGE>/monitor/monitor.bin"をコピー．
・上記のboot.scrをコピー

ブートスクリプトを用意する．

 mmc rescan
 fatload mmc 1:1 0x4f000000 monitor.bin
 fatload mmc 1:1 0x4f100000 fmp_t.bin
 fatload mmc 1:1 0x17800000 u-boot_nt.bin
 go 0x4f000000

上記内容をファイル(boot.txt)にして，u-boot用のブートファイル(boot.scr)に変換．

mkimage -A arm -T script -d boot.txt boot.scr

boot.scrをSDのFATパーティションに書き込むと，起動時にu-bootにより実行
される．

書き込んだSDカードをSD3に接続する．

2.ターミナルの接続
UART1とUART3にターミナルを115200bpsで開く．

3.SDカードからのブート
上記のSDをボードに差してSDカードからブートする．
  
=====================================================================
(7)COMの使用方法
=====================================================================
○基本サンプルプログラム

●COMの基本サンプルが以下のフォルダにある．

 <SAFEG_PACKAGE>/sample/fmp/com
 
●ファイルの編集
 以下のファイルの 
 
   <SAFEG_PACKAGE>/sample/fmp/com/gpos/Makefile
 
 CC を環境に合わせて設定する．
 
   CC = arm-linux-gnueabihf-gcc
 
●コンフィギュレーション
 COMのコンフィギュレーションを行う．
 
 $<SAFEG_PACKAGE>/sample/fmp/com/com_config.sh
 
●T-FMPのビルド 

 $cd <SAFEG_PACKAGE>/sample/fmp/com/rtos
 $make depend
 $make
  
fmp_t.bin が出来ていれば成功．
            
●Linuxアプリのビルド

 $ cd <SAFEG_PACKAGE>/sample/fmp/com/gpos
 $ make


ビルドに成功すると，以下のファイルが生成される．

gpos_app.exe

●COMのサンプルプログラムの実行

上記のfmp_t.binをSafeGにより実行し，Linuxが起動した後，Linuxアプリをビ
ルドして生成された実行ファイルを実行すればよい．


○テストプログラム

●COMのテストプログラムが以下のフォルダにある．

 <SAFEG_PACKAGE>/sample/fmp/com_test
 
●ファイルの編集
 以下のファイルの 
 
   <SAFEG_PACKAGE>/sample/fmp/com_test/gpos/Makefile
 
 CC を環境に合わせて設定する．
 
   CC = arm-linux-gnueabihf-gcc
 
●コンフィギュレーション
 COMのコンフィギュレーションを行う．
 
 $<SAFEG_PACKAGE>/sample/fmp/com_test/com_config.sh
 
●T-FMPのビルド 

 $cd <SAFEG_PACKAGE>/sample/fmp/com_test/rtos
 $make depend
 $make
  
fmp_t.bin が出来ていれば成功．
            
●Linuxアプリのビルド

 $ cd <SAFEG_PACKAGE>/sample/fmp/com_test/gpos
 $ make

ビルドに成功すると，以下のファイルが生成される．
 com_fifo_gtr.exe
 com_fifo_rtg.exe
 com_shmem_rtg.exe
 com_shmem_gtr.exe
 com_fifo_reconnect.exe
 com_fifo_init.exe


●実行

Linuxアプリをビルドして生成された6つの実行ファイルと，test.shを同じ
ディレクトリに置き，Linux起動後に以下のコマンドを実行する．

 $ cd <test.shのあるディレクトリ>
 $ taskset 0x1 ./test.sh
 
Test ENDの文字がFMPカーネルのログに出力されれば成功．

=====================================================================
(8) その他
=====================================================================
○メモリ構成

●基本構成
  メモリサイズ : 1Gbyte 
  メモリマップ 
    0x10000000 ～ 0x4fffffff
     
●SafeG環境での割り当て
  0x00000000 ～ 0x4effffff  : Linux       : 1056M
  0x4f000000 ～ 0x4f0fffff  : SafeG       : 1M* 
  0x4f100000 ～ 0x4f7fffff  : FMPカーネル : 7M*
  0x4f800000 ～ 0x4fffffff  : COM領域     : 8M
        
*Trustに設定．
        
○割込み

●SafeG環境での割り当て

以下の番号の割込みをCOMで使用する．

・129 : Non-Trust -> Trust方向の割込み
・130 : Trust -> Non-Trust方向の割込み

割込み番号はFMPとLinuxで共通

○メモリ・割込み番号の設定変更

COMで使用するメモリや割込み番号を変更するには，以下のファイルを編集す
ること．

./monitor/target/imx6/target_common.h
./com/src/target/imx6_nt_linux/doscom_driver_hw.h
<FMP_PACKAGE>/target/imx6_gcc/target_kernel.h
  
○HDMIの有効

HDMI接続を有効にするには，NT-uboot起動後にブートパラメータを次のように
設定すれば良い．

setenv smp maxcpus=4 video=mxcfb0:dev=hdmi,1280x720M@60,if=RGB24

=====================================================================
(9)制限事項
=====================================================================
●LinuxによるPLLの初期化
NT-Linuxは起動時にPLLを初期化する．現状初期化コードはそのままであるた
め，T-FMP側で使用しているペリフェラルが一旦止まる．標準で使用している
UART3は割込みが入り続けるため，空のデータが受信してこの問題を回避して
いる．

●LinuxによるDVFSの抑制
LinuxはDVFSを用いてコアのクロックを動的に変更する．そのため，FMP側の性
能も変化するため注意が必要である．

●LinuxからのHardware crypto devicesの使用
Hardware crypto devices（CAAM）はSecureからのみアクセス可能である．そ
のため，NT-Linuxからはアクセスができない．現状はコンフィギュレーション
で無効としている．

●LinuxによるClock Gating
Linuxは起動時に使用しないペリフェラルのクロック供給を止める．現状の
NT-Linuxではこの処理自体を無効としている．そのため，使用していないペリ
フェラルに対してもクロックが供給されているため注意が必要である．
   
●NT-ubootの環境変数の制限
通常のu-bootと環境変数を共有しないように，NT-ubootは常にディフォルトの
環境変数を使用する設定となっている．


=====================================================================
(10) 変更履歴
2015/10/01
・(7)COMの使用方法 に問題があったため修正．

2015/09/30
・手順に抜けがあったため追加．
・NT-ubootの環境変数の制限の追加．

2015/09/17
・最初のリリース

以上
