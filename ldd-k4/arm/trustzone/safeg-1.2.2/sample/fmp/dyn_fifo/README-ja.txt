
                SafeG User Samples: FMP FIFO Sample

          2013 (C) Daniel Sangorrin, Shinya Honda, 太田 貴也 
          
--------
概要
--------

このプログラムは Dynamic SafeG syscallを用いて，2つのOS間でFIFO通信を
行うサンプルプログラムである．

下記は Altera SoC を対象に説明する他のターゲットでも同様の方法で実行可
能である．

------------------
ビルド
------------------
・ディレクトリ構成
次のようなディレクトリ構成にする．
  ./fmp    <-- Altera SoC 向けの簡易パッケージを展開
  ./safeg-1.xxx
    ./app
    ./doc
    ./monitor
    ./patches


・コンフィギュレーションの設定
SafeGのトップフォルダの Makefile.config をターゲットに合わせて設定する．
------------
MONITOR_TARGET=socfpga
------------

・Secure側のASPのビルド方法
./trust で以下を入力
$ cd ./trust
$ make

・Non-Secureのビルド方法
./non-trust で以下を入力
$ cd ./non-trust
$ make

以上．
