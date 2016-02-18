rem ファーム読み出し.

pic32prog -r boot.bin  0x1fc00000 0x1000
pic32prog -r flash.bin 0x1d000000 0x8000
xdump boot.bin >boot.dmp
xdump flash.bin >flash.dmp
