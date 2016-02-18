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
#MPLAB			=	C:\Program files\MPLAB\MPLAB C32 Suite

# SETME!
#	Microchip Libraries for Applications v2012-10-15 Windows Directory.
MCHPUSB			=	../MicroChip

BOOTLOADER		=	Yes

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
PROCESSOR_O		=	"$(MPLAB)/pic32mx/lib/proc/32MX220F032B/processor.o"

# ------------------------------------------------------------------------------
# flags
# ------------------------------------------------------------------------------

#					-c\
#					-x c++
#					-x c
#					-pedantic -Wall\
#					-std=c99 

CFLAGS		=	-fdollars-in-identifiers $(INCLUDEDIRS) $(BOOTDEFS) -G0
CFLAGS		+=	-D__DEBUG -D__PIC32_FEATURE_SET__=220
CFLAGS		+=	-D__PIC32MX__ -D__$(PROC)__ -D$(BOARD)



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
	@echo $(CC) -c $< -o $@
	@$(CC) $(ELF_FLAGS) $(CFLAGS) $(MIPS16) -c $< -o $@

all: $(OBJS)
	$(CC) $(ELF_FLAGS) $(CFLAGS) -o $(SRCDIR)\main32.elf \
		$(SRCDIR)\main.c \
		$(PROCESSOR_O) \
		$(OBJS)	\
		$(PICLIBS) \
		$(LDFLAGS) \
		-lm -lgcc -lc
	$(OBJDUMP) -d   $(SRCDIR)\main32.elf >main32.lss
	$(OBJC) -O ihex $(SRCDIR)\main32.elf  main32.hex
	-$(RM) config.o
	hex2dump  main32.hex >main32.dmp
	hex2pickit3 main32.hex pickit3.hex


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
	if exist $(SRCDIR)\output.map $(RM) $(SRCDIR)\output.map

usb_device.o :			$(MCHPUSB)/Usb/usb_device.c
	$(CC) $(ELF_FLAGS) $(CFLAGS) -c $< -o $@
#	$(CC) $(ELF_FLAGS) $(CFLAGS) $(MIPS16) -c $< -o $@

usb_function_cdc.o :	$(MCHPUSB)/Usb/CDC\ Device\ Driver/usb_function_cdc.c
	$(CC) $(ELF_FLAGS) $(CFLAGS) $(MIPS16) -c "$<" -o $@

crt.o : crt0.S
	$(CC) $(ELF_FLAGS) $(CFLAGS) -c $< -o $@

ISRwrapper.o : ISRwrapper.S
	@echo $(CC) -c $< -o $@
	@$(CC) $(ELF_FLAGS) $(CFLAGS) -c $< -o $@
