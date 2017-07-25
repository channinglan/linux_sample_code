
		TOPPERS/SafeG
		Altera SoC(SoC FPGA) ユーザーズマニュアル

		対応バージョン: Release 1.2.0
		最終更新: 2015年09月17日

このドキュメントは，TOPPERS/SafeGをAltera SoC(SoC FPGA)で使用するため
に必要な事項を説明するものである．

=====================================================================
概要
=====================================================================
本ドキュメントは，以下のボードを対象とする

   販売元     ボード名                    本ドキュメントでの略称
 Altera社   : Cyclone V SoC 開発キット  : SoCDev
 Macnica社  : Helio                     : Helio

Helio は Rev.1.3及び1.4で動作確認を行った．

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
 <NT-LINUX>      はNon-Trust用のLinuxのトップフォルダに読み替える．
 <NT-UBOOT>      はNon-Trust用のu-bootのトップフォルダに読み替える．
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
・FMPカーネル SoC FPGA簡易パッケージ 20150917以降
   fmp_socfpga_gcc-20150917.tar.gz

・Linux
  kernel version 3.10.31

・JTAG-ICE
  京都マイクロコンピュータ Partner-Jet2 6.00i  
  
・Altera SoC エンベデッド・デザイン・スイート (EDS) 14.1
  
・ホスト PC
  Xubuntu 14.04.01 64bit
        
=====================================================================
(1) 開発環境の構築
=====================================================================
○ビルド環境

●FMPカーネルビルド環境

FMPカーネルのSOCFPGAターゲット依存部のマニュアル
(./target/socfpga/target_user.txt)を参照のこと．
    
●SafeGのビルド環境 

以下のバージョンのGCCで動作確認は行った．

  gcc version 4.9.3 20141119 (release) 
  [ARM/embedded-4_9-branch revision 218278] (GNU Tools for ARM Embedded Processors) 
 
  入手先
   https://launchpad.net/gcc-arm-embedded/4.9/4.9-2014-q4-major/+download/gcc-arm-none-eabi-4_9-2014q4-20141203-linux.tar.bz2
   
●u-boot/Linuxのビルド環境

  gcc バージョン 4.8.3 20131202 (prerelease) 
  (crosstool-NG linaro-1.13.1-4.8-2013.12 - Linaro GCC 2013.11) 

  入手先
  http://releases.linaro.org/13.12/components/toolchain/binaries/gcc-linaro-arm-linux-gnueabihf-4.8-2013.12_linux.tar.bz2
 
  展開したディレクトリを<TOOLCHAIN>とする
  また，<LINALO_GCC>は<TOOLCHAIN>/binである
      
○ターゲットボード

●ディップスイッチの設定

JTAG-ICE 使用時は，ボードは次の設定とする．
 
 SW4-1 ON
 J6    OPEN
 
 
●FPGAのデザイン

GSRD 14.1 をQuartusで作成ビルドして，FPGAに書き込む．

SoCDev
 http://releases.rocketboards.org/release/2014.12/gsrd/hw/cv_soc_devkit_ghrd.tar.gz

Helio(Rev.1.2/1.3用)
 http://www.rocketboards.org/pub/Documentation/MacnicaHelioSoCEvaluationKit/helio_ghrd_5csxc6es_v14.1.zip?t=1423029476

Helio(Rev.1.4)
 http://www.rocketboards.org/pub/Documentation/MacnicaHelioSoCEvaluationKit/helio_ghrd_5csxc5_v14.1.zip?t=1423029835

コンフィギュレーションROMへの書き込みは以下を参考にすること．

  http://rocketboards.org/foswiki/Documentation/ConfigureEpcq131
  
手順の概要は次の通り．
  
  1.SOFファイルより、JICファイルを生成.
    QuartusIIのFile -> Convert Programing Fileを選択
    Programming file type : JTAG Indirect Configuration File(.jic) を選択
    Configuration device : EPCQ 256 を選択
    Flash Loader にFPGAの型番を設定 : 5CSXFC6(Rev.1.2/1.3用) or 5CSXFC5C6(Rev.1.4)
    Add Sof Pageで output_files\helio_ghrd_top.sof を指定．

  2.Programmerを立ち上げて頂き、AutoDetectを実行．
    ⇒JTAG接続状態を確認できます。
    5CSXFC6C6デバイスの欄に，1.で作成したJICファイルを割り当てる．

  3.EPCQのProgram/Configureのチェックを入れ，プログラムボタンを押す．
  
  
