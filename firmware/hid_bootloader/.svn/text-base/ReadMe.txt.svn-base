---------------------------------------------------------------------
	PIC32MX220F032B 用の HID Bootloader
---------------------------------------------------------------------

■ 概要

- これは UBW32用 の HID bootloader をPIC32MX220F032Bチップに対応させたものです。

- Pinguino 用のブートローダーとほぼ互換です。


■ オリジナル・アーカイブの入手先

    An updated HID Bootloader project (zip) is now available

  http://www.schmalzhaus.com/UBW32/



■ 配線         PIC32MX220F032B  内蔵RC発振.

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
               SDA2/RB2 [6       23] Vusb3v3--------3.3V
               SCL2/RB3 [7       22] usb D-
             GND----Vss [8       21] usb D+   +10uF
               *---OSC1 [9       20] Vcap------||---GND
               |        [10      19] Vss------------GND
               |        [11      18] RB9                        *--10k------3.3V
               *----RA4 [12      17] RB8                        |
              3.3v--Vdd [13      16] RB7 -----------------------*--BOOT_SW--GND
                    RB5 [14      15] Vbus-----------USB Vbus(5V)
                         ~~~~~~~~~~

-  水晶を実装せずに、OSC1とRA4を接続してください。
-  RB7にBOOT_SW , MCLRにRESET_SWを実装してください。
-  PIC32MX220F032B の RB15 に接続されたLED を点滅します。
-  PICライターとの接続は上の図ではPGD3/PGC3を使用していますが、組み込み回路の都合によって
   (RA0/RA1を別の回路に接続したい場合など)他のProgramming Portに振り替えてOKです。


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
               SDA2/RB2 [6       23] Vusb3v3--------3.3V
               SCL2/RB3 [7       22] usb D-
    Xtal     GND----Vss [8       21] usb D+   +10uF
 +-----------------OSC1 [9       20] Vcap------||---GND
 *--|□|--*--------OSC2 [10      19] Vss------------GND
 |  8MHz  |    U1TX/RB4 [11      18] RB9                        *--10k------3.3V
 22pF    22pF  U1RX/RA4 [12      17] RB8                        |
 |        |   3.3v--Vdd [13      16] RB7 -----------------------*--BOOT_SW--GND
 |        |         RB5 [14      15] Vbus-----------USB Vbus(5V)
 GND    GND              ~~~~~~~~~~


-  RB7にBOOT_SW , MCLRにRESET_SWを実装してください。
-  PIC32MX220F032B の RB15 に接続されたLED を点滅します。
-  水晶使用時は、Makefileの
	CFLAGS		+=	-DUSE_INTERNAL_FRC_OSC
   をコメントアウト(あるいは削除)して再ビルドしたHEXを焼いてください。

            



            
■ 開発環境 構築
            
- （１）Windows XP / Vista / 7 / 8 のどれかを用意します。

- （２） MPLAB_IDE_8_89.zip を入手してインストールします。
        インストール時の選択で、PIC32用のコンパイラも同時にインストールするようにしてください。

- （３）Microchip Libraries for Applications v2013-06-15 Windows を入手してインストールします。

  microchip-application-libraries-v2013-06-15-windows-installer.exe

- （４）このアーカイブを展開したディレクトリと並列に、MicroChipのUSBフレームワークが来るように
        ディレクトリを配置します。
  
例）  D:\Pic32\MicroChip\USB\     ←Microchip Libraries for Applications 
      D:\Pic32\HIDBoot.X\         ←このアーカイブを展開したもの.


■ コンパイル方法

 -    setenv.bat を実行して、Pinguinoのgccに実行パスを通します。
 -    注意：他の環境(MinGW,Cygwin等)の make.exe ではビルドできません。

 -	  Makefile を編集して、変数 MPLAB のディレクトリを調整してください。
 
 -    make を実行してください。

■ コンパイル上の注意点

- Pinguinoに付属のmake.exe を使用してください。

    PATH D:\x4-easy-rev999\win32\p32\bin;%PATH%

- ここで使用するMakefileはCygwinやMinGWのshellに依存しない (cmd.exeを呼び出す) make
- でないと正しく動かないようです。



■ ファームウェアHEXの書き込み方法

- pic32progを想定しています。
  http://code.google.com/p/pic32prog/

  pic32prog.exeをパスの通った場所に設置してある場合は
  w.bat を起動すると書き込めます。
  
  書き込めたかどうか確かめる場合は、
  r.bat を実行してみてください。

- 各種の書き込み方法は下記ＨＰを参照してください。
  http://hp.vector.co.jp/authors/VA000177/html/PIC32MX.html



■ 動作確認方法

- Pinguinoに付属のmphidflash.exe を起動して、接続メッセージが出ればOKです。
- mphidflashのソースは、以下のＨＰから入手可能です。

  http://code.google.com/p/mphidflash/


