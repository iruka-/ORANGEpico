# ------------------------------------------------------------------------------
#	Makefile for Pinguino4.X make
# ------------------------------------------------------------------------------
# type:
# C:> PATH D:\x4-easy-rev999\win32\p32\bin;%PATH%

SHELL			=	cmd.exe
#HOME			=   D:\PinguinoX.4
HOME			=   D:\x4-easy-rev999
BOARD			=	PIC32_PINGUINO_220
PROC			=	32MX220F032D

# FIXME!
#   MPLAB C32 Suite Dir ( IMPORTANT! -->  NOT MPLAB-X )
#
MPLAB			=	D:\BROWSER\Pic32\MPLAB\MPLAB C32 Suite
#MPLAB			=	D:\BROWSER\MPLAB8\MPLAB C32 Suite
#MPLAB			=	C:\Program files\MPLAB\MPLAB C32 Suite

# SETME!
#	Microchip Libraries for Applications v2013-06-15 Windows Directory.
MCHPUSB			=	../MicroChip

# ------------------------------------------------------------------------------
# Makefile.win32 \ 32-bit Pinguino
# Regis Blanchot <rblanchot@gmail.com> 
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
# directories
# ------------------------------------------------------------------------------
# HOME, PROC & BOARD are defined in pinguinobetax.py
 
OSDIR		=	win32
#SRCDIR		=	$(HOME)\source
SRCDIR		=	.
P32DIR		=	$(HOME)\p32
P32CORE		=	$(HOME)\$(OSDIR)\p32
BINDIR		=	$(P32CORE)\bin
INCDIR		=	$(P32DIR)\include
LKRDIR		=	$(P32DIR)\lkr\$(BOARD)
OBJDIR		=	$(P32DIR)\obj\non-free

INCLUDEDIRS	=	\
				-I"$(MPLAB)\pic32mx\Include" \
				-I$(MCHPUSB)\include \
				-I$(MCHPUSB)\include\USB \
				-I$(LKRDIR) \
				-I. \
				-I$(OBJDIR) \
				-I$(INCDIR)\non-free \

#				-I$(INCDIR)\pinguino\core \
#				-I$(INCDIR)\pinguino\libraries \

LIBDIRS		=	-L$(OBJDIR)\usb

# ------------------------------------------------------------------------------
# Include object list $(OBJS)
# ------------------------------------------------------------------------------
include ./Objs.mak

# ------------------------------------------------------------------------------
# heap size / Regis Blanchot 2012
# ------------------------------------------------------------------------------

ifeq ($(BOARD),PIC32_PINGUINO_220)
	HEAP_SIZE = 512
else
	HEAP_SIZE = 16384
endif

# ------------------------------------------------------------------------------
# select linker script 
# ------------------------------------------------------------------------------

ifdef BOOTLOADER
	BOOTDEFS = -DUSE_BOOTLOADER
	LKRSCRIPT =	procdefs.ld
# ^^= use HID bootloader

else
	BOOTDEFS = -DUSE_SELFBOOT
	LKRSCRIPT =	selfboot.ld
# ^^= use ICSP Programmer

endif

MIPS16=-mips16

# ------------------------------------------------------------------------------
# commands
# ------------------------------------------------------------------------------

CC				=	$(BINDIR)\mips-gcc.exe
OBJC			=	$(BINDIR)\mips-objcopy.exe
OBJDUMP			=	$(BINDIR)\mips-objdump.exe
BOOTCMD			=	$(BINDIR)\mphidflash.exe
LIBS			=	-lm -lgcc -lc
#-nostdlib
#LIBS			=	-lstdc++ -lm -lgcc -lc
RM				=	@del
CP				=	@copy
MV				=	@move

PICLIBS			=	"$(MPLAB)/pic32mx/lib/no-float/libmchp_peripheral_32MX220F032B.a"

# ------------------------------------------------------------------------------
# flags
# ------------------------------------------------------------------------------