●SDの作成

以下のイメージをDD等でSDメモリに書き込む．

SoCDev
 http://releases.rocketboards.org/release/2014.12/gsrd/bin/linux-socfpga-gsrd-14.1-cv-bin.tar.gz
 
Helio
 http://www.rocketboards.org/pub/Documentation/MacnicaHelioSoCEvaluationKit/helio_gsrd_sdimage_v3.10-ltsi.tar.gz

      
○開発環境のインストール

●FMPカーネルの取得
  本パッケージにはFMPカーネル本体のソースコードを含まないため，
  TOPPERSプロジェクトのwebサイトから
  「FMP socfpga(GCC)簡易パッケージ」を入手して，<SAFEG_PACKAGE>と同じ階層
  のディレクトリに展開する．

 $ wget https://www.toppers.jp/download.cgi/fmp_socfpga_gcc-20150727.tar.gz
 $ tar zxvf fmp_socfpga_gcc-20150727.tar.gz -C <FMP_PACKAGE>

●コンフィギュレータの取得
  FMPカーネルのビルドにはTOPPERS新世代カーネル用コンフィギュレータが
  必要である．
  コンフィギュレータの実行ファイルを"<FMP_PACKAGE>/cfg/cfg/cfg"にコピーする． 

 $ wget https://www.toppers.jp/download.cgi/cfg-linux-static-1_9_4.gz
 $ gunzip cfg-linux-static-1_9_4.gz
 $ chmod +x cfg-linux-static-1_9_4
 $ mkdir <FMP_PACKAGE>/cfg && mkdir <FMP_PACKAGE>/cfg/cfg
 $ mv cfg-linux-static-1_9_4 <FMP_PACKAGE>/cfg/cfg/cfg

●NT-Linuxの取得
  インストール方法は基本的に以下のwebページの
  Building Kernel & U-Boot Separately From Git Trees の項目に従う
    http://www.rocketboards.org/foswiki/Documentation/GSRD141CompilingLinux

  ・U-bootソースコードの取得
    $ git clone http://git.rocketboards.org/u-boot-socfpga.git 
    $ cd u-boot-socfpga
    $ git checkout -b <BRANCH_NAME> ACDS14.1_REL_GSRD_PR
    <BRANCH_NAME>は任意の好きな名前でよい．
    git cloneで取得したu-boot-socfpgaを<NT_UBOOT>とする．

  ・Linuxソースコードの取得
    $ git clone http://git.rocketboards.org/linux-socfpga.git 
    $ cd linux-socfpga
    $ git checkout -b <BRANCH_NAME> ACDS14.1_REL_GSRD_PR
    <BRANCH_NAME>は任意の好きな名前でよい．
    git cloneで取得したlinux-socfpgaを<NT_LINUX>とする．


=====================================================================
(2) ビルド
=====================================================================
○SafeG

<SAFEG_PACKAGE>/Makefile.config の下記の行を有効にする．

 MONITOR_TARGET=socfpga
 
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
 
○Non-Trust uboot(NT-uboot)

ビルド方法は基本的に以下のサイトのBuilding U-Bootの情報に従う．
  http://www.rocketboards.org/foswiki/Documentation/GSRD141CompilingLinux

<NT-UBOOT>に移動してパッチをあてる．
  patch -p1 < <SAFEG_PACKAGE>/patches/target/socfpga/1_NT_uboot_base.patch

ビルドする．
 $ export CROSS_COMPILE=<LINALO_GCC>/arm-linux-gnueabihf-
 $ make mrproper
 $ make socfpga_cyclone5_config
 $ make

ビルドに成功すると，以下のファイルが生成される．
 u-boot
 u-boot.bin

○Non-Trust Linux(NT-Linux)

ビルド方法は基本的に以下のサイトのBuilding Linux Kernelの情報に従う．
 http://www.rocketboards.org/foswiki/Documentation/GSRD141CompilingLinux

<NT-LINUX>に移動してパッチをあてる．
 patch -p1 < <SAFEG_PACKAGE>/patches/target/socfpga/1_NT_linux_l2.patch
 patch -p1 < <SAFEG_PACKAGE>/patches/target/socfpga/2_NT_linux_com.patch
 
COM用のファイルをコピーする． 
 cp <SAFEG_PACKAGE>/com/src/target/socfpga_nt_linux/doscom_driver_hw.h <NT-LINUX>/safeg/
 cp <SAFEG_PACKAGE>/com/src/linux-driver/doscom.c <NT-LINUX>/safeg/
 
