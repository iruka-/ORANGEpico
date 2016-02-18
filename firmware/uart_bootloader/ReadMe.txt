---------------------------------------------------------------------
	PIC32MX220F032B 用の HID Bootloader
---------------------------------------------------------------------

■ 概要

- PIC32MX の BOOT FLASH ROM領域(3kB)にぴったり入る UARTブートローダーです。

- これを使うことで、 PC と Pinguino 間の接続トポロジーを限りなくArduino風に
  近づけることが出来ます。

  - UART 経由のprintfデバッグが出来ます。

  - USB Device / Host アプリケーション開発時の PIC32MX ファーム書き込み
    とデバッグに最適です。


■ 配線         PIC32MX220F032B 8MHz水晶使用.

                3.3V
                 |
                 *------10Ω--------------+
                10k                       |
                 |       ___    ___       | 0.1u
   ラ   -->  ----*-MCLR [1  |__| 28] AVDD-*-||---GND
   イ   -->  --PGD3/RA0 [2       27] AVSS--------GND  LED
   ター -->  --PGC3/RA1 [3       26] RB15--1kΩ-------|＞|--GND
                    RB0 [4       25] RB14
                    RB1 [5       24] RB13
               SDA2/RB2 [6       23] 
               SCL2/RB3 [7       22] 
    Xtal     GND----Vss [8       21]          +10uF
 +-----------------OSC1 [9       20] Vcap------||---GND
 *--|□|--*--------OSC2 [10      19] Vss------------GND
 |  8MHz  |    U1TX/RB4 [11      18] RB9                        *--10k------3.3V
 22pF    22pF  U1RX/RA4 [12      17] RB8                        |
 |        |   3.3v--Vdd [13      16] RB7 -----------------------*--BOOT_SW--GND
 |        |         RB5 [14      15] 
 GND    GND              ~~~~~~~~~~


-  RB7にBOOT_SW , MCLRにRESET_SWを実装してください。
-  PIC32MX220F032B の RB15 に接続されたLED を点滅します。
            



            
■ 開発環境 構築
            
- （１）Windows XP / Vista / 7 / 8.1 / 10 のどれかを用意します。(64bit OK)

- （２）Pinguino-11 をダウンロードしてインストールします。
        http://www.pinguino.cc/download.php


■ コンパイル方法

 -    setenv.bat を実行して、Pinguinoのgccに実行パスを通します。
 -    注意：他の環境(MinGW,Cygwin等)の make.exe ではビルドできません。

 -    make を実行してください。

■ コンパイル上の注意点

- Pinguinoに付属のmake.exe を使用してください。


- ここで使用するMakefileはCygwinやMinGWのshellに依存しない (cmd.exeを呼び出す) make
- でないと正しく動かないようです。

PATH C:\Pinguino-11\compilers\p32\bin;%PATH%


■ ファームウェアHEXの書き込み方法

- pic32progを想定しています。
  http://code.google.com/p/pic32prog/

  pic32prog.exeをパスの通った場所に設置してある場合は
  w.bat を起動すると書き込めます。
  
  書き込めたかどうか確かめる場合は、
  r.bat を実行してみてください。

- 各種の書き込み方法は下記ＨＰを参照してください。
  http://hp.vector.co.jp/authors/VA000177/html/PIC32MX.html

- PICKit3で書き込む場合は、main32.hex のかわりに pickit3.hex を書き込んでください。

- 対応PICは MX220 以外に、MX250 , MX170 などもOKです。


■ 動作確認方法

- このアーカイブ内の
  uartboot/hostpc/uartmon32.exe
  を起動して、PIC> プロンプトが出ればOKです。

- Pinguinoのスケッチやアプリケーションを書き込む場合は、
  uartboot/hostpc/uartflash32.exe -r main32.hex
  のようにして書き込みます。(-rオプションで実行まで行います)

