# ディレクトリ構成


    ORANGEpico --+-- firmware --+-- cdcserial2      --- 左側ソケットMX220用の USB-Serial 変換器ファームウェア
                 |              |
                 |              +-- uart_bootloader --- 右側ソケットMX170用の UART Bootloaderファームウェア
                 |              |
                 |              (以下は、おまけです)
                 |              |
                 |              +-- hid_bootloader  --- 左側ソケットMX220用の USB Bootloaderファームウェア
                 |              |                      (MX220のファームを色々書き換えて遊びたい人専用です)
                 |              |
                 |              +-- hid_bootloader16MHz 16MHz水晶用の USB Bootloaderファームウェア
                 |                                     ( 8MHzの水晶を切らしている人専用です)
                 |
                 |
                 |
                 +-- samples  --+-- LED_blinker     --- いわゆるLチカです。bootloaderから実行できるサンプルになっています



* hid_bootloaderとcdcserial2 は PIC32MX 2xx シリーズのCHIPでしか動きません(USBエンジン搭載が必要)

* uart_bootloaderは PIC32MX220用にビルドしていますが、PIC32MX 170 , 250 でも同じHEX で動きます。



# やりたいこと

* USB経由でのファームウェア更新
* MIPS-gcc開発環境を整える

# USB-to-Serial 変換を用意します。

２択（どちらかを選んでください）

* （１）PIC24Fのソケットに PIC32MX220 を挿して、USBシリアル変換を行います。

* （２）別途、市販の3.3V(TTL)レベルのUSBシリアル変換を用意します。


# （１）を選んだ場合はこちら

（２）を選んだ場合は左側ソケットの作業は要りません。

## 基板の準備

* ORANGEpico基板のPIC24Fソケットに28pin ICソケット、8MHz水晶、10uF,0.1uFを実装します。

![画像](https://raw.githubusercontent.com/iruka-/ORANGEpico/master/images/p1.jpg)

## 配線の追加

* 電源供給用のUSB-typeBコネクタのD+,D-信号を、PIC24Fソケット側のD+,D-に配線します。

![画像](https://raw.githubusercontent.com/iruka-/ORANGEpico/master/images/p2.jpg)

* さらに、PIC24F(実際にはPIC32MXを実装)のTXD/RXDとPIC32MX170のTXD/RXDがクロス接続になるように配線を繋ぎます。

> (元々のUSB-メモリー接続用のシリアルではストレートになってしまうようです)

## ファームウェア書き込み

* firmware/cdcserial2/pickit3.hex を PIC32MX220F032B に焼きます。

> (PICKit3が接続できるのは右側ソケットだけなので、差し替えて焼きます)



# （１）、（２）共通

PIC32MX170 ( 右側ソケット ) にUARTなブートローダーを焼きます。

* firmware/uart_bootloader/pickit3.hex を PIC32MX170 に焼きます。

## 動作確認

* （１）USB-typeBでWindowsと接続して、仮想シリアルポートが見えていれば、左側mx220は正常です。

* （２）の場合は調達したUSB-Serial変換器をMX170のTxD/RxDにクロス接続します。

* firmware/uart_bootloader/hostpc/uartmon32.exe を起動して、 「MIPS>  」 プロンプトが出れば成功です。

## トラブルシュート

* USB-Serial変換器とMX170 間の通信速度は 500kBPS 固定です。(8bitパリティ無し) 

* このボーレートに対応できない変換器を使用する場合は、ファームウェアとuartmon32.exeを再構築してください。

* USB-Serial変換器に割り当てられた COMポート番号は、Windowsマシン上では一番大きい番号になる
（すなわち、マザーボード上のCOMxは COM1とか2で、USB-Serialにはそれより大きい番号が振られる）ことを想定してあります。
そうでない場合は、COMxの番号をコマンドラインオプションで指定しないと接続できない場合があります。（uartmon32.exeの改良の余地あり）




## PIC32MX170 ( 右側ソケット ) にUSB経由で自作HEXファイルを焼くには

* firmware/uart_bootloader/hostpc/uartflash32.exe をWindowsのコマンドラインから実行します。

 例）
    D:> uartflash32.exe  main32.hex

* 但し、自作HEXファイルは 0x9D00_0000 から起動するようになっている必要があります。

* 0xBFC0_0000 から3kBのエリアは UART ブートローダーが占有していますので、上書きできません（無視されます）

## UARTブートと、PICKit3直接書き込み の両方に対応するHEXにすることをお勧めします。

* 0xBFC0_0000番地には、0x9D00_0000 への分岐命令を入れておきます。

* そうすることで、UARTブートローダー無しで直接起動するHEXにもなります。

* UARTブートローダーから書き込むときは、この分岐命令は焼かれずに、ブートスイッチの状態がアプリ側になっていれば直接9D00_0000を実行します。


# 応用編

[応用編はこちら](https://github.com/iruka-/ORANGEpico/blob/master/firmware/README3.md)