ビルドする．
 
 $ export CROSS_COMPILE=<LINALO_GCC>/arm-linux-gnueabihf-
 $ make ARCH=arm socfpga_defconfig
 $ make ARCH=arm uImage LOADADDR=0x8000
 $ make ARCH=arm dtbs
 $ make ARCH=arm modules
 
<NT-LINUX>/arch/arm/boot/uImage が出来ていれば成功．

=====================================================================
(5) NT-FMP実行
=====================================================================
○デバッガ(ICE)を用いた実行

1.ボードの設定
ボード出荷時のSDメモリ(u-bootが起動するもの)をボードに接続する．

2.ターミナルの接続
Trust側のFMPカーネル用
・JTAG-UARTを使用するため，コマンドラインで以下のコマンドを実行してタ
  ーミナルを開く．
  nios2-terminal.exe --instance=0

NonTrust側のFMPカーネル用
・USB-Serialのポートに対してターミナルを115200bpsで開く．

3.Partner-Jetによるロード
 1.Partner-Jet付属の Altera SoC用のプロジェクト(ALTERA_CycloneV)を基に新
   規プロジェクトをAMPで作成する．
 
 2.コア0のデバッガを起動してターゲットをリセットして実行する．
 
 3.u-bootが起動して，Linuxの起動のカウントダウンになったら，デバッガに
   よりターゲットを停止する．
 
 4.以下のコマンドにより各バイナリをダウンロード． 
 L   "<SAFEG_PACKAGE>/sample/fmp/sample1/trust/fmp_t.elf"
 LA  "<SAFEG_PACKAGE>/sample/fmp/sample1/non-trust/fmp_nt.elf"
 LA  "<SAFEG_PACKAGE>/monitor/monitor.elf"
 
 5.go により実行する．
 
 
○SDカードからのブート

1.SDカードへコピー
ボード出荷時のSDメモリ(u-bootが起動するもの)に，以下のファイルを書き込
む．

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
 fatload mmc 0:1 0x3f000000 monitor.bin
 fatload mmc 0:1 0x3f100000 fmp_t.bin
 fatload mmc 0:1 0x01000040 fmp_nt.bin
 go 0x3f000000

setenv を使用して上記コマンドを保存しておくとよい．
 
=====================================================================
(6) NT-Linux実行
=====================================================================
○デバッガ(ICE)を用いた実行

1.ボードの設定
・ボード出荷時のSDメモリ(u-bootが起動するもの)にNT-LinuxのzImage
  (<NT-LINUX>\arch\arm\boot\zImage)を
  zImage_nt としてコピーする

・ボード出荷時のSDメモリ(u-bootが起動するもの)にNT-Linuxの
  デバイスファイル(<NT-LINUX>/arch/arm/boot/dts/socfpga_cyclone5.dtb)を
  socfpga_cyclone5_nt.dtbとしてコピーする

・SDカードをボードに接続する

2.ターミナルの接続
Trust側のFMPカーネル用
・JTAG-UARTを使用するため，コマンドラインで以下のコマンドを実行してタ
  ーミナルを開く．
  nios2-terminal.exe --instance=0

NonTrust側のFMPカーネル用
・USB-Serialのポートに対してターミナルを115200bpsで開く．

3.Partner-Jetによるロード
 1.Partner-Jet付属の Altera SoC用のプロジェクト(ALTERA_CycloneV)を基に新
   規プロジェクトをAMPで作成する．
 
 2.コア0のデバッガを起動してターゲットをリセットして実行する．
 
 3.u-bootが起動して，Linuxの起動のカウントダウンになったら，デバッガに
   よりターゲットを停止する．
   
   L   "<NT-UBOOT>\u-boot"
   LA  "<SAFEG_PACKAGE>/sample/fmp/sample1/trust/fmp_t.elf"
   LA  "<SAFEG_PACKAGE>/monitor/monitor.elf"

 4.go により実行する．
 
 5.FMPカーネルのコア0実行するべきタスクが存在しなくなるとu-bootが実行
    される．
    
 6.u-bootのプロンプトで以下を実行．
   mmc rescan;
   fatload mmc 0:1 0x8000 zImage_nt; 
   fatload mmc 0:1 0x00000100 socfpga_cyclone5_nt.dtb;
   setenv bootargs console=ttyS0,115200 mem=1000M root=/dev/mmcblk0p2 rw rootwait; 
   bootz 0x8000 - 0x00000100


