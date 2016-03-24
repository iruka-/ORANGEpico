# SUPPORT CHIP

* PIC32MX250 (128k Flash 32k SRAM w/USB IE)

* PIC32MX270 (256k Flash 64k SRAM w/USB IE)

* PIC32MX150 (128k Flash 32k SRAM )

* PIC32MX170 (256k Flash 64k SRAM )

# SCHEMATIC DIAGRAM


                   3.3V
                    |
                    *------10Ω--------------+
                   10k                       |
                    |       ___    ___       | 0.1u
      ラ   -->  ----*-MCLR [1  |__| 28] AVDD-*-||---GND
      イ   -->  --PGD3/RA0 [2       27] AVSS--------GND  LED
      ター -->  --PGC3/RA1 [3       26] RB15--1kΩ-------|＞|--GND
             ---HSYNC--RB0 [4       25] RB14
                     --RB1 [5       24] RB13--240Ω------------>Video OUT<---560Ω----RB0(HSYNC/VSYNC)
                     --RB2 [6       23] Vusb3v3--------3.3V
                  SCL2/RB3 [7       22] usb D-
       Xtal     GND----Vss [8       21] usb D+   +10uF
    +-----------------OSC1 [9       20] Vcap------||---GND
    *--|□|--*--------OSC2 [10      19] Vss------------GND
    |  8MHz  |    U1TX/RB4 [11      18] RB9
    22pF    22pF  U1RX/RA4 [12      17] RB8
    |        |   3.3v--Vdd [13      16] RB7
    |        |         RB5 [14      15] Vbus-----------USB Vbus(5V)
    GND    GND              ~~~~~~~~~~
                         PIC32MX250F128B


* Oneboard MicroComputer Board [ORANGE pico](http://www.picosoft.co.jp/orange/) based.


# ToDo

* UART1(U1TX/U1RX) Support.

* NTSC VideoOut Support.

* PS/2 Keyboard Input Support.

* and more...

# Build Instructions for PIC32MX

* USE Linux or Windows ( Python required ) .

* [INSTALL Pinguino-11 p32-gcc Toolchain](http://www.pinguino.cc/download.php)

-

    $ make
       .
       .
       .
    
    CC ../py/../extmod/vfs_fat_file.c
    CC ../py/../extmod/moduos_dupterm.c
    CC main.c
    CC syscalls.c
    p32-gcc -I../lib/netutils -I. -I.. -I/usr/share/pinguino-11/p32/include/non-free/ -Ibuild -march=m4k -EL -msoft-float  -c crt0.S -o build/crt0.o
    LINK build/firmware.elf
       text	   data	    bss	    dec	    hex	filename
      85392	   3524	  18156	 107072	  1a240	build/firmware.elf
    p32-objcopy -O ihex build/firmware.elf build/main32.hex
    p32-objdump -d build/firmware.elf > build/main32.lss
    ./mapdigest.pl
    .startup        0x000000009d002000      0x1bc      444
    .text           0x000000009d003010    0x10ea4    69284
    .rodata         0x000000009d013eb4     0x3c84    15492
    .sdata2         0x000000009d017b38        0x0        0
    .sbss2          0x000000009d017b38        0x0        0
    .data           0x00000000a0000008      0x830 load address 0x000000009d017b38     2096
    .sdata          0x00000000a0000838      0x584 load address 0x000000009d018368     1412
    .sbss           0x00000000a0000dbc       0x1c load address 0x000000009d0188ec       28
    .bss            0x00000000a0000dd8      0x2d0 load address 0x000000009d0188ec      720
    .heap           0x00000000a00010a8     0x4000    16384
    .stack          0x00000000a00050a8      0x400     1024
    .ramfunc        0x00000000a0005800        0x0 load address 0x000000009d0188ec        0
    .pdr            0x0000000000000000        0x0        0
    .comment        0x0000000000000000       0x11       17
    .reginfo        0x0000000000000000       0x18       24
    ----------------------------------------
           total = 106925 (0x1a1ad) bytes


# RUN

* Connect U1TX(RB4) / U1RX(RA4) with TTL Level serial port ( or FT232RL ) 500kBPS.

* type '#' and run Python main.

main.c

    int main_python(int argc, char **argv) {
        mp_init();
        do_str("print('hello world!', list(x+1 for x in range(10)), end='eol\\n')", MP_PARSE_SINGLE_INPUT);
        do_str("for i in range(10):\n  print(i)", MP_PARSE_FILE_INPUT);
        mp_deinit();
        return 0;
    }

* Enjoy!
