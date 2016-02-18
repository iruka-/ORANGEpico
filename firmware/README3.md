# 応用編

* PIC32MX gcc 開発環境を用意する

* 自作HEXをビルドしてUART経由で焼く

* ORANGEpico基板をPinguino基板として使う

# PIC32MX p32-gcc 開発環境の導入

■ 開発環境 構築
            
* １）Windows XP / Vista / 7 / 8.1 / 10 のどれかを用意します。(64bit OK)

* ２）Pinguino-11 をダウンロードしてインストールします。
        http://www.pinguino.cc/download.php




# UART経由で自作HEXを焼くには

* firmware/uart_bootloader/hostpc/uartflash32.exe を使用します。

* Pinguino のブートローダー mphidflash.exe とだいたい同じ使い方です。

* サンプルソースには、uart_bootloaderそのものを使用できます。
  (開始番地を9D00_0000に変更します)

# ORANGEpico基板をPinguino基板にする

* たぶん、Pinguino基板として使えると思います。

* やりかたは、ブートローダーの差し替えと

> uartflash32.exe をmphidflash.exe にリネームして、Pinguinoのブートローダーを差し替える

* MX170基板の定義を追加します。

* Pinguinoには 32MX1xx の定義ファイルが無いのですが、USB IEを使用しない前提で、32MX270 を選べばなんとかなりそうです。

