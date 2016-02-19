
rem pic32progを使ってHEXを書き込む.
rem
rem 注意：パス設定が必要です。
rem   PATH C:\Pinguino-11\compilers\p32\bin;%PATH%

rem ICSP
rem pic32prog main32.hex

rem BOOTLOADER
uartflash32 -n -r -w main32.hex

