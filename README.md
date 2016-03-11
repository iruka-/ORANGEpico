# ORANGEpico でとりあえず遊んでみるページ

PIC32MXを搭載した基板 [ORANGEpico](http://www.picosoft.co.jp/orange/) ユーザーのページです。

# 産業で説明

* ORANGEpicoは電源を入れるだけで、高速なBASICが使えるワンボードコンピューター
* CPUには MICROCHIPの PIC32MX170 （MIPSアーキテクチャー 48MHzクロック ROM 256kB RAM 64kB内蔵）
* でも、BASICだけじゃあ、せっかくの往年の星MIPSチップがもったいないよね。

# 遊び方は、[こちら](https://github.com/iruka-/ORANGEpico/blob/master/firmware/README2.md)

やりたいこと
* USB経由でのファームウェア更新 ..(済)
* MIPS-gcc開発環境を整える..(済)
* ゆくゆくはPythonとか走らせたい..(済)

# ライセンス

* コンパイラやビルド環境は Pinguino-11 を使用しています。
* Pinguino のライセンス形態は GPLv2なので、このリポジトリもGPLv2でリリースします。Fork等はご自由に。
* Pinguino 自体もそうですが、このリポジトリ内にも一部non_freeなヘッダー等がありますので御注意ください。
> (それらにはMICROCHIPのCopyRightが入っています)

# samples/ ディレクトリのステータス

    samples/ --+-- LED_blinker   : いわゆるＬチカ。 (完成済)
               |
               +-- UART1_skelton : UART1エコーバック(完成済)
               |
               +-- NTSC_test     : NTSC Video out テスト(完成済)
               |
               +-- PS2KBD_test   : PS/2キーボード読み取りテスト(完成済)
               |
               +-- tinyBasic2    : tinyBasic2 移植テスト(完成済)
               |
               +-- microPython   : microPython移植テスト(途上)


--------

# 写真

![画像](https://raw.githubusercontent.com/iruka-/ORANGEpico/master/images/ORANGEpico.jpg)

* 基板背景の雑誌（広告写真）は、本記事とは無関係です。

# [開発日記](https://github.com/iruka-/ATMEL_AVR/blob/master/md/README.md)

* AVR , PIC18F , PIC32MX , ARM 系の基板の遊び方[日記](https://github.com/iruka-/ATMEL_AVR/blob/master/md/README.md)

