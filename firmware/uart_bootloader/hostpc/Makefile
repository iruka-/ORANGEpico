CC    = gcc
EXECS = uartmon32.exe
EXECF = uartflash32.exe

#
#	uartmon32
#
OBJS  = \
	monit.o \
	util.o \
	gr.o \
	portlist.o \
	codebuf.o \
	disarm.o \
	mips-dis.o	\
	mips-opc.o	\
	mips16-opc.o	\
	usb-uart.o \
	rs232c.o	\

#
#	uartflash32
#
OBJF  = \
	uartflash.o	\
	util.o      \
	codebuf.o   \
	usb-uart.o  \
	rs232c.o	\

#
CFLAGS  = -DWIN -DWINVER=0x0500 -Wall
WIN32LIB= -lkernel32 -luser32 -lgdi32 -lsetupapi 

#forDEBUG
#
CFLAGS += -DDEBUG

all: $(EXECS) $(EXECF)


.c.o:
	$(CC) $(CFLAGS) -c $*.c
 
$(EXECS): $(OBJS)
	$(CC) $(OBJS) -s $(LDFLAGS) $(WIN32LIB) -o $(EXECS)

$(EXECF): $(OBJF)
	$(CC) $(OBJF) -s $(LDFLAGS) $(WIN32LIB) -o $(EXECF)

clean:
	rm -f $(EXECS) $(EXECF) *.o core

test:
	./$(EXECS)