- BOOT_SWはオープンでuart bootloaderが起動し、クローズでユーザープログラム
  が起動するようになっています。(UBW32と逆の論理になっています)
  通常の開発時は、リセットボタンのみでスケッチの再書き込みが行えます。

- 基板をアプリ単体起動で使用したい場合は、BOOT_SWを押ボタン式ではなくジャンパー
  のようなものにして、クローズすると良いです。


■ ファームウェアの動作説明

- UARTから操作しますが、コマンドライン上は、Pinguino4.X 付属の mphidflash.exe 互換
 （予定）です。

- RESETあるいは電源投入時、RB7に接続されたBOOT_SWがOFFであれば BOOTLOADERモード
　になります。

- BOOTLOADERモードのときは U1RX/U1TXのシリアルポートを経由して、PCからのコマンド
  を待ちます。

- パソコンから uartflash32.exe を実行すると、指定されたHEXファイルを自分自身の
  9D00_0000～9D00_7FFFの領域に書き込んだ後、0x1000番地単位で開始アドレスをサーチ
  して、その開始アドレスへ分岐します。

- このような動作なので、例えば9D00_4000番地スタートのHEXファイルでも起動させること
  が可能です。
  
- HEXファイルが全く書き込まれていない状態ではBOOT_SWのON/OFF如何にかかわらず、
  BOOTLOADERモードになります。

- BOOTLOADERモードのときは電源投入するとRB15に繋がっているLEDを点滅します。

- BOOT_SWを押して、RESETあるいは電源投入した場合は、書き込んだHEXファイルのファーム
　ウェアのほうが起動します。

注意：

  BOOT_SWの(ON/OFF)論理はUBW32と逆になっています。変更したい場合は、HardwareProfile.hの
  #define	Boot_SW_POLARITY	1		// If not PRESSED , Then Run Sketch Program (UBW32 Compat)
  に変更してください。


■ 拡張機能について。

（１）ユーザーエリアを32kBに拡張しています。
	  また、さまざまな開始番地のアプリを自動的にサーチします。

	以下のアドレスから開始するアプリケーションを起動できます。
	（アドレスの若い方からサーチ）
      9D00_0000		28k
      9D00_1000		28k
      9D00_2000		24k
      9D00_3000		20k
      9D00_4000		16k

    アプリケーションが用意する割り込みベクターは通常なら先頭4kBにあります。
    その場合、割り込みベクターのオフセット０には何も書かない(0xFFFFFFFF)か
    あるいは、リセット処理への分岐命令を置いてください。
    

（３）以下のコマンドを用意しています。

#define GET_DATA					    0x07	// RAMやPORTからデータを読み込む. 最大56byteまで.
												// 4の倍数サイズであれば intでアクセス. そうでない
												// 場合はmemcpy()を使用してアクセス.

#define PUT_DATA					    0x09	// GET_DATAの逆で、RAMやPORTにデータを書き込む.

#define EXEC_CODE					    0x0a	// 指定アドレスに分岐する.

コマンドのテストルーチンは hostpc/ 以下にあります。






■ 注意：Config Fuseについて。

main.c:
  #pragma config FPLLODIV = DIV_2         // PLL Output Divider
  ・・・

- main.c 内に #pragma config 文でFuseの設定が入っていますが、これは、MPLAB付属のpic32-gccで
  コンパイルする場合のみ有効なので、Pinguino付属のp32-gccでは完全に無視されます。


- 今回のPinguino版のビルドでは、Fuseの設定はconfig.c で行います。

- config.c: config fuse の設定ファイルです。すzさんの以下のＨＰから引用いたしました。

  http://suz-avr.sblo.jp/article/57320123.html


■ 再ビルド：Configurationの変更について。

Makefileの以下の部分のコメントアウトを外すことでそれぞれ
- 内蔵RCオシレータの使用
- 8MHzのかわりに12MHz水晶の使用
が可能になります。
- cleanしてビルドしてください。

