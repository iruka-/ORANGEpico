include ./mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ./py.mk

#
CROSS_COMPILE = p32-
#
OBJCOPY	 = p32-objcopy



#============================
# Running under MS-DOS ?
 ifdef ComSpec
MSDOS=1
 endif

 ifdef COMSPEC
MSDOS=1
 endif
#
#============================

 ifdef MSDOS

PINGUINO_HOME  = C:/pinguino-11
LIBC_DIR   = $(PINGUINO_HOME)/compilers/p32/mips-elf/lib/soft-float/el/
LIBGCC_DIR = $(PINGUINO_HOME)/compilers/p32/lib/gcc/mips-elf/4.9.2/el/

 else

PINGUINO_HOME  = /usr/share/pinguino-11
LIBC_DIR       = /usr/mips-elf/lib/soft-float/el/
LIBGCC_DIR     = /usr/lib/gcc/mips-elf/4.9.2/el/

 endif


PINGUINO_INC   = $(PINGUINO_HOME)/p32/include

INC += -I.
INC += -I..
INC += -I$(PINGUINO_INC)/pinguino/core/
INC += -I$(PINGUINO_INC)/non-free/
INC += -I./include/
INC += -I$(BUILD)

PROC			= 32MX250F128B
#PROC			= 32MX270F256B
DEFINES		    = -D__PIC32MX__ -D__$(PROC)__ -DGENERIC32MX250F128

CFLAGS_PIC32MX  = -march=m4k -EL -msoft-float 

#CFLAGS  = $(INC) -Wall -Werror -ansi -std=gnu99 $(CFLAGS_PIC32MX)
CFLAGS  = $(INC) -Wall -ansi -std=gnu99 $(CFLAGS_PIC32MX)
CFLAGS += -mips16 -minterlink-mips16 -G4
CFLAGS += -ffunction-sections -fdata-sections  $(COPT) $(DEFINES)
#DEBUG = 1

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -ggdb
else
CFLAGS += -Os -DNDEBUG
endif

#/compilers/p32/mips-elf/lib/soft-float/el/
#/compilers/p32/lib/gcc/mips-elf/4.9.2/el/

LDFLAGS  = -L$(LIBC_DIR) -L$(LIBGCC_DIR)

LDFLAGS += -nostdlib -T $(PROC).ld -T elf32pic32mx.x
LDFLAGS += --defsym _min_heap_size=8192
LDFLAGS += -Map=$@.map --cref --gc-sections 


LIBS  = $(PINGUINO_HOME)/p32/obj/non-free/$(PROC).o
LIBS += -lc -lgcc

SRC_C = \
	main.c \
	syscalls.c \
	config.c \
	util.c \
	fifo.c \
	serial1.c \
	interrupt.c \
	printf.c \
	timer.c \
	dma1.c \
	spi2.c \
	graph.c \
	ps2keyb.c \
	undef.c \
	src/builtins_base.c			\
	src/builtins_datetime.c		\
	src/builtins_interpreter.c	\
	src/builtins_math.c	\
	src/eval.c          \
	src/lisp.c          \
	src/lispint.c		\
	src/main.c          \
	src/print.c         \
	src/reader.c		\


SRC_S = \
	crt0.s \
	ISRwrapper.s \

#
#    ↑  本当はISRwrapper.S であるべき所、.sにしないと .oに置換してくれないので、わざと.
#
#	gchelper.s \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o))

all: $(BUILD)/firmware.elf

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@
	$(OBJCOPY) -O ihex $@ build/main32.hex
	p32-objdump -d $@ > build/main32.lss

dump:
	hex2dump.exe build/main32.hex > build/main32.dump
#   ~~~~~~~~~~~~ elf or win32 executable
pickit3:
	hex2pickit3 build/main32.hex pickit3.hex

include ./mkrules.mk

$(BUILD)/crt0.o : crt0.S
	$(CC) $(INC) $(DEFINES) $(CFLAGS_PIC32MX) -c $< -o $@

$(BUILD)/ISRwrapper.o : ISRwrapper.S
	$(CC) $(INC) $(DEFINES) $(CFLAGS_PIC32MX) -fdollars-in-identifiers -c $< -o $@

#