○SDカードからのブート

次の流れでブートを行う．

1.SPLの実行．
2.u-bootの実行．
3.u-bootによる以下のファイルのロードとSafeGの実行．
  ・monitor.bin(SafeG)
  ・fmp_t.bin(T-FMPカーネル)
  ・u-boot_nt.bin(NT-u-boot)
4.SafeGの実行．
5.T-FMPカーネルの実行．
6.NT-u-bootの実行
7.NT-Linuxの実行

1.ボードの設定
ボード出荷時のSDメモリ(u-bootが起動するもの)に以下をコピーする．

・NT-Linuxの<NT-Linux>/arch/arm/boot/zImage を 
  zImage_nt としてコピー．
・NT-Linuxの<NT-LINUX>/arch/arm/boot/dts/socfpga_cyclone5.dtb を 
  socfpga_cyclone5_nt.dtb としてコピー．
・NT-ubootの"<NT-UBOOT>\u-boot.bin"をu-boot_nt.binとしてコピー．
・ビルドしたセキュア用のfmpのバイナリをfmp_t.binとしてコピー．
・"<SAFEG_PACKAGE>/monitor/monitor.bin"をコピー．

2.ターミナルの接続
Trust側のFMPカーネル用
・JTAG-UARTを使用するため，コマンドラインで以下のコマンドを実行してタ
  ーミナルを開く．
  nios2-terminal.exe --instance=0

NonTrust側のFMPカーネル用
・USB-Serialのポートに対してターミナルを115200bpsで開く．

3.SDカードからのブート
上記のSDを評価ボードに差してSDカードからブートする．

4.u-bootでのロード
u-bootが起動したら，Linuxのブートを止めてコマンドプロンプトにする．
以下のコマンドを実行してSDカードからロードして実行する．

 mmc rescan
 fatload mmc 0:1 0x3f000000 monitor.bin
 fatload mmc 0:1 0x3f100000 fmp_t.bin
 fatload mmc 0:1 0x01000040 u-boot_nt.bin
 go 0x3f000000

setenv を使用して上記コマンドを保存しておくとよい．bootcmd として定義す
ると，次以降は自動的にブートする．

setenv safeg_boot "mmc rescan; fatload mmc 0:1 0x3f000000 monitor.bin; fatload mmc 0:1 0x3f100000 fmp_t.bin; fatload mmc 0:1 0x01000040 u-boot_nt.bin; go 0x3f000000;"
setenv bootcmd "run safeg_boot"
saveenv

5.FMPカーネルのコア0実行するべきタスクが存在しなくなるとNon-Trust側の
  u-bootが実行される．
    
6.u-bootのプロンプトで以下を実行．

mmc rescan;
fatload mmc 0:1 0x8000 zImage_nt; 
fatload mmc 0:1 0x00000100 socfpga_cyclone5_nt.dtb;
bootargs console=ttyS0,115200 mem=1000M root=/dev/mmcblk0p2 rw rootwait; 
bootz 0x8000 - 0x00000100

以下のコマンドを実行しておくと次回以降は自動的にブートする．

setenv bootcmd_nt "run mmcload_nt; run mmcboot_nt"
setenv mmcload_nt "mmc rescan;fatload mmc 0:1 0x8000 zImage_nt; fatload mmc 0:1 0x00000100 socfpga_cyclone5_nt.dtb;"
setenv mmcboot_nt "setenv bootargs console=ttyS0,115200 mem=1000M root=/dev/mmcblk0p2 rw rootwait; bootz 0x8000 - 0x00000100"
saveenv

  
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


●tasksetのビルド

GSRD 14.1 向けのSDイメージには，COMサンプルの実行に必要な taskset がイ
ンストールされてない．次の手順で用意する．

・ホストへのパッケージのインストール
  sudo apt-get install ncurses-dev

・パッケージのダウンロード
  Busy Boxの公式ページからファイルをダウンロード．以下で動作を確認．
     BusyBox 1.23.1

・ビルド
  BusyBox 1.23.1 を展開したフォルダで以下を実行．
  
  $ export CROSS_COMPILE=arm-linux-gnueabihf-
  $ make ARCH=arm defconfig
  $ make ARCH=arm menuconfig

  コンフィギュレーションメニューで以下を設定．
  ・Busybox Settings -> Build Options -> 
    Build Busybox as a static binary (no shared libs)にチェックを入れる。
  ・Miscellaneous Utilities -> taskset にチェック
 
  コンフィギュレーション終了後に以下を実行．
  
  $ make ARCH=arm 
  
  実行ファイル busybox ができていれば成功．
  
