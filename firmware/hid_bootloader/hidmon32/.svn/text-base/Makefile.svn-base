CC    = gcc
EXECS = hidmon32.exe

OBJS  = \
	main.o \
	hex.o \
	monit.o \
	util.o \
	gr.o \
	portlist.o \
	usb-windows.o	\
	codebuf.o \
	disarm.o \
	mips-dis.o	\
	mips-opc.o	\
	mips16-opc.o	\
	undef.o	

CFLAGS = -DWIN
LDFLAGS = -lhid
WIN32LIB= -lkernel32 -luser32 -lgdi32 -lsetupapi 

#forDEBUG
#CFLAGS += -DDEBUG

all: $(EXECS)

#*.o: hidmon32.h

.c.o:
	$(CC) $(CFLAGS) -c $*.c
 
hidmon32.exe: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(WIN32LIB) -o hidmon32.exe

clean:
	rm -f $(EXECS) *.o core

test:
	./hidmon32.exe

bindist: $(EXECS)
	zip hidmon32-bin-win.zip README.txt CHANGELOG COPYING $(EXECS)

