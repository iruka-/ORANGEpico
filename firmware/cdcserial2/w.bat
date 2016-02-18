hex2dump -f main32.hex boot.hex
mphidflash -r -n -w boot.hex
