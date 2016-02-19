make
if errorlevel 1 goto end1
rem pic32mon -iboot.cmd
rem sleep 2

hex2dump -f main32.hex boot.hex
mphidflash -r -n -w boot.hex

sleep 2
pic32mon -iuser.cmd

:end1