・インストール
  ・GSRD 14.1 のイメージを入れたSDをマウント．
  ・上記 busybox を busybox_new としてマウントしたフォルダの /bin にコピーする．
  ・コピーした /bin に移動して以下のコマンドを実行．
     $ ln -s busybox_new taskset

●COMのサンプルプログラムの実行

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
    0x00000000 ～ 0x3fffffff
     0x00000000 ～ 0x00100000 は，内部SRAM等でオーバーラップ可能
     
●SafeG環境での割り当て
  0x00000000 ～ 0x3effffff  : Linux       : 1056M
  0x3f000000 ～ 0x3f0fffff  : SafeG       : 1M* 
  0x3f100000 ～ 0x3f7fffff  : FMPカーネル : 7M*
  0x3f800000 ～ 0x3fffffff  : COM領域     : 8M
        
*Trustに設定．
        
○割込み

●基本構成
・72 ～ 135 : FPGAからHPSへの割込み

●SafeG環境での割り当て

以下の番号の割込みをCOMで使用する．

・134 : Non-Trust -> Trust方向の割込み
・135 : Trust -> Non-Trust方向の割込み

割込み番号はFMPとLinuxで共通

○メモリ・割込み番号の設定変更

COMで使用するメモリや割込み番号を変更するには，以下のファイルを編集す
ること．

./monitor/target/socfpga/target_common.h
./com/src/target/socfpga_nt_linux/doscom_driver_hw.h
<FMP_PACKAGE>/target/socfpga_gcc/target_kernel.h

○起動シーケンス

SafeGを用いてT-FMP/NT-Linuxを動作させる時の起動シーケンスは次の通りで
ある．

・電源ON
・CORE1はリセットがホールドされた状態．
・CORE0実行
  ・ROMローダー
   ・オンチップROMで実行．
   ・オンチップRAMの先頭4KBを使用．
  ・プリローダー(spl)
   ・ROMローダによってオンチップRAMにロードされる
      ・評価ボードではsdのsda3からロードされる。  
   ・u-bootをロード．   
  ・u-boot実行
   ・SafeG(monitor.bin)，T-FMP(fmp_t.bin)，NT-ubootをロード．
  ・SafeG実行．
    ・CORE1のリセット解除
      ・0x0000～0xffffにブートROMをマップしてリセットを解除．
・CORE0/CORE1実行
  ・T-FMPの実行．
    ・CORE0 : T-FMPの実行タスクが存在しなくなれば，SafeGを呼び出して，
              Non-Trust u-bootを実行する
    ・CORE1 : T-FMPの実行タスクが存在しなくなれば，SYSMGRのCPU1STARTADDR  
              に0以外の値が書き込まれるのを待つ．0以外の値が書き込まれ
              たら，SafeGを呼び出してNon-Trustに遷移する．
・CORE0実行
  ・Linuxを実行
   ・CORE1の起動タイミングになれば，SYSMGRのCPU1STARTADDR にCORE2の
     エントリをアドレスに書き込む．
・CORE0/CORE1実行     
  ・Linuxを実行
  

=====================================================================
(9)制限事項
=====================================================================
・メモリ保護設定には未対応 
  現状のSafeG/Linuxでは，メモリコントローラによるRTOS側の保護は未実施
  である．Linuxからの攻撃を防ぐには，SafeGにおいて，適切にメモリコント
  ローラを設定する必要がある．
  
・DMAには未対応
  現状のSafeG/Linuxパッチでは，DMAをセキュアモードで動作させている．
  LinuxからのDMAを介した攻撃を防ぐにはノンセキュアモードにする必要があ
  る．

      
=====================================================================
(10) 変更履歴
=====================================================================
2015/10/01
・(7)COMの使用方法 に問題があったため修正．

2015/09/17
・SafeG 1.2.0 に対応．

2015/07/09
・SafeG 1.1.0 に対応．

2015/03/01
・Helioサポートの追加
  ターゲットボードとしてMacnica社のHelioを追加．
・GSRD 14.1 のサポート
  ベースのLinux環境を GSRD 14.1 に変更．

2013/11/20
・最初のリリース

以上
