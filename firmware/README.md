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