■ ファームウェアの動作説明

- Pinguino4.X 付属の MX220用 HID Bootloader互換です。

- RESETあるいは電源投入時、RB7に接続されたBOOT_SWがOFFであれば BOOTLOADERモード
　になります。

- BOOTLOADERモードのときは USB HIDデバイスとして振舞います。

- パソコンから mphidflash.exe を実行すると、指定されたHEXファイルを自分自身の
  9D00_2000～9D00_7FFFの領域に書き込んだ後、0x1000番地単位で開始アドレスをサーチ
  して、その開始アドレスへ分岐します。

- このような動作なので、例えば9D00_4000番地スタートのHEXファイルでも起動させること
  が可能です。
  
- HEXファイルが全く書き込まれていない状態ではBOOT_SWのON/OFF如何にかかわらず、
  BOOTLOADERモードになります。

- BOOTLOADERモードのときはUSBに接続されるとRB15に繋がっているLEDを高速点滅します。

- BOOT_SWを押して、RESETあるいは電源投入した場合は、書き込んだHEXファイルのファーム
　ウェアのほうが起動します。

注意：

  BOOT_SWの(ON/OFF)論理はUBW32と逆になっています。変更したい場合は、HardwareProfile.hの
  #define	Boot_SW_POLARITY	1		// If not PRESSED , Then Run Sketch Program (UBW32 Compat)
  に変更してください。


  ブートローダーから起動されるHEXファイルのファームウェアにて割り込みを使用するときは　
  _CP0_SET_EBASE(0x9FC02000); //(この番地は例です) // 割り込みベクターを設定する。
  というふうにしてベースアドレスを自分の都合の良いアドレス(4k単位のどこか)に割り当てて
  から割り込み許可を出す必要があります。





■ 現在のステータス

- HIDデバイスとして動作OK。
- ブートローダーとしての動作OK。
- -mips16オプション入れて、サイズ最小化済み。8K(+3K)に縮んでいる。

- 拡張機能付き。



■ 拡張機能について。

（１）ユーザーエリアを28kBに拡張しています。
	  また、さまざまな開始番地のアプリを自動的にサーチします。

	以下のアドレスから開始するアプリケーションを起動できます。
	（アドレスの若い方からサーチ）
      9D00_1000		28k
      9D00_2000		24k
      9D00_3000		20k
      9D00_4000		16k
      9D00_5000		12k

    アプリケーションが用意する割り込みベクターは通常なら先頭4kBにあります。
    その場合、割り込みベクターのオフセット０には何も書かない(0xFFFFFFFF)か
    あるいは、リセット処理への分岐命令を置いてください。
    
    Pinguinoスケッチであれば、割り込みベクターは9D00_3000、リセット処理は
    9D00_4000 に配置されているはずです。
    
    が、Flashメモリーを有効に利用するためには、9D00_3000の割り込みベクター
    のうちで、未使用部分にもプログラムを配置して、かつ、開始番地を9D00_3000
    にすることで3kB程度はFlashエリアを有効に使用できます。
    
    さらに、割り込みベクターと開始番地を9D00_2000に変更することで、さらに4kB
    増やすことが出来ます。


（３）以下のコマンドを用意しています。

#define GET_DATA					    0x07	// RAMやPORTからデータを読み込む. 最大56byteまで.
												// 4の倍数サイズであれば intでアクセス. そうでない
												// 場合はmemcpy()を使用してアクセス.

#define PUT_DATA					    0x09	// GET_DATAの逆で、RAMやPORTにデータを書き込む.

#define EXEC_CODE					    0x0a	// 指定アドレスに分岐する.

コマンドのテストルーチンは hidmon32/ 以下にあります。






■ 注意：Config Fuseについて。

main.c:
  #pragma config FPLLODIV = DIV_2         // PLL Output Divider
  ・・・

- main.c 内に #pragma config 文でFuseの設定が入っていますが、これは、MPLAB付属のpic32-gccで
  コンパイルする場合のみ有効なので、Pinguino付属のmips-gccでは完全に無視されます。


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

- 内蔵RCオシレータの使用時はファームサイズが1kBほど大きくなりますので、
  ユーザーエリア的には9d00_2000～7FFFが使用可能領域になります。



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

- USB descripterの省略
	長い文字列は全部１文字にしました。
	そうしないと合計 7kBに入らないからです。




■ ツール

- xdump   :  バイナリーファイルを１６進数でダンプする.

- hex2dump:  HEXファイルを１６進数ダンプリストにする. / HEXファイルのコマンド05をフィルタリングする。

- pic32prog.exe : PicKit2を経由してPIC32MXにHEXファイルを書き込む.




■ 謝辞

  pic32mxやpic32progについてのノウハウの多くをすzさんのＨＰにて勉強させていただきました。
  ここに感謝の意を表します。ありがとうございました。


