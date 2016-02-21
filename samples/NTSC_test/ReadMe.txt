---------------------------------------------------------------------
	PIC32MX250F128B で NTSCグラフィック出力 サンプル.
---------------------------------------------------------------------

■ 概要

-  Pinguino のコンパイラを使用して、NTSCグラフィック出力サンプルをビルドします。
-  スタンドアローンで動作するHEXと、Pinguinoのブートローダーから書き込んで
   実行できるHEXを共通化してあります。どちらで書き込んでも動作します。


■ 解像度
-  240x200dotぐらいです.(VRAMは256x200)

-  SPIクロックは4.8MHzです
-  さらに、32bit SPIモードなので、dotは32の倍数になります

■ NTSC出力の配線
   Video -- 560 RB0
            240 RB13


■ 配線         PIC32MX250F128B 

                3.3V
                 |
                 *------10Ω--------------+
                10k                       |
                 |       ___    ___       | 0.1u
   ラ   -->  ----*-MCLR [1  |__| 28] AVDD-*-||---GND
   イ   -->  --PGD3/RA0 [2       27] AVSS--------GND  LED
   ター -->  --PGC3/RA1 [3       26] RB15--1kΩ-------|＞|--GND
          ---HSYNC--RB0 [4       25] RB14
                  --RB1 [5       24] RB13--240Ω------------>Video OUT<---560Ω----RB0(HSYNC/VSYNC)
                  --RB2 [6       23] Vusb3v3--------3.3V
               SCL2/RB3 [7       22] usb D-
    Xtal     GND----Vss [8       21] usb D+   +10uF
 +-----------------OSC1 [9       20] Vcap------||---GND
 *--|□|--*--------OSC2 [10      19] Vss------------GND
 |  8MHz  |    U1TX/RB4 [11      18] RB9
 22pF    22pF  U1RX/RA4 [12      17] RB8
 |        |   3.3v--Vdd [13      16] RB7
 |        |         RB5 [14      15] Vbus-----------USB Vbus(5V)
 GND    GND              ~~~~~~~~~~
            



            
■ Pinguino 開発環境構築
            
- Windows XP / Vista / 7 / 8 のどれかを用意します。

- Pinguino X.3 安定版を下記サイトから入手してインストールします。
  http://wiki.pinguino.cc/index.php/Main_Page/ja

- Pinguino X.3 コンパイラーにパスを通します。

  setenv.bat
    PATH C:\PinguinoX.3\win32\p32\bin;%PATH%


■ MPLAB 開発環境の追加
            
- MPLAB_IDE_8_89.zip を入手してインストールします。
  インストール時の選択で、PIC32用のコンパイラも同時にインストールするようにしてください。


■ Microchip USBライブラリの追加
            
- 次にMicrochip Libraries for Applications v2012-10-15 Windows を入手してインストールします。

  microchip-application-libraries-v2012-10-15-windows-installer.exe

- このアーカイブを展開したディレクトリと並列に、MicroChipのUSBフレームワークが来るようにディレクトリを配置します。

例）  D:\Pic32\MicroChip\USB\     ←Microchip Libraries for Applications 
      D:\Pic32\pic32mon\          ←このアーカイブを展開したもの.


■ コンパイル方法

- コマンドラインから 

  D:>  make

  でビルドしてください。


■ コンパイル上の注意点

- Pinguinoに付属のmake.exe を使用してください。

    PATH C:\PinguinoX.3\win32\p32\bin;%PATH%

- ここで使用するMakefileはCygwinやMinGWのshellに依存しない (cmd.exeを呼び出す) make
- でないと正しく動かないようです。


■ 書き込み方法

- pic32progを想定しています。
  http://code.google.com/p/pic32prog/

  pic32prog.exeをパスの通った場所に設置してある場合は
  wp.bat を起動すると書き込めます。
  
  書き込めたかどうか確かめる場合は、
  r.bat を実行してみてください。


- 各種の書き込み方法は下記ＨＰを参照してください。
  http://hp.vector.co.jp/authors/VA000177/html/PIC32MX.html


■ Pinguinoのブートローダーからの書き込み方法

- w.bat を実行します。


注意：
	ユーザーエリアを広くするために、プログラム開始番地を9D00_1000に繰り上げています。
	ブートローダーには HIDBoot_mips32gcc.X.zip に同梱されている8KB(+3kB)のサイズの
	ものを使用してください。


■ ファームウェアの動作説明

- VGA(正確にはSVGA 800x600 @ 60Hz)の信号を出力します。
- USB接続して直接ポートを見たり書き換えたりメモリーを覗いたりすることができます。
  pic32mon.exe を使用してください。

- グラフィックRAMの割り当てはoutput.map(コンパイル時に生成されます)で参照できます。
  何も変更しなければたぶん 0xa0000004 番地から 0x1900 バイトあります。
  このメモリーをpic32mon.exeから書き換えることで、グラフィック表示のパターンを
  書き換えできます。

- グラフィック描画関数やコマンドについては未実装です。



■ メモリーマップ（全体）

PIC32のメモリーマップです。
- 物理割り当てされているエリアは 0000_0000 ～ 2000_0000 の512MBです。
- 物理割り当てされている512MBと全く同じものが KSEG0とKSEG1にもマップされます。
- KSEG0とKSEG1の違いはキャッシュ無効/有効で分けられています。

FFFF_FFFF +---------------+
          |               |
          | Reserved      |
          |               |
C000_0000 +---------------+
          | KSEG1(論理)   | Cacheなし.
A000_0000 +---------------+
          | KSEG0(論理)   | Cacheあり.
8000_0000 +---------------+
          |               |
          | Reserved      |
          |               |
          |               |
          |               |
2000_0000 +---------------+
          | 物理メモリー  | ROM/RAM/PORT
0000_0000 +---------------+



■ メモリーマップ（Flash ROM/RAM領域）

A000_7FFF +---------------+
          |               |
          |   SRAM (32kB) |
          |               |
A000_0000 +---------------+

(BFC00BFF)
9FC0_0BFF +---------------+
          |BOOT Flash(3kB)| RESET直後の開始番地はBFC0_0000です。
9FC0_0000 +---------------+ Config Fuseは BFC0_0BF0～BFC0_0BFFの16byteです。
(BFC00000)                  割り込みベクターもBOOT Flash内に置かれます。

9D00_1FFFF+---------------+
          |               |
          |Program Flash  |
          |    (128kB)    |
          |               |
9D00_0000 +---------------+





■ 謝辞

  pic32mxやpic32progについてのノウハウの多くをすzさんのＨＰにて勉強させていただきました。
  ここに感謝の意を表します。ありがとうございました。


