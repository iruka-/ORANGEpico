
SHELL			=	cmd.exe
HOME			=   C:\pinguino-11
BOARD			=	PIC32_PINGUINO_220
PROC			=	32MX220F032B

# ------------------------------------------------------------------------------
# Makefile.win32 \ 32-bit Pinguino
# Regis Blanchot <rblanchot@gmail.com> 
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
# directories
# ------------------------------------------------------------------------------

SRCDIR		=	.
P32DIR		=	$(HOME)\p32
P32CORE		=	$(HOME)\compilers\p32
BINDIR		=	$(P32CORE)\bin
INCDIR		=	$(P32DIR)\include
LKRDIR		=	$(P32DIR)\lkr
OBJDIR		=	$(P32DIR)\obj\non-free

INCLUDEDIRS	=	-I$(INCDIR)\non-free \
				-I$(INCDIR)\pinguino\core \
				-I$(INCDIR)\pinguino\libraries \
				-I$(LKRDIR) \
				-I$(PDEDIR) \
				-I$(OBJDIR)

#LIBDIRS		=	-L$(OBJDIR)\usb

# ------------------------------------------------------------------------------
# Include object list $(OBJS)
# ------------------------------------------------------------------------------
include ./Objs.mak

# ------------------------------------------------------------------------------
# heap size / Regis Blanchot 2012
# ------------------------------------------------------------------------------

HEAP_SIZE = 16384
ifeq ($(BOARD),PIC32_PINGUINO_220)
	HEAP_SIZE = 512
endif
ifeq ($(BOARD),GENERIC32MX220F032)
	HEAP_SIZE = 512
endif

# ------------------------------------------------------------------------------
# select the cdc library / Jean-pierre Mandon 2012
# ------------------------------------------------------------------------------

CDCLIBRARY = libcdc.a
ifeq ($(BOARD),PIC32_PINGUINO_220)
	CDCLIBRARY = libcdc220.a
endif
ifeq ($(BOARD),GENERIC32MX250F128)
	CDCLIBRARY = libcdc220.a
endif
ifeq ($(BOARD),GENERIC32MX220F032)
	CDCLIBRARY = libcdc220.a
endif

# ------------------------------------------------------------------------------
# select MIPS16/MIPS32 option
# MIPS16 option could drastically reduce code size. 
# ------------------------------------------------------------------------------
__MIPS16_ENABLE__	= true
ifdef __MIPS16_ENABLE__
	MIPS16_INTER = -minterlink-mips16
	MIPS16_OPT   = -mips16
endif

# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
# select linker script 
# ------------------------------------------------------------------------------

#	本番ブートローダー.
#LKRSCRIPT =	selfboot.ld
#	デバッグ用. (hidbootから起動)
LKRSCRIPT =	procdefs.ld


# ------------------------------------------------------------------------------
# commands
# ------------------------------------------------------------------------------

CC				=	$(BINDIR)\p32-gcc.exe
OBJC			=	$(BINDIR)\p32-objcopy.exe
LIBS			=	-lm -lgcc -lc
RM				=	@del
CP				=	@copy
MV				=	@move

# ------------------------------------------------------------------------------
# flags
# ------------------------------------------------------------------------------

DEFINES		=	-D__PIC32MX__ -D__$(PROC)__ -D$(BOARD)

CFLAGS		=	-fdollars-in-identifiers $(INCLUDEDIRS) -msoft-float -ffunction-sections -fdata-sections -Os -G4 $(DEFINES)

# Option: SELECT 12MHz X'tal
#CFLAGS		+=	-DUSE_12MHz_XTAL
# Option: CHANGE BAUDRATE
#CFLAGS		+=	-DBAUDRATE=115200


LDFLAGS		=	$(LIBS)
#LDFLAGS		=	$(LIBDIRS) $(LIBS)

ELF_FLAGS	= -EL -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -march=24kc \
				-msoft-float \
				$(MIPS16_INTER)  \
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
	@$(CC) $(ELF_FLAGS) $(CFLAGS) $(MIPS16_OPT) -c $< -o $@

%.asm : %.c
	@echo $(CC) -S $< -o $@
	@$(CC) $(ELF_FLAGS) $(CFLAGS) $(MIPS16_OPT) -S $< -o $@

all: $(OBJS)
	@echo $(CC) *OBJS* -o $(SRCDIR)\main32.elf
	@$(CC) $(ELF_FLAGS) $(LDFLAGS) $(CFLAGS) -o $(SRCDIR)\main32.elf  \
		$(OBJS)	\
        $(HOME)/user/source/obj/non-free/$(PROC).o \
		-lm -lgcc -lc
	$(OBJC) -O ihex $(SRCDIR)\main32.elf $(SRCDIR)\main32.hex
	hex2dump main32.hex >main32.dmp
	$(BINDIR)\p32-objdump -S main32.elf > main32.lss
	hex2pickit3 main32.hex pickit3.hex

clean:
	-$(RM) main32.elf
	-$(RM) *.o

exec:
	$(OBJC) -O ihex $(SRCDIR)\main32.elf $(SRCDIR)\main32.hex

correct:

crt.o : crt0.S
	@echo $(CC) -c $< -o $@
	@$(CC) $(ELF_FLAGS) $(CFLAGS) -c $< -o $@

ISRwrapper.o : ISRwrapper.S
	@echo $(CC) -c $< -o $@
	@$(CC) $(ELF_FLAGS) $(CFLAGS) -c $< -o $@