# Option: SELECT Internal FRC OSC
#CFLAGS		+=	-DUSE_INTERNAL_FRC_OSC

# Option: SELECT 12MHz X'tal
#CFLAGS		+=	-DUSE_12MHz_XTAL



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

A000_1FFF +---------------+
          |               |
          |   SRAM (8kB)  |
          |               |
A000_0000 +---------------+

(BFC00BFF)
9FC0_0BFF +---------------+
          |BOOT Flash(3kB)| RESET直後の開始番地はBFC0_0000です。
9FC0_0000 +---------------+ Config Fuseは BFC0_0BF0～BFC0_0BFFの16byteです。
(BFC00000)                  割り込みベクターもBOOT Flash内に置かれます。

9D00_7FFF +---------------+
          |               |
          |Program Flash  |
          |    (32kB)     |
          |               |
9D00_0000 +---------------+



■ ファームサイズ縮小のための禁断テクニック

- crt0.Sの改造.
	BFC0_0000(kseg1) から 9FC0_0010(kseg0)へのlong jumpはどうしても
	16byte掛かりますが、

_reset:
        la      k0, _startup			# ２命令必要.
        jr      k0                      # Jump to startup code
#		nop								# 遅延スロットはnop

	nopを省略することで、12byteにしています。
	nopの部分には
_startup:
        la      sp,_stack
	の前半の命令(lui) が入ります。
	すなわち、lui sp,high(_stack) が、遅延スロットと飛び先で、都合２回実行されます。
	（実害はありません）

- while() {} ループを do {} while() ループに格下げ
    .bssのクリアと .data のコピーのループを do while ループに格下げしています。
    .bssと.data が零バイトの場合問題がありますが、零バイトでないことがあらかじめ
    確定しているので実害はありません。

- ramfuncコピーの削除
	ramfunc(RAM上で実行される関数)は一切定義していませんので、省略します。

- main()の呼び出しの簡略化
        la      k0, main
        jr      k0                      # Jump to startup code
		nop
	を	jalx	main に置き換えています。
	main関数は常に mips16であることを仮定しています。
	mainの引数(argc,argv)の設定も省略しています。
	また、main()から制御が戻ることはないので無限ループも省略しています。

- 各種例外ベクターの省略
	HID bootloaderは一切の割り込みを使用していませんのでベクターエリアを全部省略しています。
	バスエラー等も端折っています。
	またINTxxxx() 系のシステム関数は di,ei以外ダミーにして、付随する大きなテーブルのリンク
	を防いでいます。

- mips16化と、１回しか呼ばれない小さな関数の適切なinline化、

- コンパイラに与える -Os -G4 オプション (gpアクセス)。
	-G4 を与えると、絶対番地アクセスがgp相対になり、１命令づつ縮みます。
	-G4 より大きな (-G8 -G16等)を与えると逆にコードサイズが増えます。


- 9fc0_0000～9fc0_0bf0 までの領域に詰め込み
	_BOOTROM_ アトリビュートを与えます。
	但し、inline化されることがわかっている関数については、配置がそうなることが確定
	していても _BOOTROM_ は与えずに、static inline とだけ記述します。

	確実にinline化したい場合は、MACRO記述に置き換えるか、そのまま手書きでinlineします。




■ ツール

- xdump   :  バイナリーファイルを１６進数でダンプする.

- hex2dump:  HEXファイルを１６進数ダンプリストにする. / HEXファイルのコマンド05をフィルタリングする。

- hex2pickit3:  HEXファイルを PICKit3で書き込めるメモリー空間のHEXに変換する.

- pic32prog.exe : PicKit2を経由してPIC32MXにHEXファイルを書き込む.




■ 謝辞

  pic32mxやpic32progについてのノウハウの多くをすzさんのＨＰにて勉強させていただきました。
  ここに感謝の意を表します。ありがとうございました。