#					-c\
#					-x c++
#					-x c
#					-pedantic -Wall\
#					-std=c99 

CFLAGS		=	-fdollars-in-identifiers $(INCLUDEDIRS) $(BOOTDEFS) 
CFLAGS		+=	-D__DEBUG -D__PIC32_FEATURE_SET__=220
CFLAGS		+=	-D__PIC32MX__ -D__$(PROC)__ -D$(BOARD) -G4 -Os

#CFLAGS		+=	-D__PIC32MX__ -D__$(PROC)__ -D$(BOARD) -g -G4 -Os

# Option: SELECT Internal FRC OSC
#CFLAGS		+=	-DUSE_INTERNAL_FRC_OSC

# Option: SELECT 12MHz X'tal
#CFLAGS		+=	-DUSE_12MHz_XTAL


ELF_FLAGS	=	-EL -Os -ffunction-sections -fdata-sections -march=24kc \

LDFLAGS		=	 $(LIBDIRS)	\
				-msoft-float -Wl,--gc-sections $(MIPS16) \
				-Wl,-L$(OBJDIR) \
				-Wl,--defsym,_min_heap_size=$(HEAP_SIZE) \
				-Wl,-Map=$(SRCDIR)\output.map \
				-T$(LKRSCRIPT) \
				-T$(LKRDIR)\elf32pic32mx.x

#-------------------------------------------------------------------------------
#	rules   
#-------------------------------------------------------------------------------
%.o : %.c
	$(CC) $(ELF_FLAGS) $(CFLAGS) $(MIPS16) -c $< -o $@

%.asm : %.c
	$(CC) $(ELF_FLAGS) $(CFLAGS) $(MIPS16) -S $< -o $@

all: $(OBJS)
	$(CC) $(ELF_FLAGS) $(CFLAGS) -o $(SRCDIR)\main32.elf \
		$(OBJS)	\
		$(SRCDIR)\main.c \
		$(OBJDIR)\$(PROC).o \
		$(PICLIBS) \
		$(LDFLAGS) \
		-lm -lgcc -lc
	$(OBJDUMP) -d   $(SRCDIR)\main32.elf >$(SRCDIR)\main32.lss
	$(OBJC) -O ihex $(SRCDIR)\main32.elf $(SRCDIR)\main32.hex
	-$(RM) config.o
	hex2dump $(SRCDIR)\main32.hex >main32.dmp

lss:
	$(OBJDUMP) -d -S   $(SRCDIR)\main32.elf >$(SRCDIR)\main32.lss

boot:
	hex2dump -f main32.hex boot.hex
	$(BOOTCMD) -n -r -w boot.hex

write:
	pic32prog main32.hex

dump:
	hex2dump $(SRCDIR)\main32.hex >main32.dmp

clean:
	-$(RM) *.o
	if exist $(SRCDIR)\main32.o $(RM) $(SRCDIR)\main32.o
	if exist $(SRCDIR)\main32.elf $(RM) $(SRCDIR)\main32.elf
	if exist $(SRCDIR)\main32.hex $(RM) $(SRCDIR)\main32.hex
	if exist $(SRCDIR)\main32.dmp $(RM) $(SRCDIR)\main32.dmp
	if exist $(SRCDIR)\main32.lss $(RM) $(SRCDIR)\main32.lss

#usb_device.o :			$(MCHPUSB)/Usb/usb_device.c
#	$(CC) $(ELF_FLAGS) $(CFLAGS) $(MIPS16) -c $< -o $@

usb_function_hid.o :	$(MCHPUSB)/Usb/HID\ Device\ Driver/usb_function_hid.c
	$(CC) $(ELF_FLAGS) $(CFLAGS) $(MIPS16) -c "$<" -o $@

crt.o : crt0.S
	$(CC) $(ELF_FLAGS) $(CFLAGS) -c $< -o $@

