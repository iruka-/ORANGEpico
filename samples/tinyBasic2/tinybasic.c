/*
 * TinyBASIC ::	A tiny BASIC interpreter with limited SDL support
 *
 * TinyBASIC TWO (14th November	2012) Supporting Triton	VDU	& font
 *
 * This	is a C version of a	tiny BASIC implementation.	It is modelled
 * closely along the lines of Dr Li	Cheng Wang's Palo Alto Tiny	Basic which
 * was first published in Dr Dobbs Journal in 1976
 * The original	C code used	here was downloaded	from
 * http://www.programmersheaven.com/download/16060/Download.aspx
 * in June 2012	and	then a number of modifications carried out.
 *
 * This	code has been extended by AFLack to	add	the	following features
 * a simple	code editor	based on entering text with	line numbers
 * various commands	to manage the current program including	save/load
 * added the "let" statement
 * added support for an	array [@()]
 * added support for two functions:	abs(x) and rnd(x)
 * added STEP clause in	FOR	loops (with	negative loops)
 * added support for a DRAW	command	to invoke some libSDL_gfx calls
 * rename END to STOP which	shows the line number
 * re-modelled the expression parser to	support	comparison operators
 * modified	IF to use the new exp parser and to	treat THEN as optional
 * modified	PRINT to use "," only.	Final "," means	no \n
 * modified	PRINT to accept	#x field-width parameters
 * modified	GOSUB to accept	expressions	for	the	line to	call
 * modified	GOTO to	accept expressions for the line	to GOTO
 * NEXT	allows the loop	variable to	be named or	not
 * FOR allows more statements to follow	on the line
 * modified	INPUT to accept	expressions
 * modified	INPUT to allow up to 2 prompts
 * re-wrote	stack code and implemented common stack	for	GOSUB/FOR
 * added edit command to allow easy	re-edit	of an existing statement
 * added trace command (which can also be used from	the	">"	prompt)
 * added twin flavours (vanilla	or raspberry)
 * BASIC statements	entered	without	a line-num are run immediately
 * Shows how long a	program	took to	run	(in	seconds.milliseconds)
 * EDIT	command	supports various control-keys
 * Command history added to	input at the ">" prompt
 * Improved	error reporting	by pin-pointing	the	start of the error token
 * Three flavours: vanilla,	raspberry &	humbug
 *
 * code	now	works correctly	on computers which use only	\n for line	endings
 * Added the following control commands:
 * run			- run the current program
 * list	[line]	- list the current program,	optionally from	<line> onwards
 * edit	<line>	- make changes to <line>
 * load	file	- load from	filename.  Make	<file> current file
 * save	[file]	- save the program.	 If	<file> omitted use current filename
 *				- if filename is different and exists, check before	over-write
 * help	[topic]	- show help	page on	<topic>	of show	all	available topics
 * trace [on|off] -	enable/disable trace mode or show trace	mode
 * flavour [vanilla|raspberry] - select	flavour	or show	flavour
 * renumber	[first[,last][undo]- renumber program with undo	feature
 * exit			- quit interpretor and return to Linux shell
 * new			- clear	the	current	program	(prompt	if changes have	been made)
 * cd			- change directory (a directory	must be	given)
 * pwd			- show current directory
 * ls			- run "ls" to show files in	current	directory
 *
 *
 * Thanks to the following for bug reports and/or suggestions
 * Trevor Hughes
 * Dave	Bodenstab
 *
 * Command-line	argumnets:
 * tinybasic [-fv|-fr|-fh] [-r [file]]
 * -r	: run the loaded file
 * -fv	: select vanilla flavour
 * -fr	: select raspberry flavour
 * -fh	: select humbug	flavour	(Triton	mode)
 * file	: a	tinybasic program file
 *
 * AFL,	22/12/2012
 *
 * $Log: tinybasic.c,v $
 * Revision	2.1	 2012/12/22	17:53:22  afl
 * Changed PRINTF()	to printf()	during initialisation in humbug
 * so that errors are handled.
 *
 * Revision	2.0	 2012/12/22	16:21:23  afl
 * Now includes	support	for	Transam	Triton VDU (humbug flavour)
 *
 * Revision	1.3	 2012/11/10	18:05:34  afl
 * Added renumber command
 * Added command history via <up>/<down> keys
 * Fixed bug in	edit command (if line number was changed)
 * Fixed bug in	save (thanks Trevor	Hughes)
 * Fixed code errors in	C sttng	compares (thanks Dave Bodenstab)
 * Slightly	improved quality of	the	error indication pointer in	serror()
 *
 * Revision	1.2	 2012/10/21	15:57:13  afl
 * Version number in the versionString should now be updated automatically by RCS
 *
 * Revision	1.1	 2012/10/21	15:50:48  afl
 * Fixed usage string which	didn't include new flavour options
 *
 * Revision	1.0	 2012/10/21	15:46:23  afl
 * First version of	the	1.x	series release ("TinyBASIC One")
 *
 * Revision	0.5	 2012/09/01	21:45:29  afl
 * Removed GNU readline().	Created	edit command.
 *
 * Revision	0.5	 2012/09/01	20:48:07  afl
 * Code	now	supports edit <line>.  No longer uses GNU readline()
 *
 * Revision	0.4	 2012/08/22	20:56:51  afl
 * Fixed minor bug in GOTO which only occurred when	GOTO was the last statement
 * in a	program
 *
 * Revision	0.3	 2012/08/12	20:23:04  afl
 * BASIC now closely resembles Palo	Alto Tiny Basic
 * Added CLEAR,	LINE & TEXT	to DRAW
 * Added WAIT DELAY	and	WAIT KEY
 * Added help files
 * Fixed a number of minor bugs
 *
 * Revision	0.2	 2012/08/05	16:10:46  afl
 * Code	extended in	many respects:
 * BASIC now much closer to	Palo Alto (changes to expressions, GOTO/GOSUB/FOR
 * NEXT/INPUT/IF and PRINT)
 * LIST	now	pipes put through more.	 Takes optional	arg: first line	to list
 * added DRAW commands CLEAR, BOX, LINE
 * Fixed bug in	editor when	a new "first" line was entered
 *
 * Revision	0.1	 2012/07/24	17:37:05  afl
 * First Version to	be checked-into	RCS
 *
 * $Id:	tinybasic.c,v 2.1 2012/12/22 17:53:22 afl Rel $
 */

#include "stdio.h"
#include "setjmp.h"
#include "math.h"
#include "ctype.h"
#include "stdlib.h"


struct termios {
	int x;
};

/*
 * From	version	1.0	there are two flavours of tinybasic
 * This	can	be selected	from the command-line with the "-fv" or	"-fr" options
 * The default flavour can be changed at compile-time using
 * -DDEF_FLAVOUR=VER_VANIILA|VER_RASPBERRY
 *
 * Use numbers which allow bit masking
 */
#define	VER_VANILLA		1		/* vanilla flavour */
#define	VER_RASPBERRY	2		/* raspberry flavour */
#define	VER_HUMBUG		4		/* humbug flavour */
#ifndef	DEF_FLAVOUR
#define	DEF_FLAVOUR		VER_VANILLA
#endif
int	Flavour	= DEF_FLAVOUR;

/*
 * if true,	STRICT_FORNEXT implements the FOR/NEXT semantics strictly
 * by always executing the loop	at last	once even if the tests indicate	otherwise
 */
#define	STRICT_FORNEXT	0

//#define	NUM_LAB	1000			/* maximum number of labels	(=numbered lines) */
#define	NUM_LAB	100			/* maximum number of labels	(=numbered lines) */
#define	LAB_LEN	6
#define	PROG_SIZE 50000			/* maximium	size of	a tiny basic program (chars) */

/*
 * token types
 * as the input	is parsed it is	broken down	into one of	these token	types
 * see get_token()
 */
#define	DELIMITER		1
#define	VARIABLE		2
#define	NUMBER			3
#define	COMMAND			4
#define	STRING			5
#define	QUOTE			6
/*
 * AFL has added these new token type
 * see get_token()
 */
#define	ARRAY			7
#define	FUNCTION		8

#define	PRINT	1
#define	INPUT	2
#define	IF		3
#define	THEN	4
#define	FOR		5
#define	NEXT	6
#define	TO		7
#define	STEP	8
#define	GOTO	9
#define	EOL		10
#define	FINISHED  11
#define	GOSUB	12
#define	RETURN	13
/*
 * additional command tokens added to the original Tiny	Basic
 */
#define	LET		14
#define	REM		15
#define	STOP	16
#define	DRAW	17
#define	WAIT	18
#define	TRACE	19
/*
 * additional command tokens only usable in	raspberry flavour
 */
#define	BREAK	30
#define	CONTINUE 31
#define	REPEAT	32
#define	UNTIL	33
#define	WHILE	34
#define	WEND	35
#define	ELSE	36
/*
 * additional command only available in	humbug flavour
 */
#define	VDU		37

char *p_buf;	/* start of	buffer to be analysed */
char *prog;		/* holds expression	to be analyzed */
jmp_buf	e_buf;	/* hold	environment	for	longjmp() */

int	variables[26]= {	/* 26 user variables,  A-Z */
	0, 0,	0, 0, 0, 0,	0, 0, 0, 0,
	0, 0,	0, 0, 0, 0,	0, 0, 0, 0,
	0, 0,	0, 0, 0, 0
};

struct commands	{ /* keyword lookup	table */
	char command[20];
	char tok;
	int  flavours;		/* which flavour the command can be	used in	*/
} table[] =	{			/* Commands	must be	entered	lowercase */
	"print", PRINT,		VER_VANILLA+VER_RASPBERRY+VER_HUMBUG,
	"input", INPUT,		VER_VANILLA+VER_RASPBERRY+VER_HUMBUG,
	"if",	IF,				VER_VANILLA+VER_RASPBERRY+VER_HUMBUG,
	"then",	THEN,			VER_VANILLA+VER_RASPBERRY+VER_HUMBUG,
	"goto",	GOTO,			VER_VANILLA+VER_HUMBUG,
	"for", FOR,			VER_VANILLA+VER_RASPBERRY+VER_HUMBUG,
	"next",	NEXT,			VER_VANILLA+VER_RASPBERRY+VER_HUMBUG,
	"to",	TO,				VER_VANILLA+VER_RASPBERRY+VER_HUMBUG,
	"step",	STEP,			VER_VANILLA+VER_RASPBERRY+VER_HUMBUG,
	"gosub", GOSUB,		VER_VANILLA+VER_RASPBERRY+VER_HUMBUG,
	"return",	RETURN,		VER_VANILLA+VER_RASPBERRY+VER_HUMBUG,
	"let", LET,			VER_VANILLA+VER_RASPBERRY+VER_HUMBUG,
	"rem", REM,			VER_VANILLA+VER_RASPBERRY+VER_HUMBUG,
	"stop",	STOP,			VER_VANILLA+VER_RASPBERRY+VER_HUMBUG,
	"draw",	DRAW,			VER_VANILLA+VER_RASPBERRY,
	"wait",	WAIT,			VER_VANILLA+VER_RASPBERRY+VER_HUMBUG,
	"trace", TRACE,		VER_VANILLA+VER_RASPBERRY+VER_HUMBUG,
	"while", WHILE,		VER_RASPBERRY,
	"wend",	WEND,			VER_RASPBERRY,
	"break", BREAK,		VER_RASPBERRY,
	"continue", CONTINUE,	VER_RASPBERRY,
	"repeat",	REPEAT,		VER_RASPBERRY,
	"until", UNTIL,		VER_RASPBERRY,
	"else",	ELSE,			VER_RASPBERRY,
	"vdu", VDU,			VER_HUMBUG,
	"", 0	 /*	mark end of	table */
};

/*
 * An array	of supported function names
 */
char *functions[] =	{ "rnd", "abs",	0 };

char token[80];
char token_type, tok;

struct label {
	char name[LAB_LEN];
	char *p;	/* points to place to go in	source file*/
};
struct label label_table[NUM_LAB];

char *find_label(),	*gpop();

/*
 * TinyBASIC uses a	new	type of	stack which	holds both
 * for/next	loops and gosub/returns.  This allows a	RETURN
 * to remove all for loops active in the subroutine
 * and for a NEXT executed in an outer loop	to clear inner loops
 *
 * STACK_SIZE is the maximum number	of for loops and gosubs	nexted
 * as there	are	only 26	variables, a depth of 100 is plenty
 */
#ifndef	STACK_SIZE
#define	STACK_SIZE		100
#endif
#define	STACK_FOR		1
#define	STACK_GOSUB		2
#define	STACK_WHILE		3
#define	STACK_REPEAT	4
struct forsub {
	int frame_type;		/* type	of object STACK_FOR	or STACK_GOSUB */
	int var;				/* FOR variable	*/
	int target;			/* FOR end value */
	int step;			/* FOR STEP	value */
	char	*loc;			/* FOR or RETURN address */
};
struct forsub stack[STACK_SIZE];
struct forsub spop();
int	   sp =	0;			/* stack pointer */

void exec_print(), scan_labels(), find_eol(), exec_goto();
void exec_if(),	exec_for(),	exec_next(), exec_input();
void exec_gosub(), exec_return(), label_init();
void serror(), get_exp(), putback();
void level2(), level3(), level4(), level5(), level6(), level7(), primitive();
void unary(), arith();
void exec_let();
char *progLineNum();
void exec_trace();

/*
 * additional language options (available in raspberry flavour)
 */
void exec_while(), exec_wend();
void exec_break(), exec_continue();
void exec_repeat(),	exec_until();
void exec_else();

int	Trace =	0;			/* if true show	statement execution	*/

/*
 * additional language options (available only in humbug flavour)
 */
void exec_vdu();

int	inch( char *s, int len );
int	stredit( char *str,	int	maxlen,	int	flags );
int get_token();
int	find_var(char *s);
int iswhite(char c);
int isdelim(char c);
void assignment();
int look_up(char *s);
int get_next_label(char	*s);
int	renumber( char *orig, char *new, int maxlen	);
int	run( char *runme, int showstop );

//FIXME! 未定義
void vdu( int pos, unsigned	char c ) {}
void prun( char	*c ){}
void SDL_Quit(){}
void execute_time() {}
void closeScreen( char *s ){}

/*
 * Pipes
 */
FILE *popen(const char *command, const char *type)
{
	static char fd[]="(NULLP)";
	return fd;
}
int   pclose(FILE *stream)
{
	return 0;
}

void outch(	unsigned char c	);

/* end of tiny basic globals */
/****************************************************************/
/*
 * BEGIN additional	code to:
 * 1) manage basic programs
 * 2) extend BASIC with	various	new	features
 * 3) interface	with the SDL_gfx library
 */

#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <term.h>
//#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#if		0
#include <sys/wait.h>

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_gfxPrimitives.h>
#endif

typedef	unsigned char  Uint8;
typedef	short	       Sint16;
typedef	unsigned short Uint16;
typedef	unsigned int   Uint32;
typedef	void SDL_mutex;
typedef	void SDL_Surface;

#define	readyString		"Ready"
#define	promptString	"> "

/*
 * %s in the welcome string	is replaced	by the flavour setting
 * Needs a RCS check-out with co option	"-kv"
 */
#define	welcomeString	"This is %s TinyBASIC Two"
#define	versionString	"2.1 23/12/2012"
#define	progname		"tinybasic"		/* used	by usage-errors	*/

/*
 * The program is stored in	a linked-list of structures
 */
struct _progListElem {
	int lineNumber;				/* the line	number */
	char	*lineText;				/* the line	text itself	*/
	struct _progListElem	*next;	/* pointer to the next list	element	*/
};
/*
 * TinyBASIC has two program buffers
 * Work	is the normal working buffer, but when a 'renumber'	is used
 * the working program is copied to	Undo.
 * each	buffer has it's	own	'head' record and flag to indicate if the
 * the buffer has been changed
 */

#define	PROGLIST_ORIG	1
#define	PROGLIST_RENUM	2
/*
 * global to record	which program buffer is	in use:	ORIG or	RENUM
 * the "renumber undo" swaps the lists back	only if	the	current	list is	RENUM
 */
int	whichProgList =	PROGLIST_ORIG;

struct _progList {
	struct _progListElem	*first;			/* head	record */
	int	  bufferUnsaved;				/* buffer has unsaved changes */
};
struct _progList Work;
struct _progList Undo;
struct _progList temp;			/* used	for	swapping Orig <-> Renum	*/

char *currFileName = (char *)0;	/* holds filename of current program */

/*
 * the basic editor	can	handle inserting new text or deleting existing lines
 */
#define	TEXT_DELETE		1
#define	TEXT_INSERT		2

/*
 * when	renumbering	the	program, a lookup-table	is required	between
 * old line	numbers	and	new.  This is build	from a linked list of
 * these structures...
 */
struct _renumElem {
	int old;
	int new;
	struct _renumElem *next;
};
struct _renumElem *renumberTableHead = (struct _renumElem *)0;

/*
 * The @() array is	implemented	by storing values in a linked-list which
 * is sorted into index-order
 */
struct _arrayListElem {
	int index;					/* array index */
	int value;					/* value stored	at this	index position */
	struct _arrayListElem *next;	/* pointer to next array index */
};
struct _arrayListElem *arrayHead = (struct _arrayListElem *)0;

/*
 * define what the interpretor should return when requested	for	an
 * undefined index position.  Choices are a	value (typically 0)	or
 * "-1"	meaning	"throw an error"
 */
#ifndef	ARRAY_UNKNOWN_ELEM
#define	ARRAY_UNKNOWN_ELEM		0
#endif

/*
 * The editor implements a number of primitive commands
 * These are defined in	the	following table
 * The table is	null-terminated
 * Although	the	commands are in	lowercase, case	is not important when matching
 */
char *commandTable[] = {
	"new",				/* discard the edit	buffer and start again */
	"list",				/* show	the	program	*/
	"run",				/* run the BASIC program */
	"edit",				/* edit	a line of code */
	"load",				/* load	the	editor from	a saved	file */
	"ls",				/* show	the	names of saved files */
	"cd",				/* change working directory	*/
	"pwd",				/* show	working	directory */
	"save",				/* save	the	edit buffer	to a named file	*/
	"quit",				/* exit	the	editor back	to the UNIX	shell */
	"exit",				/* as quit */
	"trace",				/* enable/disable trace	mode */
	"flavour",			/* select flavour */
	"help",				/* provide simple get-you-going	help */
	"renumber",			/* renumber	program, with optional undo	*/
	0
};

/*
 * Help
 * Help	files are stored in	/usr/share/tinybasic/help
 * the command "help" list the files to	show the user what's available
 *
 * This	can	be over-ridden in the Makefile with	-DHELPDIR="pathname"
 */
#ifndef	HELPDIR
#define	HELPDIR			"/usr/share/tinybasic/help"
#endif

/*
 * ^C program interruption
 */
int	userStopRequest	= 0;		/* BASIC STOPs when	this is	non-zero */

/*
 * to implement	else (as in	if-else) we	need to	keep information
 * about if	statements
 */
struct _if {
	char	line_num[LAB_LEN];		/* statement line number */
	int	test;					/* result of the if-test */
	int	skip_else;				/* true	if we're skipping else clauses */
} Lastif;

/*
 * History (added in v1.3)
 * Each	statement entered at the ">" prompt	is saved in	the	history
 * buffer.	<up> and <down>	keys can be	used to	replay previous	statements
 */
#ifndef	HISTORY_SIZE
#define	HISTORY_SIZE	100		/* command prompt history "depth" */
#endif

/*
 * the command history variables
 */
static char	*historyBuf[HISTORY_SIZE];
static int	freeHist = 0;		/* pointer to next free	slot in	historyBuf[] */
static int	lastHist = 0;		/* pointer to next slot	to return for <up> */

/*
 * The stredit() function now supports history via <up>/<down>
 * and takes a parameter which enables/disables	history.
 * Use one of these	defines
 */
#define	STREDIT_HIST_INIT		1
#define	STREDIT_HIST_ENABLE		2
#define	STREDIT_HIST_DISABLE	3

/*
 * improved	error diagnostics
 * the get_token() function	adjusts	*prog as it	parses the program buffer
 * when	an error occurs, *prog is used to identify the point where the
 * error has occured.  However this	is left	pointing to	the	end	of the
 * token sequence which	gives rise to the error.
 * To improve error	reporting, get_token() takes a copy	of *prog
 * in *cur_token which is subsequently used	by serror()
 */
char *cur_token;

/*
 * definitions for interfacing with	SDL	and	SDL_gfx
 *
 * predefined colours (all with	maximum	alpha value)
 */
struct _colours	{
	char	*name;
	Uint32 value;
} predef_colours[] = {
	"red",		0xff0000ff,
	"green",		0x00ff00ff,
	"blue",		0x0000ffff,
	"yellow",	0xffff00ff,
	"cyan",		0x00ffffff,
	"magenta",	0xff00ffff,
	"white",		0xffffffff,
	"black",		0x000000ff,
	"raspberry",	0xed0f6dff,
	0, 0
};
#ifndef	DEFAULT_COLOUR
#define	DEFAULT_COLOUR "white"
#endif

/*
 * the draw	commands all have optional colour and alpha	parameters
 * these next two globals hold the current working colours and alpha values
 */
Uint32 currentColour	= 0xff;
int	   currentAlpha		= 255;

#ifndef	WINDOW_WIDTH
#define	WINDOW_WIDTH  640;
#endif
#ifndef	WINDOW_HEIGHT
#define	WINDOW_HEIGHT 480;
#endif
const char *WINDOW_TITLE = "TinyBASIC";

int	Screen_height =	WINDOW_WIDTH;
int	Screen_width  =	WINDOW_HEIGHT;

/*
 * note	that when in humbug	flavour	the	screen size	is fixed at
 * 1024	x 384 (corresponding to	Triton's 16	lines of 64	characters)
 */

/*
 * Special Fonts
 * TinyBASIC can import	fonts in special ascii font	loader (afl) format
 */
#ifndef	FONTDIR
#define	FONTDIR			"/usr/share/tinybasic/fonts"
#endif
#define	TRITONFONT		"triton.afl"	/* triton font */

char FontName[80];				/* Font	Name */
int	 GlyphWidth	 = 0;			/* width of	each glyph (pixels)	*/
int	 GlyphHeight = 0;			/* height of each glyph	(pixels) */
int	 GlyphCount	 = 0;			/* number of glyphs	in loaded font */
unsigned char *FontBase	= 0;	/* pointer to the loaded font buffer */

/*
 * HUMBUG screen
 * In Humbug flavour TinyBASIC creates a window	allowing it	to display
 * characters using	the	Triton font
 * TinyBASIC emulates the Thompson CSF SFC96364	VDU	chip
 * This	has	1024 bytes of screen memory	organised as 16	lines of 64	bytes
 * The code	supports various controls, "hardware" scroll, clear-screen and
 * a cursor
 */
struct _sfc {
	unsigned	char vduram[1024];	/* screen memory */
	int rows;					/* number of rows */
	int cols;					/* number of columns */
	int cursor_pos;				/* screen address of cursor	*/
	int cursor_row;				/* screen row of cursor	*/
	int topleft;					/* screen address of top-left pos */
	int showcursor;				/* 0=hide cursor, 1=show cursor	*/
	SDL_mutex *scn_lock;			/* mutex permitting	screen access */
};
#ifndef	HUMBUG_WIN_COLS
#define	HUMBUG_WIN_COLS			64
#endif
#ifndef	HUMBUG_WIN_ROWS
#define	HUMBUG_WIN_ROWS			16
#endif
#define	PHOSPHOR_AMBER			0xc0a000ff
#define	PHOSPHOR_GREEN			0x10c000ff
#define	PHOSPHOR_WHITE			0xa0a0a0ff
#ifndef	HUMBUG_PHOSPHOR_COLOUR
#define	HUMBUG_PHOSPHOR_COLOUR	PHOSPHOR_AMBER
#endif

#define	HUMBUG_CURSOR_BLINK_RATE		400		/* milli-seconds on	or off */
#define	HUMBUG_CURSOR_ROW_START	18				/* pixel row start */
#define	HUMBUG_CURSOR_ROW_END	19				/* pixel row end */
#define	HUMBUG_CURSOR_COL_START	2				/* pixel col start */
#define	HUMBUG_CURSOR_COL_END	12				/* pixel col start */

struct _sfc	Vmem;				/* the working screen space	*/


SDL_Surface	*Screen		= (SDL_Surface *)0;
/*
 * DRAW	COMMAND	TEMPLATES
 * The draw	command	uses these templates to	assist in the semi-automtic	decoding
 * of the various draw sub-commands.  See argParser() for details
 */
#define	DRAWTP_START	"dQdNDN"				/* start */
#define	DRAWTP_POINT	"DNDNdCdN"				/* point */
#define	DRAWTP_CIRCLE	"DNDNDNdNdCdN"			/* circle */
#define	DRAWTP_TRIANGLE	"DNDNDNDNDNDNdNdCdN"	/* triange */
#define	DRAWTP_RECT		"DNDNDNDNdNdCdN"		/* rectangle / box */
#define	DRAWTP_LINE		"DNDNDNDNdNdCdN"		/* line	*/
#define	DRAWTP_TEXT		"DNDNDQdCdN"			/* text	*/
#define	DRAWTP_CLEAR	"dC"					/* clear */

/*
 * PRINTF
 * Version of printf() which outputs appropriately according to	flavour
 * All normal TinyBASIC	output (i.e. except	debug output) should use this
 * call, rather	than standard "printf()"
 *
 * Page	Mode.  If called with fmt == (char *)1 page	mode is	enabled
 * In this mode	the	routine	prints "---MORE?---" after 15 lines	have
 * been	output.	 Pressing <space> or <return> returns 0.  Pressing
 * <q> returns -1.	Page mode is only available	in Humbug flavour.
 */

int	PRINTF(	char *fmt, ... )
{
	va_list	ap;
	char	tmp[BUFSIZ];
	char	*p,	*m;
	char	*moremsg = "MORE?";
	char	key[25];
	static int pagemode = 0;
	static int scrnlines	= 0;

	va_start( ap, fmt );
	if( Flavour == VER_VANILLA || Flavour ==	VER_RASPBERRY )	{
		vprintf( fmt,	ap );
		return(	0	);
	} else {
		if( fmt	== (char *)0 ) {
			pagemode =	0;
			return(	0 );
		}
		if( fmt	== (char *)1 ) {
			pagemode =	1;
			scrnlines = 0;
			return(	0 );
		}
		vsprintf(	tmp, fmt, ap );
		for( p=tmp; *p;	p++ ) {
			if( *p	== '\n'	) {
				vdu( 0,	'\r' );
				scrnlines++;
			}
			vdu( 0,	*p	);
			if( *p	== '\n'	&& pagemode	&& scrnlines%(HUMBUG_WIN_ROWS-1) ==	0 ) {
				for( m=moremsg;	*m;	m++	)
					vdu(	0, *m );
				/*
				 * wait	for	<q>	or <space> or <return>
				 */
				inch( key, sizeof( key ) );
				vdu( 0,	29 );				/* cursor to line start	*/
				vdu( 0,	13 );				/* erase "more?" text */
				if(	strcasecmp(	key, "q" ) == 0	)
					return(	-1 );
				if(	strcasecmp(	key, "\033"	) == 0 )
					return(	-1 );			/* escape */
				return(	0 );
			}
		}
	}
	return 0;
}

/*
 * FGETS
 * Works exactly like fgets() but correctly	works with Humbug flavour
 */

char *FGETS( char *s, int len, FILE	*stream	)
{
	*s =	'\0';					/* clear string	before collecting input	*/
	stredit(	s, len,	STREDIT_HIST_DISABLE );
	strncat(	s, "\n", len );
	//printf( "FGETS: returning string \"%s\"", s );
	return(	s );
}

/*
 * INCH
 * Read	a string from either stdin or SDL and return it
 * Return null-terminated string in	supplied buffer
 *	 (ensuring it doesn't overflow)
 * In Vanilla or Raspbery flavour, use read() on stdin
 * In Humbug use SDL.  Return control keys (like <up>) as key-sym names
 *	 (enclosed in angle-brackets)
 * Return value	is number of keys being	returned, or 0 = end-of-file
 *	 or	-1 = error
 */
/**	General	event structure	*/
typedef	union SDL_Event	{
	Uint8 type;
	/*
			SDL_ActiveEvent	active;
			SDL_KeyboardEvent key;
			SDL_MouseMotionEvent motion;
			SDL_MouseButtonEvent button;
			SDL_JoyAxisEvent jaxis;
			SDL_JoyBallEvent jball;
			SDL_JoyHatEvent	jhat;
			SDL_JoyButtonEvent jbutton;
			SDL_ResizeEvent	resize;
			SDL_ExposeEvent	expose;
			SDL_QuitEvent quit;
			SDL_UserEvent user;
			SDL_SysWMEvent syswm;
	*/
} SDL_Event;
#define	SDL_ENABLE		1
#define	SDL_KEYDOWN	2
#define	SDL_QUIT		3

void user_putc(char c);
void user_puts(char *s);
int  user_getc(void);

int	inch( char *s, int len )
{
	// キーボードから1文字(もしくは文字シーケンス)取得する. 最大5バイトまで.
	int c;
	while(1) {
		c = user_getc();
		if(c!=(-1)) break;
	}
	// 改行文字は '\n'に置き換える.
//	if(c==0x0d) {
//		c=0x0a;
//	}
	
	s[0]=c;
	s[1]=0;
	return(	1 );

#if		0
	int i =	0;
	SDL_Event event;

	if( Flavour == VER_VANILLA || Flavour ==	VER_RASPBERRY )	{
		/*
		 *	read stdin
		 */
		i = read( 0, s,	len-1 );
		if( i >= 0	) *(s+len-1) = '\0';
		return(	i );
	} else {
		/*
		 * get key via SDL keyboard event...
		 */
		SDL_EnableUNICODE( SDL_ENABLE	);
		//printf(	"INCH: about to wait for an event...\n"	);
		while( SDL_WaitEvent(	&event ) ) {
			switch(	event.type	) {
			case SDL_KEYDOWN:
				//printf( "INCH: SDL_KEYDOWN event...\n" );
				if(	event.key.keysym.unicode ==	0 )
					snprintf( s,	len-1, "<%s>", SDL_GetKeyName( event.key.keysym.sym	) );
				else
					snprintf( s,	len-1, "%c", (Uint16)event.key.keysym.unicode );
				*(s+len-1) = '\0';
				if(	*s == 0x3 )
					return(	0 );				/* mimic ^C	interrupt */

				return(	1 );
			case SDL_QUIT:
				//printf( "INCH: SDL_QUIT event...\n" );
				return(	0 );
			}
		}
	}
#endif
	return 0;
}
/*
 * ADDTOBUFFER
 * Purpose:	add	a line to a	linked-list	buffer
 * Given: line number
 *		  pointer to a line	of text
 *		  structure	holding	head record	and	unsaved-flag
 *
 * Return: zero	for	success, non-zero for "out of memory"
 * If the new string is	a null pointer then	delete the indicated line
 * Manage the buffer's flag	indicating that	the	buffer has changed
 */
int addtoBuffer( int ln, char *s, struct _progList *saveBuf	)
{
	int	 mode =	0;
	char	*cp;
	struct _progListElem	*LP	  =	(struct	_progListElem *)0;
	struct _progListElem	*prev =	(struct	_progListElem *)0;
	struct _progListElem	*new  =	(struct	_progListElem *)0;

	/*
	* is the new line of text empty? (indicating a delete operation)
	*/
	if( s ==	(char *)0 )	 mode =	TEXT_DELETE;
	else					 mode =	TEXT_INSERT;

	if( mode	== TEXT_INSERT ) {
		/*
		 * take	a	copy of	the	text
		 */
		if( (cp	=	strdup(	s )) ==	(char *)0 )
			return(	1 );
	}
	/*
	* create a node	on the heap	for	this line
	*/
	if( (new	= (struct _progListElem	*)malloc( sizeof( struct _progListElem ))) == (struct _progListElem	*)0	) {
		return(	1	);
	}
	new->next	   = (struct _progListElem *)0;
	new->lineNumber	= ln;
	if( mode	== TEXT_INSERT )
		new->lineText	= cp;

	/*
	* now we need to find the place	in the list	for	this line
	*/
	if( saveBuf->first == (struct _progListElem *)0	) {
		/*
		 * The buffer	is empty, add the new record as	the	Head record
		 */
		if( mode == TEXT_INSERT	) {
			saveBuf->first	= new;
		}
		saveBuf->bufferUnsaved++;
		return(	0	);
	}
	/*
	* otherwise	look for the correct place in the list...
	*/
	prev	= (struct _progListElem	*)0;
	for(	LP=saveBuf->first; LP; prev=LP,	LP=LP->next	) {
		if( LP->lineNumber < new->lineNumber )
			continue;
		else if( LP->lineNumber	== new->lineNumber ) {
			/*	printf(	"replacing line %d\n", LP->lineNumber ); */
			if( mode == TEXT_DELETE	) {
				/*
				 * we have been	asked to delete	this line
				 * make	prev->next point to	this record's next node
				 */
				if(	prev ==	(struct	_progListElem *)0 ) {
					/*
					* we are being asked to	delete the Head	node
					*/
					saveBuf->first =	LP->next;
					free( LP->lineText );
					free( LP	);
					saveBuf->bufferUnsaved++;
					return(	0 );
				}
				/*
				 * we are being	asked to delete	a mid-list node
				 */
				prev->next = LP->next;
				free( LP->lineText );
				free( LP );
				saveBuf->bufferUnsaved++;
				return(	0 );
			}
			/*
			 *	we need	to replace this	node with the new one
			 */
			new->next = LP->next;
			if( prev == (struct _progListElem *)0 )	{
				//printf( "new head record\n" );
				/*
				 * this	record is the first	record in the list
				 */
				saveBuf->first = new;
			} else {
				/* printf( "new ordinary record\n" ); */
				prev->next = new;
			}
			/*
			 *	and	free-up	memory used	by this	node
			 */
			free( LP->lineText	);
			free( LP );
			saveBuf->bufferUnsaved++;
		} else {
			/*
			 *	we need	to insert the new node before "LP"
			 */
			if( prev == (struct _progListElem *)0 )	{
				//printf( "new head record\n" );
				/*
				 * this	record is the first	record in the list
				 */
				new->next =	saveBuf->first;
				saveBuf->first = new;
			} else {
				prev->next = new;
				new->next =	LP;
			}
		}
		break;
	}
	if( LP == (struct _progListElem	*)0 ) {
		/*
		 * we	must have reached the end of the list without a	match
		 */
		prev->next = new;
	}
	saveBuf->bufferUnsaved++;
	return(	0 );
}

/*
 * YESORNO
 * General function	to print a message and wait	for	one	of two possible
 * responses.  Repeat the question until one response is received.
 * Given: pointer to message to	print (and repeat as necessary)
 *	pointers to	the	the	two	responses
 * Return: 1=first response	received, 2=second response	received
 */

int	yesORno( char *msg,	char *r1, char *r2 )
{
	int	gotresponse	= 0;
	char	input[BUFSIZ];

	while( !gotresponse ) {
		PRINTF(	"%s (%s/%s) ", msg, r1,	r2 );
		fflush(	stdout );
		FGETS( input,	sizeof(input)-1, stdin );
		/*
		 * remove	trailing newline (if present)
		 */
		if( input[ strlen(input)-1 ] ==	'\n' )	input[strlen(input)-1] = '\0';
		/*
		 * check for valid response
		 */
		if( strcasecmp(	input, r1	) == 0 )  return( 1	);
		if( strcasecmp(	input, r2	) == 0 )  return( 2	);
		PRINTF(	"Please enter \"%s\" or \"%s\" only\n\n",	r1,	r2 );
	}
	return 0;
}

/*
 * ARRAYSTORE
 * store a value in	the	array
 * given: integer values for the array index (0	is first element) and value
 * return: void	(fail safe if given	a negative index)
 */
void arrayStore( int index,	int	value )
{
	struct _arrayListElem *cur, *prev, *new;

	if( index < 0 )	return;		/* do nothing */

	/*
	* create a new node	to hold	this array value
	*/
	if( (new	= (struct _arrayListElem *)malloc( sizeof( struct _arrayListElem ) )) == (struct _arrayListElem	*)0	) {
		return;					/* do nothing */
	}
	new->index =	index;
	new->value =	value;
	new->next  =	(struct	_arrayListElem *)0;

	/*
	* now find the position	in the linked list to insert this index
	*/
	prev	= (struct _arrayListElem *)0;
	for(	cur=arrayHead; cur && index>cur->index;	prev=cur, cur=cur->next	)
		;
	/*
	* the loop stops for one of	the	following reasons:
	* 1. we've run out of records, so append "new" to list
	* 2. we've found the exact same	index match, so	replace	with "new"
	* 3. we've found an	index value	less than the one we're	looking	for, so
	*	 insert	"new" between "cur"	and	"prev"
	*/
	if( cur	== (struct _arrayListElem *)0 )	{
		/*
		 * append	"new" after	"cur"
		 */
		if( arrayHead	== (struct _arrayListElem *)0 )	{
			/*
			 *	special	case: make "new" the arrayHead
			 */
			arrayHead = new;
		} else {
			prev->next	= new;
		}
		return;
	}
	if( index ==	cur->index ) {
		/*
		 * replace the node at "cur" with	"new"
		 */
		if( prev )
			prev->next	= new;
		else {
			/*
			 *	we're replacing	a new head record
			 */
			arrayHead = new;
		}
		new->next	 = cur->next;
		free(	cur	);				/* free-up memory used by cur */
	} else {
		/*
		 * insert	new	between	cur	and	prev
		 */
		if( prev )
			prev->next	= new;
		else {
			/*
			 *	we're inserting	a new head record
			 */
			arrayHead = new;
		}
		new->next	 = cur;
	}
	return;
}

/*
 * ARRAYRETRIEVE
 * retreive	a value	from the array
 * given: integer value	for	the	array index	(0 in first	element), pointer to
 *	 value to be returned
 * return: integer stored in given index.  If the index	cannot be found, program
 *	 can do	one	of two things, depending how interpretor is	compiled
 *	 with ARRAY_UNKNOWN_ELEMENT	set	to -1, throw error (serror(15))
 *	 with ARRAY_UNKNOWN_ELEMENT	set	to 0   return zero
 */

void arrayRetrieve(	int	index, int *result )
{
	struct _arrayListElem *cur;

	for(	cur=arrayHead; cur && cur->index !=	index; cur=cur->next )
		;

	if( cur	== (struct _arrayListElem *)0 )	{
		/*
		 * oops, no array	element	stored for this	index
		 */
#if	ARRAY_UNKNOWN_ELEMENT == -1
		serror(	15 );
#else
		*result	=	0;
#endif
	} else {
		*result	=	cur->value;
	}
	return;
}

/*
 * LOADPROGRAM
 * Load	a basic	program	into memory
 * Given: filename to load
 * Return: void.  Handle errors	in this	function
 */

void loadProgram( char *n )
{
	FILE	*FD;
	char	 buf[BUFSIZ];
	char	*lineNum;
	char	*lineText;
	int	 linecount = 0;
	void	 parseLine();

	/*
	* Try and open the named file
	*/
	if( (FD	= fopen(	n, "r" )) == (FILE *)0 ) {
		PRINTF(	"sorry, cannot locate file \"%s\"\n",	n );
		return;
	}
	while( fgets( buf, BUFSIZ, FD )	!= (char	*)0	) {
		/*
		 * we	expect this	to be a	line of	BASIC code,	starting with a
		 * deminal line-number
		 */
		parseLine( buf,	&lineNum,	&lineText );
		if( lineNum == (char *)0 ||
			lineText == (char	*)0	||
			atoi(	lineNum	) == 0 ) {
			/*
			 *	not	a valid	line of	BASIC
			 */
			continue;
		} else {
			linecount++;
			addtoBuffer( atoi(	lineNum	), lineText, &Work );
		}
	}
	fclose(	FD );

	if( linecount ==	0 ) {
		PRINTF(	"sorry, nothing loaded from file \"%s\"\n", n	);
	} else {
		PRINTF(	"loaded %d lines", linecount );
		/*
		 * as	the	program	has	been successfully loaded, remember the name
		 */
		currFileName = strdup( n );
	}
	/*
	* flag there is	a clean	edit buffer	after the load
	*/
	Work.bufferUnsaved =	0;
	return;
}

/*
 * SAVEPROGRAM
 * Given a filename, save the current edit buffer to the named file
 *	If the name	to save-to is the current filename,	don't check	if the file	exists
 * void	function.  Handle all the errors
 */

void saveProgram( char *n )
{
	struct _progListElem	*P;
	FILE	  *FD;
	int	   mustcheck = 0;
	int	   linecount = 0;
	char	   overwriteMsg[BUFSIZ];

	/*
	* is there a program to	save?
	*/
	if( Work.first == (struct _progListElem	*)0 ) {
		PRINTF(	"sorry, no program to save\n"	);
		return;
	}
	if( currFileName	== (char *)0 )
		mustcheck++;
	else {
		/*
		 * are we	saving to the current filename?
		 */
		if( strcmp( n, currFileName )	!= 0 ) {
			mustcheck++;
		}
	}
	if( mustcheck )	{
		/*
		 * no, so	does the named file	exist?
		 */
		if( access( n, W_OK+F_OK ) ==	0 ) {
			/*
			 *	file exists	and	is writable
			 */
			sprintf( overwriteMsg,	"over-write file \"%s\"?", n );
			if( yesORno( overwriteMsg,	"yes", "no"	) == 2 ) {
				PRINTF(	"save aborted\n" );
				return;
			}
		}
	}
	/*
	* write	the	edit buffer
	*/
	if( (FD	= fopen(	n, "w" )) == (FILE *)0 ) {
		PRINTF(	"sorry, can't save: %s\n", strerror( errno ) );
		return;
	}
	for(	P=Work.first; P; P=P->next,	linecount++	) {
		if( fprintf( FD, "%d %s\n", P->lineNumber, P->lineText ) < 0 ) {
			PRINTF(	"sorry, can't save the program: %s\n",	strerror( errno	) );
			fclose(	FD	);
			return;
		}
	}
	fclose(	FD );
	/*
	* if there was no currFileName or we were given	a different	name,
	* remember this	name
	*/
	if( currFileName	== (char *)0 ||	strcmp(	currFileName, n	) )
		currFileName = strdup( n );

	/*
	* report back to the user
	*/
	PRINTF(	"saved %d lines to \"%s\"\n", linecount,	n );

	/*
	* flag we have saved the edit buffer
	*/
	Work.bufferUnsaved =	0;

	return;
}

/*
 * CLEAREDITBUFFER
 * remove all nodes	in the edit	buffer list, free up all memory	and	reset
 * first node
 * Given: _progList	struct indicating which	list to	clear
 */
void clearEditBuffer( struct _progList *Buff )
{
	struct _progListElem	*P,	*next;

	if( Buff->first	== (struct _progListElem	*)0	)
		return;			/* empty buffer, nothing to	do!	*/

	P=Buff->first;
	while( P	) {
		next = P->next;
		free(	P->lineText	);
		free(	P );
		P	= next;
	}
	Buff->first = (struct _progListElem *)0;
	return;
}


/*
 * PARSELINE
 * Scan	a line presented to	the	editor.	 Extract the first space-separated word
 * and the remainder of	the	line
 * Given: pointer to string	to parse
 * Return: pointer to the first	word, pointer to the remainder
 * Void	function
 */

void parseLine(	char *input, char **first, char	**rem )
{
	if( (*first = strtok( input,	" \t" )) ==	(char *)0 ) {
		/*
		 * empty line
		 */
		*first = (char *)0;
		*rem	 = (char *)0;
		return;
	}
	*rem	= strtok( (char	*)0, "\n" );
	return;
}

/*
 * FPE_HANDLER
 * function	to handle SIGFPE (float	point exceptions)
 */

void FPE_handler( int signum )
{
	serror(	16 );
	return;
}

/*
 * INT_HANDLER
 * function	to handle SIGINT (interrupts)
 */
void INT_handler( int signum )
{
	//puts(	"keyboard interrupt caught and ignored"	);
	userStopRequest++;
	return;
}

/*
 * ARGPARSER
 * Purpose:	given an token template	for	a DRAW command,	parse the remainder	of
 *			the	token-list for the command
 * Given:	token-template string, pointer to error=code,
 *			varargs	list to	receive	decoded	values (see	below)
 * Return:	number of tokens successfully parsed into respective arguments,
 *			-1 parse error.
 *
 * Notes on	the	template string
 * The template	consists of	letters	denoting the expected order	of tokens
 * D = DELIMITER
 * Q = QUOTE (a	Quoted string)
 * N = Number or Variable
 * C = Colourname (returns the equivalient RGB value)
 *
 * If a	value is in	lowercase, it denotes that the token is	optional
 * and that	no error is	to be generated	if the token is	missing.  All subsequent
 * token definitions are then ignored and the input	stream is expected to have
 * EOL or FINISHED.	 See get_token() for more details about	the	tokens
 *
 * For each	token representing a value,	a suitable pointer variable	is expected
 * to be present in	the	va_list.  Strings are copied to	the	heap and a pointer is
 * returned, integers are copied using the supplied	pointer
 *
 * If an error occurs, -1 is returned and the error	variable will hold a value
 * suitable	for	passing	to serror()
 *
 * It is the responsibility	of the caller to free heap-allocated memory
 *
 * Enable debugging	using value	256
 *
 * AFL,	25/07/2012
 */

int	argParser( const char *template, int *error, ... )
{
	va_list	ap;
	const char *want;
	int	  t;
	int	  argcount = 0;
	int	 *i;							/* for returning integer values	*/
	char	 **c;							/* for return char pointer values */
	Uint32 colour, *cp;
	Uint32 colour2Uint();

	va_start( ap, error );				/* initialise the variable arg code	*/

	for(	want=template; *want; want++ ) {
		//printf(	"argParser next arg must be '%c'...\n",	*want );

		t	= get_token();
		//printf(	"next token was %d (token->'%c')\n", t,	*token );

		if( islower( (int)(*want) ) ) {
			/*
			 *	this next token	is optional
			 */
			//printf( "next token is optional.  tok=%d, *token='%c'\n", tok, *token	);
			if( tok	==	EOL	|| tok == FINISHED || *token ==	';'	) {
				*error = 0;
				va_end(	ap );
				return(	argcount );
			}
		}
		switch(	*want	) {
		case 'd':
		case 'D':
			//printf( "found delimiter...('%c')\n",	*token	);
			if( t != DELIMITER	) {
				*error = 0;
				va_end(	ap );
				return(	-1 );
			}
			break;

		case 'n':
		case 'N':
			//printf( "parsing number..." ); fflush( stdout	);
			if( t != NUMBER	&&	t != VARIABLE && *token	!= '(' ) {
				*error = 0;
				va_end(	ap );
				return(	-1 );
			}
			putback();				/* push	the	token back onto	the	stream */
			/*
			 *	we need	the	next variable off the va_list to take this value
			 */
			i = va_arg( ap,	int *);		/* i is	the	address	of where to	store data */
			get_exp( i, 0 );
			//printf( "value=\"%d\"\n", *i	);
			argcount++;
			break;

		case 'q':
		case 'Q':
			//printf( "parsing quoted-string..." );	fflush(	stdout	);
			if( t != QUOTE	) {
				*error = 0;
				va_end(	ap );
				return(	-1 );
			}
			/*
			 *	we need	the	next variable off the va_list to take this value
			 */
			c = va_arg( ap,	char **	);
			*c	= strdup( token	);
			//printf( "value=\"%s\"\n", *c	);
			argcount++;
			break;

		case 'c':
		case 'C':
			//printf( "parsing colourname..." ); fflush( stdout );
			if( t != VARIABLE ) {
				*error = 0;
				va_end(	ap );
				return(	-1 );
			}
			/*
			 *	token points to	the	string which should	be a colourname
			 *	try	and	convert	the	colour name	to a RGB value
			 */
			if( colour2Uint( token,	&colour	) != 0	) {
				*error = 19;
				va_end(	ap );
				return(	-1 );
			}
			/*
			 *	we need	the	next variable off the va_list to return	this colour-value
			 */
			cp	= va_arg( ap, Uint32 * );
			*cp = colour;
			//printf( "colour value=\"%0x\"\n", *cp	);
			argcount++;
			break;

		default	:
			printf(	"oops: internal error!  Invalid DRAW template atom '%c' in template \"%s\"\n",	*want, template	);
		}
		va_end(	ap );
	}
	/*
	* we have exhaused the template, so	next token should be
	* EOL or FINISHED, or a	delimiter or a semi-colon
	*/
	t = get_token();
	//printf( "next token was %d, *token='%c'\n", t,	*token );
	if( tok	== EOL || tok ==	FINISHED ||	t == DELIMITER || *token ==	';'	) {
		*error = 0;
		return(	argcount );
	} else {
		*error = 0;		/* oops	syntax error */
		return(	-1 );
	}
}

/*
 * STREDIT
 * Edit	a string using the keyboard, reading new text from stdin
 * Given: str:	  pointer to string	buffer (which may be empty)
 *		  maxlen: maximum length of	text allowed in	the	string (excluding null)
 *		  flags:  one of STREDIT_HIST_ENABLE, STREDIT_HIST_DISABLE or
 *				  STREDIT_HIST_INIT
 *
 * Supported edit keys:
 * <RETURN>	   : exit edit and return string
 * <control-C> : exit edit and return original string
 * <control-U> : clear string and continue editing
 * <control-A> : move cursor to	line start
 * <control-E> : move cursor to	line end
 * <backspace> : delete	character to left of cursor
 * <up>		   : load edit buffer with older history entry entry
 * <down>	   : load edit buffer with more	recent history entry
 *
 * Return: 0 = normal exit
 *		  -1 = user	terminated edit	with ^C
 *		   1 = string is newline terminated	which makes	editing	impossible
 *		   2 = no TERM env settings	found
 *		   3 = error manipulating the terminal settings
 *		   4 = cannot find term	capability "le"	(cursor	left)
 *		   5 = cannot find term	capability "nd"	(cursor	right)
 *		   6 = cannot find term	capability "kb"	(key-backspace)
 *		   7 = cannot find term	capability "kl"	(key-left)
 *		   8 = cannot find term	capability "kr"	(key-right)
 *		   9 = cannot find term	capability "kd"	(key-left)
 *		   10 =	cannot find	term capability	"ku" (key-right)
 *		   19 =	invalid	flags setting
 *
 */

int	stredit( char *str,	int	maxlen,	int	flags )
{
	int	i;
	int	interrupt =	0;
	int	have_input = 1;
	int	keys = 0;
	int	cursor;
	int	keyup =	0;
	int	keydown	= 0;
	char	readbuf[20];

//	char	*buf;
	char	*cp;
//	char	*term_name;

	char	cap_le[10] = { 0x1d, 0 , 0, 0 };		/* cursor left */
	char	cap_nd[10] = { 0x1c, 0 , 0, 0 };		/* cursor right	*/
	char	cap_kb[10] = { 0x08, 0 , 0, 0 };		/* key-baskspace */
	char	cap_kl[10] = { 0x1b, '[', 'D', '\0'	};	/* left	arrow */
	char	cap_kr[10] = { 0x1b, '[', 'C', '\0'	};	/* right arrow */
	char	cap_kd[10] = { 0x1b, '[', 'B', '\0'	};	/* down	arrow */
	char	cap_ku[10] = { 0x1b, '[', 'A', '\0'	};	/* up arrow	*/

//	struct termios orig_settings, new_settings;

	int	history	= 0;

	void	_delchar(),	_insertchar(), _printstr(),	_clearscrn();
	void	_addToHistory();
	char	*_upHistory(), *_downHistory();

	/*
	* check	flags
	*/
	if( flags !=	STREDIT_HIST_ENABLE	&&
		flags !=	STREDIT_HIST_DISABLE &&
		flags !=	STREDIT_HIST_INIT )
		return(	19 );

	if( flags ==	STREDIT_HIST_INIT ) {
		/*
		 * set up	the	history	buffer
		 */
		for( history=0;	history<HISTORY_SIZE;	history++ )
			historyBuf[history] ==	(char *)0;
		freeHist = 0;
		lastHist = 0;
		return(	0	);
	}
	//printf( "STREDIT 1:\n"	);
	if( strlen( str	) &&	str[ strlen( str )-1 ] == '\n' ) {
		/*
		 * string	is new-line	terminated which makes editing impossible
		 */
		return(	1	);
	}
	/*
	* code operates	very differently if	in HUMBUG flavour
	* in this case we read the keyboard	from SDL
	* and the terminal is the Triton VDU (which	has	no termcap entry)
	*/
	//printf( "STREDIT 2:\n"	);
	if( Flavour == VER_VANILLA || Flavour ==	VER_RASPBERRY )	{
#if	0
		/*
		 * find	our terminal name
		 */
		if( (term_name = getenv( "TERM"	)) ==	(char *)0 )
			return(	2 );

		if( tgetent( (char *)0,	term_name	) == 0 ) {
			printf(	"tgetent failed\n"	);
			return(	3 );
		}
		/*
		 * read	values for le, nd, bs, kl, kr, ku	& kd
		 */
		buf =	cap_le;
		if( (cp	=	tgetstr( "le", &buf	)) == (char	*)0	)
			return(	4 );
		buf =	cap_nd;
		if( (cp	=	tgetstr( "nd", &buf	)) == (char	*)0	)
			return(	5 );
		buf =	cap_kb;
		if( (cp	=	tgetstr( "kb", &buf	)) == (char	*)0	)
			return(	6 );
//#ifndef	__linux
		buf =	cap_kl;
		if( (cp	=	tgetstr( "kl", &buf	)) == (char	*)0	)
			return(	7 );
		buf =	cap_kr;
		if( (cp	=	tgetstr( "kr", &buf	)) == (char	*)0	)
			return(	8 );
		if( flags	== STREDIT_HIST_ENABLE ) {
			buf = cap_ku;
			if( (cp	= tgetstr(	"ku", &buf )) == (char *)0 )
				return(	9 );
			buf = cap_kd;
			if( (cp	= tgetstr(	"kd", &buf )) == (char *)0 )
				return(	10 );
		}
#endif

#if		0
		/*
		 * now put the terminal	into non-Canonical mode	and no echo
		 */
		tcgetattr( 0,	&orig_settings );

		new_settings = orig_settings;

		new_settings.c_lflag &=	~ECHO;			// turn	off	echo
		new_settings.c_lflag &=	~ICANON;		// turn	off	canonical mode
		new_settings.c_cc[VMIN]	=	1;
		new_settings.c_cc[VTIME] = 1;

		tcsetattr( 0,	TCSANOW, &new_settings );
#endif
	} else {
		/*
		 * HUMBUG	Flavour
		 * set values	for	le,	nd,	kb,	kl,	kr,	ku & kd	based on Triton
		 */
		//printf(	"STREDIT 3:\n" );
		sprintf( cap_le, "%c", (unsigned char)8	);
		sprintf( cap_nd, "%c", (unsigned char)9	);
		/*
		 * now set values	for	keys (these	are	SDL	sym	values)
		 * backspace key changes value depending on console/X11	display
		 */
		if( getenv( "DISPLAY"	) == (char *)0 )
			sprintf( cap_kb, "%c",	(unsigned char)0x7f	);	/* back-space */
		else
			sprintf( cap_kb, "%c",	(unsigned char)0x08	);	/* back-space */
		strcpy(	cap_kl,	"<left>" );
		strcpy(	cap_kr,	"<right>"	);
		strcpy(	cap_ku,	"<up>" );
		strcpy(	cap_kd,	"<down>" );
	}
	//printf( "STREDIT 4:\n"	);
	if( strlen( str	) ) {
		/*
		 * print the string and	position the cursor at the end
		 */
		PRINTF(	"%s",	str	);
		fflush(	stdout );
		cursor = strlen( str );
	} else
		cursor = 0;

	//printf( "STREDIT 5:\n"	);
	while( 1	) {				/* while we're in stredit()	...	*/
		have_input = 1;
		while( have_input	) {	/* while we're editing a string	...	*/
			if( Flavour ==	VER_HUMBUG ) {
				//printf( "STREDIT: about to call inch()...\n" );
				if(	(keys =	inch( readbuf, sizeof( readbuf ) ) ) <=	0 ) {
					have_input =	0;
					interrupt++;
					continue;
				}
				if(	readbuf[0] == '\r' ) {
					/*
					* user pressed <CR>.  Translate	this to	\n for
					* UNIX compatibility
					*/
					readbuf[0] =	'\n';
				}
			} else {
				if(	(keys =	read( 0, readbuf, 5	)) <= 0	) {
					have_input =	0;
					interrupt++;
					continue;
				}
				readbuf[keys] =	'\0';
			}
			if( ( readbuf[0]	== '\n'	) 
			  ||( readbuf[0]	== '\r'	) ) {
				/*
				 * user	pressed	<CR>
				 */
				have_input = 0;
			}
			if( flags == STREDIT_HIST_ENABLE ) {
				/*
				 * check for <up> or <down>
				 */
				if(	strcmp(	readbuf, cap_ku	) == 0 ) {
					_clearscrn( str,	cursor,	cap_le );
					keyup++;
					have_input =	0;
					continue;
				}
				if(	strcmp(	readbuf, cap_kd	) == 0 ) {
					_clearscrn( str,	cursor,	cap_le );
					keydown++;
					have_input =	0;
					continue;
				}
			}
			/*
			 *	check for special keys received
			 */
			if( readbuf[0]	== 0x1 ) {
				/*
				 * control-A : position	cursor to line start
				 */
				while( cursor )	{
					cursor--;
					PRINTF(	"%s", cap_le	);
				}
				fflush(	stdout );
			}
			if( readbuf[0]	== 0x5 ) {
				/*
				 * control-E : position	cursor at line end
				 */
				while( str[cursor] != '\0' ) {
					cursor++;
					PRINTF(	"%s", cap_nd	);
				}
				fflush(	stdout );
			}
			if( readbuf[0]	== 0x15	) {
				/*
				 * control-U : clear string
				 */
				_clearscrn(	str, cursor, cap_le	);
				strcpy(	str, ""	);
				cursor = 0;
			}
			//printf( "readbuf = '%s'\n", readbuf );
			if( strcmp( readbuf, cap_kl ) == 0	) {
				/*
				 * key left
				 */
				if(	cursor ) {
					cursor--;
					PRINTF(	"%s", cap_le	);
					fflush(	stdout );
				}
			} else if( strcmp( readbuf, cap_kr )	== 0 ) {
				/*
				 * key right
				 */
				if(	str[cursor]	!= '\0'	) {
					cursor++;
					PRINTF(	"%s", cap_nd	);
					fflush(	stdout );
				}
			} else if( strcmp( readbuf, cap_kb )	== 0 ) {
				/*
				 * key backspace
				 */
				if(	cursor >= 1	) {
					/*
					* remove the character to the left of the cursor
					*/
					cursor--;
					PRINTF(	"%s", cap_le	);
					fflush(	stdout );
					_delchar( str, cursor );
					_printstr( str,	cursor,	1, cap_le );
					PRINTF(	"%s", cap_le	);
					fflush(	stdout );
				}
			} else {
				if(	keys > 1 &&	readbuf[0] == 0x1b ) {
					/*
					* undefined	escape sequence	(ignore)
					*/
					continue;
				}
				if(	readbuf[0] == '<' && readbuf[ strlen( readbuf )-1 ]	== '>' ) {
					/*
					* this is a	SDL	keysym which we're not interested in
					*/
					continue;
				}
				for( i=0; i<keys; i++ )	{
					//printf( "inserting char '%c'\n", readbuf[i] );
					if( isprint( (int)	readbuf[i] ) ) {
						/*
						 * insert	the	character into the buffer 'at' cursor
						 * first check the str can take	another	character
						 */
						if( strlen( str	)	< maxlen ) {
							_insertchar( str, cursor, readbuf[i] );
							_printstr(	str, cursor, 0,	cap_le );
							cursor++;
						}
					}
				}
			}
		}
		/*
		 * user	has indicated	an end to editing this string
		 * did he	choose a history key (<up> or <down>)?
		 */
		if( keyup	) {
			/*
			 *	load the edit buffer with the "previous" history entry
			 */
			cp	= _upHistory();
			if( cp	)  strncpy(	str, cp, maxlen	);
			else	   strcpy( str,	"" );
			/*
			 *	and	display
			 */
			if( strlen( str	) ) {
				/*
				 * print the string	and	position the cursor	at the end
				 */
				PRINTF(	"%s", str );
				fflush(	stdout );
				cursor = strlen( str );
			} else
				cursor = 0;
			keyup =	0;
			/*
			 *	and	resume editing
			 */
			continue;
		} else if( keydown ) {
			/*
			 *	load the edit buffer with the "next" history entry
			 */
			cp	= _downHistory();
			if( cp	) strncpy( str,	cp,	maxlen );
			else	  strcpy( str, "" );
			/*
			 *	and	display
			 */
			if( strlen( str	) ) {
				/*
				 * print the string	and	position the cursor	at the end
				 */
				PRINTF(	"%s", str );
				fflush(	stdout );
				cursor = strlen( str );
			} else
				cursor = 0;
			keydown	= 0;
			/*
			 *	and	resume editing
			 */
			continue;
		} else {
			/*
			 *	some other reason we quit editing, so exit inner loop
			 */
			break;
		}
	}
	/*
	* return terminal to "normal mode" (only in	Vanilla	or Raspberry flavour)
	*/
	if( Flavour == VER_VANILLA || Flavour ==	VER_RASPBERRY )	{
//		tcsetattr( 0, TCSANOW, &orig_settings );
	}

	if( Flavour == VER_HUMBUG ) {
		vdu( 0,	29 );		/* non-clear <CR> */
		vdu( 0,	10 );
	} else
		PRINTF(	"\n" );

	if( interrupt )	return(	-1 );

	if( strlen( str	) ) _addToHistory( str );

	return(	0 );
}

/*
 * ADDTOHISTORY
 * add str to history buffer and adjust	the	pointers
 */

void _addToHistory(	char *s	)
{
	int	i;
	char	*dup;

	if( (dup	= strdup( s	)) == (char	*)0	) {
		PRINTF(	"stredit(): history: out of memory!\n" );
		exit(	2 );
	}
	historyBuf[freeHist]	= dup;
	/*
	* bump the pointers, and adjust	if they	over-flow
	* check	if we need to free a previous save
	*/
	freeHist++;
	freeHist	%= HISTORY_SIZE;
	lastHist	= freeHist;
	/*
	* remove used entries in the buffer
	* we keep 'freeHist' and 'freeHist+1' empty
	*/
	if( historyBuf[freeHist]	) {
		free(	historyBuf[freeHist] );
		historyBuf[freeHist] = (char *)0;
	}
	i = freeHist;
	i++;
	i %=	HISTORY_SIZE;
	if( historyBuf[i] ) {
		free(	historyBuf[i] );
		historyBuf[i]	= (char	*)0;
	}
	//printf( "saved str at %d, lastHist is %d\n", freeHist,	lastHist );
	return;
}

/*
 * UPHISTORY
 * Return an older entry from the history stack, or	<null> if the stack
 * is empty
 */

char *_upHistory( void )
{
//	int	i;

	if( lastHist	== freeHist	) {
		lastHist--;
		if( lastHist < 0 )  lastHist = HISTORY_SIZE-1;
	} else	if(	historyBuf[lastHist] ==	(char *)0 )
		return(	(char	*)0	);
	else {
		lastHist--;
		if( lastHist < 0 )  lastHist = HISTORY_SIZE-1;
	}
	return(	historyBuf[lastHist]	);
}

/*
 * DOWNHISTORY
 * return a	more recent	history	entry, or <null> if	we're at the top of
 * the stack
 */

char *_downHistory(	void )
{
//	int	i;

	if( lastHist	== freeHist	)  return( (char *)0 );
	lastHist++;
	lastHist	%= HISTORY_SIZE;
	return(	historyBuf[lastHist]	);
}

/*
 * clear screen	display	of the edit	string and reset cursor
 * to left-most	position
 */
void _clearscrn( char *s, int pos, char	*cursorleft	)
{
	/*
	* move cursor to the left-end of the string
	*/
	while( pos ) {
		pos--;
		PRINTF(	"%s",	cursorleft );
	}
	/*
	* now print	<spaces> to	delete the string display
	*/
	while( *(s+pos)	) {
		pos++;
		PRINTF(	" " );
	}
	/*
	* and finally move the cursor back the the start
	*/
	while( pos ) {
		pos--;
		PRINTF(	"%s",	cursorleft );
	}
	fflush(	stdout );
	return;
}

/*
 * insert character	at cursor position.	 If	character at cursor, insert	before
 * not sure	if this	is a GCC bug, bug this particular function won't allow
 * it to be	formally defined using _insertchar(	char *s, int pos, char c )
 */
void _insertchar( s, pos, c	)
char *s;
int	 pos;
char c;
{
	char	*y,	*z;

	if( *(s+pos)	) {
		/*
		 * position occupied,	move characters	"up" to	make space
		 */
		z	= s	+ strlen( s	);
		z++;						/* z points	to \0 +	1 */
		y	= s	+ strlen( s	);		/* y points	to \0 */
		while( y >= (s+pos) )
			*z-- =	*y--;
		*(s+pos) = c;
	} else {
		/*
		 * insert	at the end of the line
		 */
		*(s+pos) = c;
		*(s+pos+1) = '\0';
	}
	return;
}

/*
 * delete the character	at "pos" from the given	string
 */
void _delchar( char	*s,	int	pos	)
{
	char	*z;

	s +=	pos;
	z  =	s+1;
	do {
		*s++ = *z++;
	} while(	*s );
	return;
}

/*
 * _PRINTSTR
 * Print a string starting with	the	given position
 * erase flag =	0: simple re-print the string
 * erase flag =	1: string has been reduced in length, need to erase	final position
 */

void _printstr(	char *s, int pos, int erase, char *cap_le )
{
	int i =	0;

	s +=	pos;
	while( *s ) {
		PRINTF(	"%c",	*s );
		s++;
		i++;
	}
	if( erase ) {
		PRINTF(	"%c",	' '	);
		i++;
	}
	for(	i--; i>0; i-- )	{
		PRINTF(	"%s",	cap_le );
	}
	fflush(	stdout );
	return;
}

/*
 * DOEDITCOMMAND
 * Process one of the "direct" edit	commands
 * Given: pointer to the command name, pointer to the remainder	of the line
 * Return: void
 */

void doEditCommand(	char **com,	char *rest )
{
	int	 start;
//	int	 fd;
	int	 rval;
//	int	 progsize =	0;
	int	 i;
	int	 renumberFirst = 0,	renumberStep = 0;
	int	 newnum;
	char	 *stmtNum, *stmtText;
	char	 topicpath[80];
	char	 lsbuf[80];
	char	 editbuf[BUFSIZ];
	char	 *progbuf;
	char	 *cp;
	char	 *comma;
	struct _progListElem	*LP	  =	(struct	_progListElem *)0;
	struct _arrayListElem *cur, *next_array;
	FILE	*FD=NULL;
	int	addRenumber(), lookupRenumber();
	char	*ll2buffer();
	void	loadProgram(), saveProgram();
	void	clearEditBuffer();
	void	resetRenumber();

	if( strcmp( *com, "run"	) ==	0 ) {
		/*
		 * create	a working buffer (in memory) from the linked-list
		 * pass	this to	the run()	function
		 */
		if( (progbuf = ll2buffer()) == (char *)0 ) {
			PRINTF(	"sorry, insufficient memory to run your program\n"	);
			return;
		}
		// clear the array
		cur =	arrayHead;
		while( cur ) {
			next_array	= cur->next;
			free( cur );
			cur = next_array;
		}
		arrayHead	= (struct _arrayListElem *)0;

		// clear the variables
		for( i=0;	i<26; i++ )	 variables[i] =	0;

		// clear the label-table
		for( i=0;	i<NUM_LAB; i++ ) {
			strcpy(	label_table[i].name, ""	);
			label_table[i].p =	(char *)0;
		}

		// reset the stack
		sp = 0;

		/*
		 * now go	call run() ...
		 */
		run( progbuf,	1 );
		free(	progbuf	);
		PRINTF(	"%s\n",	readyString );
		return;
	}
	if( strcmp( *com, "list"	) == 0 ) {
		/*
		 * was an	optional line-number given?
		 */
		if( rest == (char	*)0	)
			start =	0;
		else
			start =	atoi( rest	);

		if( Flavour == VER_HUMBUG	) {
			/*
			 *	enable page	mode
			 */
			PRINTF(	(char *)1 );
		} else {
			/*
			 *	popen "more" to	page the output
			 */
			if( (FD	= popen( "more", "w" ))	==	(FILE *)0 ) {
				printf(	"cannot open more" );
				return;
			}
		}
		/*
		 * list	the contents of	the buffer, starting at	"start"
		 */
		for( LP=Work.first; LP;	LP=LP->next ) {
			if( LP->lineNumber	< start	)  continue;
			if( Flavour ==	VER_HUMBUG ) {
				if(	PRINTF(	"%d %s\n", LP->lineNumber, LP->lineText	) <	0 )
					break;
			} else {
				if(	fprintf( FD, "%d %s\n",	LP->lineNumber,	LP->lineText ) < 0 ) {
					pclose(	FD );
					break;
				}
			}
		}
		if( Flavour == VER_HUMBUG	)
			PRINTF(	(char *)0 );			/* disable page	mode */
		else
			pclose(	FD	);

		return;
	}
	if( strcmp( *com, "edit"	) == 0 ) {
		/*
		 * must	be followed by a line	number
		 */
		if( rest == (char	*)0	) {
			PRINTF(	"edit requires a statement number\n" );
			return;
		}
		start	= atoi(	rest );
		/*
		 * need	to find	the line in the	buffer
		 */
		for( LP=Work.first; LP &&	LP->lineNumber!=start; LP=LP->next )
			;
		if( LP ==	(struct	_progListElem *)0 ) {
			PRINTF(	"sorry, can't find statement number %d\n",	start );
			return;
		}
		/*
		 * prepare the edit buffer
		 */
		snprintf(	editbuf, sizeof( editbuf )-1, "%d %s", start, LP->lineText );
		if( (rval	= stredit( editbuf,	sizeof(	editbuf	)-1, STREDIT_HIST_DISABLE )) !=	0 ) {
			if( rval == -1	) {
				/*
				 * user	pressed	^C
				 */
				PRINTF(	"edit abandoned\n" );
				return;
			} else {
				PRINTF(	"string edit failed (%d)\n", rval );
				exit( 2	);
			}
		}
		parseLine( editbuf, &stmtNum,	&stmtText );
		for( cp=stmtNum; *cp &&	isdigit( (int)(*cp) ); cp++ ) ;
		if( *cp	 ) {
			PRINTF(	"%s %s: unrecognised statement\n",	stmtNum, stmtText );
			return;
		}
		/*
		 * this	is a line	of code	to be added	to the buffer
		 */
		if( addtoBuffer( atoi( stmtNum ),	stmtText, &Work	) != 0 ) {
			PRINTF(	"Sorry, out of memory\n" );
		}
		/*
		 * if	the	edit command changed the line number, we need to remove
		 * the old statement
		 */
		if( strcmp( rest,	stmtNum	) ) {
			addtoBuffer( atoi(	rest ),	(char *)0, &Work );	/* remove */
		}
		return;
	}
	if( strcmp( *com, "save"	) == 0 ) {
		/*
		 * if	there is no	name given,	use	currFileName
		 * (obtained from	a previous load)
		 */
		if( rest == (char	*) 0) {
			if( currFileName == (char *)0 )	{
				PRINTF(	"save requires a file name\n" );
				return;
			} else {
				saveProgram( currFileName );
			}
		} else
			saveProgram( rest );
		return;
	}
	if( strcmp( *com, "load"	) == 0 ) {
		/*
		 * we	expect another argument, which is the filename to load
		 */
		if( rest == (char	*)0	) {
			PRINTF(	"load requires a file name\n" );
			return;
		}
		loadProgram( rest	);
		PRINTF(	"\n" );
		return;
	}
	if( strcmp( *com, "ls" )	== 0 ) {
		/*
		 * show	files	using /bin/ls
		 */
		if( Flavour == VER_HUMBUG	) {
			snprintf( lsbuf, sizeof( lsbuf	)-1,
				  "ls | column -c %d | expand",	HUMBUG_WIN_COLS-3 );
			lsbuf[	sizeof(lsbuf)-1	] =	'\0';
			prun( lsbuf );
		} else
			prun( "ls"	);

		return;
	}
	if( strcmp( *com, "pwd"	) ==	0 ) {
		/*
		 * show	working	directory	name
		 */
		prun(	"pwd" );
		return;
	}
	if( strcmp( *com, "cd" )	== 0 ) {
		/*
		 * we	expect another argument, which is the directory	change to
		 */
		if( rest == (char	*)0	) {
			/*
			 *	use	the	HOME directory from	the	environment
			 */
			if( (rest = getenv( "HOME"	)) == (char	*)0	) {
				PRINTF(	"cd requires a directory name and HOME env variable not set!\n"	);
				return;
			}
		}
		if( chdir( (const	char *)rest	) <	0 )
			PRINTF(	"%s: %m\n", rest );

		return;
	}
	if( strcmp( *com, "new"	) ==	0 ) {
		if( Work.bufferUnsaved ) {
			if( yesORno( "Program has unsaved changes.  Clear anyway",	"yes", "no"	) == 1 ) {
				clearEditBuffer( &Work );
				Work.bufferUnsaved = 0;
			}
			return;
		} else {
			clearEditBuffer( &Work	);
			Work.bufferUnsaved	= 0;

			//	forget the current filename	(freeing any space used	)
			if( currFileName )	 free( currFileName	);
			currFileName =	(char *)0;
			return;
		}
		return;
	}
	if( strcmp( *com, "help"	) == 0 ) {
		/*
		 * did we	get	asked for a	specific topic?
		 */
		if( rest == (char	*)0	) {
			/*
			 *	list the topics	we know	about
			 */
			PRINTF(	"help is available on the following topics:\n"	);
			if( Flavour ==	VER_HUMBUG ) {
				snprintf( lsbuf, sizeof(lsbuf)-1,
					  "ls %s | column -c %d | expand", HELPDIR,	HUMBUG_WIN_COLS-3 );
				lsbuf[sizeof(lsbuf)-1] = '\0';
				strcpy(	topicpath, lsbuf );
			} else {
				strcpy(	topicpath, "ls " );
				strcat(	topicpath, HELPDIR );
			}
			prun( topicpath	);
			return;
		}
		/*
		 * let's see if	the topic	requested can be found
		 * (take care	not	to overflow	the	topicpath string)
		 */
		snprintf(	topicpath, sizeof( topicpath )-1, "%s/%s", HELPDIR,	rest );
		topicpath[ sizeof( topicpath )-1 ] = '\0';
		if( access( topicpath, R_OK )	!= 0 ) {
			PRINTF(	"sorry, no help on topic \"%s\".  Try just \"help\" or \"help help\"\n", rest );
			return;
		}
		/*
		 * fire-up nroff to view the topic
		 * in	humbug mode	don't use more and set LL register to 6i ...
		 */
		if( Flavour == VER_HUMBUG	) {
			snprintf( topicpath, sizeof( topicpath	)-1,
				  "nroff -man -rLL=6i -c %s/%s | cat", HELPDIR,	rest );
			PRINTF(	(char *)1 );	/* enable page mode	*/
		} else
			snprintf( topicpath, sizeof( topicpath	)-1,
				  "nroff -man %s/%s | more", HELPDIR,	rest );

		topicpath[ sizeof( topicpath )-1 ] = '\0';
		prun(	topicpath );
		if( Flavour == VER_HUMBUG	)
			PRINTF(	(char *)0 );	/* quit	page mode */

		return;
	}
	if( strcmp( *com, "flavour" ) ==	0 ) {
		/*
		 * either	show current flavour or	select one
		 */
		/*
		 * did we	get	a parameter?
		 */
		if( rest == (char	*)0	) {
			/*
			 *	show current flavour
			 */
			PRINTF(	"Current flavour is " );
			switch(	Flavour	) {
			case VER_VANILLA:
				PRINTF(	"vanilla\n"	);
				break;
			case VER_RASPBERRY:
				PRINTF(	"raspberry\n" );
				break;
			case VER_HUMBUG:
				PRINTF(	"humbug\n" );
				break;
			}
			return;
		}
		if( Flavour == VER_HUMBUG	) {
			PRINTF(	"Cannot change the flavour while in Humbug.\n"	);
			PRINTF(	"Exit TinyBASIC and re-start\n"	);
			return;
		}
		if( strcasecmp(	rest,	"vanilla" )	== 0 ) {
			Flavour	= VER_VANILLA;
			return;
		} else if( strcasecmp( rest, "raspberry" ) == 0	) {
			Flavour	= VER_RASPBERRY;
			return;
		} else if( strcasecmp( rest, "humbug" )	== 0 ) {
			PRINTF(	"To select \"humbug\" flavour, re-start TinyBASIC with option \"-fh\"\n" );
			return;
		} else {
			PRINTF(	"Available flavours are \"vanilla\" or \"raspberry\"\n"	);
			return;
		}
		return;
	}
	if( strcmp( *com, "trace" ) == 0	) {
		/*
		 * either	show trace mode, or	enable/disable it
		 */
		/*
		 * did we	get	a value	for	trace?
		 */
		if( rest == (char	*)0	) {
			/*
			 *	show trace mode
			 */
			PRINTF(	"Trace is %s\n", Trace?	"on":"off"	);
			return;
		}
		if( strcasecmp(	rest,	"on" ) == 0	) {
			Trace =	1;
			return;
		} else if( strcasecmp( rest, "off" ) ==	0 ) {
			Trace =	0;
			return;
		} else {
			PRINTF(	"Please use \"trace on\" or \"trace off\"\n" );
			return;
		}
		return;
	}
	if( strcmp( *com, "renumber"	) == 0 ) {
		/*
		 * usage:	renumber [first[,last]]
		 *		renumber [undo]
		 * first,	analyse	the	rest of	the	command	for	what to	do
		 */
		if( rest == (char	*)0	) {
			/*
			 *	assume renumber	10,10
			 */
			renumberFirst =	10;
			renumberStep  =	10;
		} else {
			/*
			 *	decode the rest	of the command string
			 */
			if( (comma	= strchr( rest,	','	)) == (char	*)0	) {
				/*
				 * single word
				 */
				if(	strcasecmp(	rest, "undo" ) == 0	) {
					/*
					* undo the renumber	operation
					*/
					if( whichProgList ==	PROGLIST_ORIG )	{
						PRINTF(	"this is the version before the last renumber\n" );
						return;
					}
					/*
					* swap the Work	buffer for the Undo	buffer
					*/
					temp	= Work;
					Work	= Undo;
					Undo	= temp;
					whichProgList =	PROGLIST_ORIG;
					return;
				} else {
					/*
					* this should be the line number step (and first number)
					*/
					renumberFirst =	atoi( rest );
					if( renumberFirst ==	0 ) {
						PRINTF(	"'%s' doesn't look like a valid first line number\n",	rest );
						return;
					}
					renumberStep	= renumberFirst;
				}
			} else {
				/*
				 * at least	two	words
				 */
				*comma++ = '\0';
				renumberFirst =	atoi( rest );
				if(	renumberFirst == 0 ) {
					PRINTF(	"'%s' doesn't look like a valid first line number\n", rest );
					return;
				}
				cp = comma;
				for( ; *comma && isdigit( (int)	(*comma)) ==0; comma++ )
					;
				renumberStep = atoi( comma );
				if(	renumberStep ==	0 ) {
					PRINTF(	"'%s' doesn't look like a valid step number\n",	cp );
					return;
				}
			}
		}
		//printf(	"renumber First=%d, Step=%d\n",	renumberFirst, renumberStep	);
		resetRenumber();			/* clear the renumber lookup table */
		/*
		 * if	there's	an Undo	list, free it before we	continue
		 */
		clearEditBuffer( &Undo );

		/*
		 * build a look-up list	from the current program to the	new numbers
		 */
		for( LP=Work.first, newnum=renumberFirst;
			LP;
			LP=LP->next,	newnum+=renumberStep ) {
			addRenumber( LP->lineNumber, newnum );
		}
		/*
		 * now create	a new linked-list from the existing	program	buffer
		 * substituting	old line numbers for the equivalent new	ones
		 */
		for( LP=Work.first; LP;	LP=LP->next ) {
			newnum	= lookupRenumber( LP->lineNumber );
			if( newnum	< 0	) {
				PRINTF(	"cannot renumber: something odd about statement %d\n", LP->lineNumber );
				return;
			}
			//printf( "adding %d %s...\n",	newnum,	LP->lineText );
			/*
			 *	need to	scan this line and find	GOTO or	GOSUBs and change the
			 *	line numbers they refer	to the new numbers
			 */
			//printf( "renumber: '%s' ", LP->lineText );
			fflush(	stdout	);
			switch(	(i	= renumber(	LP->lineText, editbuf, sizeof( editbuf ) ))	) {
			case -2:
				PRINTF(	"can't find a statement number for GOTO/GOSUB at %d\n",	LP->lineNumber );
				break;
			case -1:
				PRINTF(	"no space to renumber line %d\n", LP->lineNumber );
				break;
			case 0:
				break;
			default	:
				PRINTF(	"warning: possible computed GOTO or GOSUB at line %d\n", newnum	);
				break;
			}
			if( i <	0 )
				return;

			//printf( "-> '%s'\n",	editbuf	);

			addtoBuffer( newnum, editbuf, &Undo );
		}
		/*
		 * now swap the	Work buffer for	the Undo one
		 * "renumber undo" just	swaps	the	around
		 * record	the	fact the we've switched	to the RENUM list
		 */
		temp = Work;
		Work = Undo;
		Undo = temp;
		whichProgList	= PROGLIST_RENUM;
		return;
	}
	if( strcmp( *com, "quit"	) == 0 || strcmp( *com,	"exit" ) ==	0 ) {
		if( Work.bufferUnsaved ) {
			if( yesORno( "Program has unsaved changes.  Quit anyway?",	"yes", "no"	) == 1 ) {
				if(	Flavour	== VER_HUMBUG )	{
					/* shutdown SDL	*/
					SDL_Quit();
				}
				exit( 0	);
			} else
				return;
		}
		if( Flavour == VER_HUMBUG	) {
			/*	shutdown SDL */
			SDL_Quit();
		}
		exit(	0 );
	}
	return;
}

/*
 * RESETRENUMBER
 * reset the linked-list used to hold the renumber lookup-table
 */
void resetRenumber()
{
	struct _renumElem *RP, *next;

	if( renumberTableHead )	{
		/*
		 * iterate over	the list freeing memory	used
		 */
		RP= renumberTableHead;
		while( RP	) {
			next =	RP->next;
			free( RP );
			RP	= next;
		}
		renumberTableHead	= (struct _renumElem *)0;
	}
	return;
}

/*
 * ADDRENUMBER
 * add an entry	to the renumber	table
 * Given: old line number, equivalent new line number
 * Return: 0  =	OK
 *		   -1 =	already	have value for 'old'
 */
int	addRenumber( int old, int new )
{
	struct _renumElem *RP, *newnode;

	if( renumberTableHead )	{
		/*
		 * scan	the table	and	check we haven't got an	entry for 'old'
		 */
		for( RP=renumberTableHead; RP->next; RP=RP->next ) {
			if( RP->old ==	old	)
				return(	-1 );
		}
		/*
		 * RP	now	points to the end node.	 Check this	one	doesn't	contain	the
		 * 'old' line	number
		 */
		if( RP->old == old )
			return(	-1	);

		/*
		 * add this to the end of	the	list
		 */
		if( (newnode = malloc( sizeof( struct	_renumElem ) ))	== (struct _renumElem *)0 ) {
			printf(	"TinyBASIC: out of memory!\n" );
			exit( 1	);
		}
		RP->next = newnode;
		newnode->old = old;
		newnode->new = new;
		newnode->next	= (struct _renumElem *)0;
		return(	0	);
	}
	/*
	* this is the first	record
	*/
	if( (newnode	= malloc( sizeof( struct _renumElem	) )) ==	(struct	_renumElem *)0 ) {
		PRINTF(	"TinyBASIC: out of memory!\n"	);
		exit(	1 );
	}
	newnode->old	= old;
	newnode->new	= new;
	newnode->next =	(struct	_renumElem *)0;
	renumberTableHead = newnode;
	return(	0 );
}

/*
 * LOOKUPRENUMBER
 * return the equivalent new number	for	an old line	number
 * return -1 if	the	old	number cannot be found
 */

int	lookupRenumber(	int	old	)
{
	struct _renumElem *RP;

	if( renumberTableHead )	{
		/*
		 * scan	the table	and	check we haven't got an	entry for 'old'
		 */
		for( RP=renumberTableHead; RP; RP=RP->next ) {
			if( RP->old ==	old	)
				return(	RP->new	);
		}
		return(	-1 );
	}
	return(	-1 );
}

/*
 * RENUMBER
 * Given: existing statement text, buffer for new statement, buffer	max	length
 * Return:	0 =	statement converted
 *		   -1 =	error (such	as no space	in new buffer for converted	line)
 *		   -2 =	error (can't convert line number)
 *			1 =	computed GOTO or GOSUB found
 */

int	renumber( char *orig, char *new, int maxlen	)
{
	char	*op, *np;
	char	*c;
	char	*linenum_begin;
	char	stmtnum[20];
	int	newnum;
	int	computedGoto = 0;

	op =	orig;
	np =	new;
	while( *op ) {
		/*
		 * skip	white	space
		 */
		for( ;
			*op && (*op == '	' || *op ==	'\t') && maxlen;
			*np++ =	*op++, maxlen--	)
			;
		if( maxlen ==	0 )
			return(	-1	);

		if( *op	== '\0'	) {
			break;
		}
		/*
		 * should	now	point to a command
		 */
		if( strncasecmp( op, "rem", 3	) == 0 ) {
			/*
			 *	skip to	the	end	of the line
			 */
			while(	*op	&& maxlen ) {
				*np++ =	*op++;
				maxlen--;
			}
			if( maxlen	== 0 )
				return(	-1 );
			break;
		} else {
			/*
			 *	scan forward skipping spaces
			 *	if we find ';',	go back	to the top of the loop
			 *	else try to	match for GOTO or GOSUB
			 */
			while(	*op	&& *op != ';' )	{
				if(	strncasecmp( op, "goto", 4 ) ==	0 ||
					strncasecmp( op, "gosub", 5	) == 0 ) {
					//printf( "renumber: GOTO or GOSUB!\n" );
					/*
					* convert these	commands
					* first, transfer the command to the new buffer	and
					* look for the line	number
					*/
					while( *op != '	' &&	*op	!= '\t'	&& maxlen ) {
						*np++	= *op++;
						maxlen--;
					}
					if( maxlen == 0	)
						return(	-1 );

					/*
					* skip over	white space	to find	the	line number
					*/
					while( (*op == '	' || *op ==	'\t') && maxlen	) {
						*np++	= *op++;
						maxlen--;
					}
					if( maxlen == 0	)
						return(	-1 );

					/*
					* the next token should	be all digits ending in
					* a	space, semicolon or	null.  If we find anything else
					* then treat it	as an expression, which	means it is	a
					* computed GOTO/GOSUB, which we	can't convert
					*/
					c = stmtnum;
					linenum_begin =	op;
					while( isdigit(	*op ) )
						*c++ = *op++;

					*c =	'\0';
					/*
					* what ended the statement number?
					*/
					if( *op	!= '	' && *op !=	'\t' &&	*op	!= ';' && *op != '\0' )	{
						/*
						 * flag	we've	found a	computed GOTO or GOSUB
						 * then	copy the offending expression	to the new buffer
						 * and continue	converting as	there may be more
						 * GOSUBS	or GOTOS on	this line
						 */
						computedGoto++;
						do {
							*np++ =	*linenum_begin++;
						}	while( linenum_begin <=	op );
						op++;
					} else {
						/*
						 * can we	find a new line	number of this statement number?
						 */
						if( (newnum =	lookupRenumber(	atoi( stmtnum )	)) < 0 ) {
							/*
							 *	oops, no
							 */
							return(	-2	);
						}
						/*
						 * convert the new statement number to ASCII
						 */
						snprintf(	stmtnum, sizeof( stmtnum ),	"%d", newnum );
						/*
						 * can we	add	the	new	statement number to	the	new	buffer?
						 */
						if( (maxlen -	strlen(	stmtnum	)) == 0	)
							return(	-1	);

						/*
						 * add the new statement number	to the new buffer
						 */
						c	= stmtnum;
						while( *c	) {
							*np++ =	*c++;
							maxlen	--;
						}
						/*
						 * and carry on...
						 */
					}
				} else {
					*np++ =	*op++;
					maxlen--;
					if( maxlen == 0	)
						return(	-1 );
				}
			}
		}
		if( *op	== '\0'	)
			break;

		/*
		 * if	we found a semicolon, copy to the new buffer and
		 * continue...
		 */
		if( *op	== ';' ) {
			if( maxlen	) {
				*np++ =	*op++;
				maxlen--;
			}
		}
		if( maxlen ==	0 )
			return(	-1	);

	}
	*np = '\0';
	return(	computedGoto? 1	: 0 );
}

/*
 * LOADFONT: import	a font file
 * Given: font file	name (this function	builds a font path)
 *		  buffer with size for error messages to be	returned
 * Return:	0 =	font loaded
 *		   -1 =	error (buffer holds	diagnostic)
 */
int	loadFont( char *fname, char	*ebuff,	int	esize )
{
	int	i, j;
	int	linenum;
	int	glyphnum;
	char	fontpath[BUFSIZ];
	char	buff[BUFSIZ];
	unsigned	int	rowdata;
	int	rowbyte;
	int	inglyph;
	int	glyphcount = 0;
	int	bytes;
	int	row=0;
	char	*space,	*equal;
	FILE	*FD;

	/*
	* build	the	font file pathname
	*/
	snprintf( fontpath, sizeof( fontpath	), "%s/%s",	FONTDIR, fname );
	/*
	* try to open the font file
	*/
	if( (FD	= fopen(	fontpath, "r" )) ==	(FILE *)0 ) {
		snprintf(	ebuff, esize-1,	"can't load font file %s", fontpath	);
		ebuff[ esize-1 ] = '\0';
		return(	-1 );
	}
	linenum	= 0;
	inglyph	= 0;
	GlyphWidth =	GlyphHeight	= 0;
	strcpy(	FontName, ""	);
	glyphcount =	0;
	while( fgets( buff, sizeof( buff	), FD )	!= (char *)0 ) {
		linenum++;

		if( buff[	strlen(buff)-1 ] ==	'\n' )
			buff[ strlen(buff)-1 ]	= '\0';

		/*
		 * ignore	blank lines	and	comments
		 */
		if( strlen( buff ) ==	0 || buff[0] ==	'#'	)
			continue;

		/*
		 * is	the	line the start of a	glyph definition?
		 */
		if( strncmp( buff, "[glyph", 6 ) == 0	) {
			/*
			 *	are	we already processing a	glyph?
			 */
			if( inglyph ) {
				if(	row	!= GlyphHeight ) {
					snprintf( ebuff,	esize-1, "not enough row data for glyph %02X at line %d", glyphnum,	linenum	);
					ebuff[esize-1] =	'\0';
					return(	-1 );
				}
				glyphcount++;
			} else {
				/*
				 * do we have values for GlyphWidth, Height	and	Count?
				 */
				if(	GlyphWidth	== 0 ||
					GlyphHeight	== 0 ||
					GlyphCount	== 0 ) {
					snprintf( ebuff,	esize-1, "cannot import glyph data (line %d): no glyph sizes defined", linenum );
					ebuff[ esize-1 ]	= '\0';
					return(	-1 );
				}
				/*
				 * malloc a	buffer for the font	data
				 */
				if(	(FontBase =	(unsigned char *)malloc( GlyphCount	* GlyphWidth/8 * GlyphHeight ))	== (unsigned char *)0 )	{
					strncpy(	ebuff, "cannot malloc space for font data",	esize-1	);
					ebuff[ esize-1 ]	= '\0';
					return(	-1 );
				}
				/*
				 * initialise the new font memory
				 */
				for( i=0; i<(GlyphCount*GlyphWidth/8*GlyphHeight); i++ )
					*(FontBase +	i) = 0x00;

			}
			/*
			 *	process	glyph definition: get the hex value	of the glyph
			 */
			if( (space	= strchr( buff,	' '	)) == (char	*)0	) {
				snprintf( ebuff, esize-1, "bad glyph definition at line %d", linenum );
				ebuff[ esize-1 ] = '\0';
				return(	-1 );
			}
			if( sscanf( ++space, "%x",	&glyphnum )	!= 1 ) {
				snprintf( ebuff, esize-1, "bad glyph definition at line %d (missing hex value?)", linenum );
				ebuff[ esize-1 ] = '\0';
				return(	-1 );
			}
			//printf( "importing glyph %02X from font %s\n", glyphnum,	FontName );
			inglyph++;
			row = 0;
		} else if( strncmp(	buff, "FontName=", 9 ) == 0	) {
			/*
			 *	extract	the	font name
			 */
			equal =	strchr(	buff, '=' );
			strcpy(	FontName, ++equal );
		} else if( strncmp(	buff, "GlyphWidth=", 11	) == 0 ) {
			/*
			 *	extract	the	Glyph width	(pixels)
			 */
			equal =	strchr(	buff, '=' );
			GlyphWidth	= atoi(	++equal	);
		} else if( strncmp(	buff, "GlyphHeight=", 12 ) == 0	) {
			/*
			 *	extract	the	Glyph height (pixels)
			 */
			equal =	strchr(	buff, '=' );
			GlyphHeight = atoi( ++equal );
		} else if( strncmp(	buff, "GlyphCount=", 11	) == 0 ) {
			/*
			 *	extract	the	Glyph count
			 */
			equal =	strchr(	buff, '=' );
			GlyphCount	= atoi(	++equal	);
		} else if( inglyph ) {
			/*
			 *	this is	row	data for the current glyph
			 */
			if( sscanf( buff, "%x",	&rowdata )	!= 1 ) {
				snprintf( ebuff, esize-1, "bad row data at line %d: no hex value found", linenum );
				ebuff[ esize-1 ] = '\0';
				return(	-1 );
			}
			/*
			 *	we need	to break row data up into bytes, then store	them in
			 *	the	font array
			 */
			bytes =	GlyphWidth	/ 8;
			if( GlyphWidth	% 8	!= 0 )	bytes++;
			//printf( "GlyphHeight=%d, bytes=%d rowdata=%04x ", GlyphHeight, bytes,	rowdata	);
			for( i=bytes-1,	j=0; i>=0;	i--, j++ ) {
				rowbyte	= rowdata &	(0xff<<i*8);
				rowbyte	>>=	(i*8);
				//printf( "i=%d rowbyte=%02X fontoffset=%d ", i, rowbyte, glyphnum * bytes * GlyphHeight + j +(row*bytes) );
				*(FontBase + (glyphnum * bytes * GlyphHeight + j + (row*bytes))) = (unsigned char)rowbyte;
			}
			//printf( "\n"	);
			row++;
		} else {
			snprintf( ebuff, esize-1, "unrecognised font definition statement at line %d",	linenum	);
			ebuff[	esize-1	] =	'\0';
			return(	-1	);
		}
	}
	fclose(	FD );
	/*
	* are we already processing	a glyph?
	*/
	if( inglyph ) {
		if( row	!= GlyphHeight ) {
			snprintf( ebuff, esize-1, "not enough row data for glyph %02X at line %d",	glyphnum, linenum );
			ebuff[	esize-1	] =	'\0';
			return(	-1	);
		}
		glyphcount++;
	}
	snprintf( ebuff,	esize-1, "imported %d glyphs from font \"%s\"",	glyphcount,	FontName );
	ebuff[ esize-1 ]	= '\0';

	return(	0 );
}
/*
 * VDU
 * Basic Emulator for Thompson SFC96364
 * Given: position (0..1024) and character to display
 * Implements the following	features:
 * When	position = 0, "prints" the character at	the	cursor position
 *	 advancing the cursor by one space
 * else, poke the character	directly to	the	screen at (position-1)
 * The following ASCII codes are special control codes:
 * 08 (backspace) :	move cursor	one	space left (wrapping round screen)
 * 09 (forward)	  :	move cursor	one	space right	(wrapping round	screen)
 * 0A (line	feed) :	scroll text	up one line, clearing bottom line
 * 0B (up)		  :	move cursor	up one line, wrapping round	screen)
 * 0C (clear)	  :	clear screen and reset home	position)
 * 0D (return)	  :	move cursor	to line	start, clear to	end-of-line
 * 1B (esc)		  :	move cursor	down wrapping round	screen)
 * 1C (home)	  :	reset screen to	home position (no clear)
 * 1D ()		  :	move cursor	to line	start, no line clear
 */
#if		0
void vdu( int position,	unsigned char chr )
{
	int	i, col,	clearrow;
	struct _sfc temp;			/* apply changes to	here */
	void	mapvmem();
	Uint32 showCursor();

	if( chr	> 0x7f )
		return;					/* code	out	of range, ignore */

	temp	= Vmem;					/* take	a copy of working VDU */
	if( position	== 0 ) {
		/*
		 * print mode
		 * check for special control codes
		 */
		switch(	chr ) {
		case 8:
			/*
			 * back-space cursor
			 */
			if(	temp.cursor_pos	% temp.cols	== 0 ) {
				/*
				* we're	on column 0, we	will step down one row
				*/
				temp.cursor_row++;
				/*
				 * cursor	moves to bottom	row
				 */
				temp.cursor_row	=	(temp.rows - 1);
			}
			temp.cursor_pos--;
			/* keep	cursor on the display */
			temp.cursor_pos	%= (temp.rows *	temp.cols);
			/*
			 * update Vmem...
			 */
			Vmem = temp;
			showCursor(	0, (void *)1 );
			break;

		case 9:
			/*
			 * forward-space cursor
			 */
			if(	temp.cursor_pos	% temp.cols	== (temp.cols -	1) ) {
				/*
				* we're	on the last	column,	we will	step up	one	row
				*/
				temp.cursor_row++;
				if( temp.cursor_row == temp.rows	)
					/*
					 * cursor	moves to top row
					 */
					temp.cursor_row	=	0;
			}
			temp.cursor_pos++;
			/* keep	cursor on the display */
			temp.cursor_pos	%= (temp.rows *	temp.cols);
			/*
			 * update Vmem...
			 */
			Vmem = temp;
			showCursor(	0, (void *)1 );
			break;

		case 0x0a:
			/*
			 *	move cursor	down one line, scrolling (clearing)	the	bottom line
			 *
			 *	update the cursor pointer, ensuring	it stays inside	the	vdu	mem
			 */
			temp.cursor_pos	+=	temp.cols;
			temp.cursor_pos	%=	(temp.rows * temp.cols);
			/*
			 *	update the cursor screen row
			 */
			temp.cursor_row++;
			//printf( "cursor row=%d\n", temp.cursor_row );
			/*
			 *	do we need to scroll the screen?
			 */
			if( temp.cursor_row ==	temp.rows ) {
				//printf( "0x0a: need to scroll...\n" );
				/*
				 * scrolling needs to occur
				 *
				 * indicate	cursor remains on the bottom line
				 */
				temp.cursor_row	= (temp.rows -1);
				/*
				 * top-left	position now moves "up"	by a row
				 */
				temp.topleft +=	temp.cols;
				/*
				 * ensure temp.topleft remains inside the screen buffer
				 */
				temp.topleft %=	(temp.rows * temp.cols);
				/*
				 * the row to clear	starts at (n-1)	rows "down"	from topleft
				 */
				clearrow = temp.topleft	+ (temp.rows-1)*temp.cols;
				clearrow %=	(temp.rows * temp.cols);
				/*
				 * now clear the bottom	line of	the	screen
				 */
				for( i=0; i<temp.cols; i++ )
					temp.vduram[clearrow+i]	= 0x20;
			}
			/*
			 *	now	update Vmem
			 */
			//printf( "temp topleft=%d, Vmem topleft=%d\n",	temp.topleft, Vmem.topleft	);
			mapvmem( temp,	Vmem );
			Vmem =	temp;
			showCursor( 0,	(void *)1 );
			break;

		case 0x0b:
			/*
			 *	move cursor	up one line, wrapping round
			 *
			 *	update the cursor pointer, ensuring	it stays inside	the	vdu	mem
			 */
			temp.cursor_pos	-=	temp.cols;
			temp.cursor_pos	%=	(temp.rows * temp.cols);
			/*
			 *	update the cursor screen row
			 */
			temp.cursor_row--;
			if( temp.cursor_row < 0	) {
				temp.cursor_row	= temp.rows-1;
			}
			/*
			 *	now	update Vmem
			 */
			Vmem =	temp;
			showCursor( 0,	(void *)1 );
			break;

		case 0x0c:
			/*
			 *	clear screen and home cursor
			 */
			for( i=0; i<(temp.rows*temp.cols);	i++	)
				temp.vduram[i] = 0x20;

			temp.cursor_pos	= 0;
			temp.cursor_row	= 0;
			temp.topleft	 = 0;
			/*
			 *	use	SDL	to clear the screen
			 */
			if( SDL_mutexP(	Vmem.scn_lock )	< 0 ) {
				printf(	"can't get mutex lock: %s\n", SDL_GetError() );
			}
			SDL_FillRect(Screen, NULL,	SDL_MapRGB(Screen->format, 0, 0, 0));
			if( SDL_mutexV(	Vmem.scn_lock )	< 0 ) {
				printf(	"can't release mutex lock: %s\n", SDL_GetError() );
			}
			/*
			 *	and	update the working VDU details
			 */
			Vmem =	temp;
			showCursor( 0,	(void *)1 );
			break;

		case 0x0d:
			/*
			 *	carriage return: clear to end of line, move	cursor to col 0
			 */
			col = temp.cursor_pos %	temp.cols;
			for( i=temp.cursor_pos;	i<temp.cursor_pos+temp.cols-col; i++ )
				temp.vduram[i] = 0x20;

			temp.cursor_pos	-=	(temp.cursor_pos % temp.cols);
			mapvmem( temp,	Vmem );
			Vmem =	temp;
			showCursor( 0,	(void *)1 );
			break;

		case 0x1b:
			/*
			 *	move cursor	down one line, wrapping	round (no scroll)
			 *
			 *	update the cursor pointer, ensuring	it stays inside	the	vdu	mem
			 */
			temp.cursor_pos	+=	temp.cols;
			temp.cursor_pos	%=	(temp.rows * temp.cols);
			/*
			 *	update the cursor screen row
			 */
			temp.cursor_row++;
			if( temp.cursor_row ==	temp.rows ) {
				temp.cursor_row	= 0;
			}
			/*
			 *	now	update Vmem
			 */
			Vmem =	temp;
			showCursor( 0,	(void *)1 );
			break;

		case 0x1c:
			/*
			 *	home the cursor.
			 *	move cursor	to topleft and return Vmem #0 to topleft position
			 */
			temp.cursor_pos	= 0;
			temp.cursor_row	= 0;
			temp.topleft	 = 0;
			mapvmem( temp,	Vmem );
			Vmem =	temp;
			showCursor( 0,	(void *)1 );
			break;

		case 0x1d:
			/*
			 *	return cursor to column	0, no clear	to end of line
			 */
			col = temp.cursor_pos %	temp.cols;
			temp.cursor_pos	-=	(temp.cursor_pos % temp.cols);
			mapvmem( temp,	Vmem );
			Vmem =	temp;
			showCursor( 0,	(void *)1 );
			break;


		default:
			/*
			 *	"print"	character at cursor	position, advance cursor
			 *	convert	chr	to uppercase, if it	is lowercase
			 */
			if( isalpha( chr )	&& islower(	chr	) )	 chr = toupper(	chr	);
			//printf( "vdu: chr %02x@%d...\n",	chr, temp.cursor_pos );
			temp.vduram[temp.cursor_pos] =	chr;
			temp.cursor_pos++;
			/*
			 *	keep cursor_pos	within the VDU memory buffer
			 */
			temp.cursor_pos	%=	(temp.rows * temp.cols);
			if( temp.cursor_pos % temp.cols	==	0 ) {
				/*
				 * cursor has stepped to the line start
				 */
				temp.cursor_row++;
				//printf( "cursor now on row %d...", temp.cursor_row );
				if(	temp.cursor_row	== temp.rows ) {
					/*
					* scrolling	needs to occur
					*
					* indicate cursor remains on the bottom	line
					*/
					//printf( "scrolling..."	);
					temp.cursor_row	= (temp.rows	-1);
					/*
					* top-left position	now	moves "up" by a	row
					*/
					temp.topleft	+= temp.cols;
					/*
					* ensure temp.topleft remains inside the screen	buffer
					*/
					temp.topleft	%= (temp.rows *	temp.cols);
					//printf( "topleft now %d, ", temp.topleft );
					/*
					* the row to clear starts at (n-1) rows	"down" from	topleft
					*/
					clearrow	= temp.topleft + (temp.rows-1)*temp.cols;
					clearrow	%= (temp.rows *	temp.cols);
					//printf( "clearing buf from %d...\n", clearrow	);
					/*
					* now clear	the	bottom line	of the screen
					*/
					for(	i=0; i<temp.cols; i++ )
						temp.vduram[clearrow+i]	=	0x20;
					//printf( "temp topleft=%d, Vmem topleft=%d\n",	temp.topleft, Vmem.topleft );
				}
			}
			mapvmem( temp,	Vmem );
			Vmem =	temp;
			showCursor( 0,	(void *)1 );
			break;
		}
	} else {
		/*
		 * memory-poke mode
		 */
		position--;
		/*
		 * ensure	position is	"within range"
		 */
		position %= (temp.rows * temp.cols);
		temp.vduram[position]	= chr;
		mapvmem( temp, Vmem );
		Vmem = temp;
	}
	return;
}
/*
 * MAPVMEM
 * Given: new VDU RAM and current RAM
 * Figure-out the changes and update the SDL screen
 */

void mapvmem( struct _sfc new, struct _sfc current )
{
	int c, n, i;
	int row,	col;
	int x, y;

	if( SDL_mutexP(	Vmem.scn_lock )	< 0 ) {
		printf(	"can't get mutex lock: %s\n",	SDL_GetError() );
		PRINTF(	"can't get mutex lock: %s\n",	SDL_GetError() );
	}
	/*
	* check	topleft	values
	* if new is	different from current,	whole screen must be re-drawn
	*/
	if( new.topleft	!= current.topleft ) {
		/*
		 * redraw	whole screen
		 * first,	clear the screen
		 */
		SDL_FillRect(Screen, NULL, SDL_MapRGB(Screen->format,	0, 0, 0));
		/*
		 * and re-drawn	new buffer
		 */
		for( n=new.topleft, i=0;
			i<(new.rows*new.cols);
			n++,	i++, n%=(new.rows*new.cols)	) {
			if( new.vduram[n] != 0x20 ) {
				row	= i	/ new.cols;
				col	= i	% new.cols;
				//printf( "scroll: i=%d new.cols=%d, mapping %02x at (%d,%d)\n", i,	new.cols, new.vduram[n], row , col );
				x =	col	* GlyphWidth;
				y =	row	* GlyphHeight;
				characterColor(	Screen,	x, y, new.vduram[n], HUMBUG_PHOSPHOR_COLOUR	);
			}
		}
		SDL_Flip(	Screen );
		if( SDL_mutexV(	Vmem.scn_lock	) <	0 ) {
			PRINTF(	"can't release mutex lock: %d\n", SDL_GetError() );
		}
		return;
	}
	/*
	* screens have same	topleft	point
	* do a byte-by-byte	comparison to see what needs changing
	*/
	for(	c=current.topleft, n=new.topleft, i=0;
		i<(current.rows*current.cols);
		c++, n++, i++, c%=(current.rows*current.cols), n%=(new.rows*new.cols) )	{
		if( new.vduram[n]	!= current.vduram[c] ) {
			row = (i%1024)/new.cols;
			col = (i%1024)%new.cols;
			x = col	* GlyphWidth;
			y = row	* GlyphHeight;
			/*
			 *	found a	difference
			 *	is the current screen character	blank?
			 */
			if( current.vduram[c] != 0x20 )	{
				/*
				 * must	clear the current screen position first
				 */
				boxColor( Screen, x, y,	(x+GlyphWidth),	(y+GlyphHeight), 255 );
			}
			if( new.vduram[n] != 0x20 ) {
				/*
				 * and now map the new glyph...
				 */
				characterColor(	Screen,	x, y, new.vduram[n], HUMBUG_PHOSPHOR_COLOUR	);
			}
		}
	}
	SDL_Flip( Screen	);
	if( SDL_mutexV(	Vmem.scn_lock )	< 0 ) {
		PRINTF(	"can't release mutex lock: %d\n",	SDL_GetError() );
	}
}

/*
 * SHOWCURSOR
 * Called when a cursor	is to be displayed
 * This	function arranges to be	called regularly by	SDL_AddTimer()
 * to implement	a flashing cursor
 * The function	uses the follwing definitions (see top of program)
 *
 * #define HUMBUG_CURSOR_BLINK_RATE
 * #define HUMBUG_CURSOR_ROW_BEGIN
 * #define HUMBUG_CURSOR_ROW_END
 * #define HUMBUG_CURSOR_COL_START
 * #define HUMBUG_CURSOR_COL_END
 *
 * Given: rate (delay) in milliseconds
 *		  update flag: 0 = normal blink	update
 *					   1 = force immediate cursor update
 * Force update	blanks the cursor (if lit) and immediately redraws it
 */

Uint32 showCursor( Uint32 rate,	void *force_upd	)
{
	int cur_row,	cur_col;
	static int x1, y1, x2, y2;
	static int state	= 0;
	/*
	* if cursor	is off,	disable	further	calls
	*
	* in the final version the code	needs to ensure	the	cursor is "off"
	* before returning...
	*/
	if( Vmem.showcursor == 0	)  return( 0 );

	if( SDL_mutexP(	Vmem.scn_lock )	< 0 ) {
		printf(	"showcursor: mutex lock error: %s\n",	SDL_GetError() );
		return;
	}
	if( force_upd )	{
		if( state	== 1 ) {
			/*	blank cursor, then light it	at current cursor position */
			boxColor( Screen, x1, y1, x2, y2, 255 );
		}
		cur_row	=	Vmem.cursor_row;
		cur_col	=	Vmem.cursor_pos	% Vmem.cols;
		/*
		 * work-out rectangle	for	the	cursor (x1,	y1)	and	(x2, y2)
		 */
		y1 = cur_row*GlyphHeight+HUMBUG_CURSOR_ROW_START;
		x1 = cur_col*GlyphWidth+HUMBUG_CURSOR_COL_START;
		x2 = x1	+	(HUMBUG_CURSOR_COL_END - HUMBUG_CURSOR_COL_START);
		y2 = y1	+	(HUMBUG_CURSOR_ROW_END - HUMBUG_CURSOR_ROW_START);
		/*
		 * draw	the cursor
		 */
		boxColor(	Screen,	x1,	y1,	x2,	y2,	HUMBUG_PHOSPHOR_COLOUR );

		state	= 1;
		SDL_Flip(	Screen );
		if( SDL_mutexV(	Vmem.scn_lock	) <	0 ) {
			printf(	"showcursor: mutex unlock error: %s\n",	SDL_GetError()	);
		}
		return;
	}
	/*
	* need to toggle cursor
	* is it	currently on or	off?
	*/
	if( state ==	0 ) {
		state	= 1;
		/*
		 * draw	a	filled rectagle	for	the	cursor in the current character	cell
		 */
		cur_row	=	Vmem.cursor_row;
		cur_col	=	Vmem.cursor_pos	% Vmem.cols;
		//printf(	"set cursor at cell (%d,%d)...", cur_row, cur_col );
		/*
		 * work-out rectangle	for	the	cursor (x1,	y1)	and	(x2, y2)
		 */
		y1 = cur_row*GlyphHeight+HUMBUG_CURSOR_ROW_START;
		x1 = cur_col*GlyphWidth+HUMBUG_CURSOR_COL_START;
		x2 = x1	+	(HUMBUG_CURSOR_COL_END - HUMBUG_CURSOR_COL_START);
		y2 = y1	+	(HUMBUG_CURSOR_ROW_END - HUMBUG_CURSOR_ROW_START);
		/*
		 * draw	the cursor
		 */
		//printf(	"draw box (%d,%d), (%d,%d)\n", x1, y1, x2, y2 );
		boxColor(	Screen,	x1,	y1,	x2,	y2,	HUMBUG_PHOSPHOR_COLOUR );
	} else {
		/*
		 * blank the previous	cursor rectangle
		 */
		//printf(	"clear box (%d,%d), (%d,%d)\n",	x1,	y1,	x2,	y2 );
		boxColor(	Screen,	x1,	y1,	x2,	y2,	255	);
		state	= 0;
	}
	SDL_Flip( Screen	);
	if( SDL_mutexV(	Vmem.scn_lock )	< 0 ) {
		printf(	"showcursor: mutex unlock error: %s\n",	SDL_GetError() );
		return;
	}
	return(	rate	);				/* indicate	we need	another	call-back */
}

/*
 * PRUN
 * This	is a replacement for system()
 * The function	uses popen() to	run	a pipe-line	command, passing stdin
 * to the PRINTF() function	in Humbug Flavour.
 * For Vanilla or Raspberry	the	command	is run via system(3)
 */

void prun( char	*cmd )
{
	char	buff[BUFSIZ];
	FILE	*pipe;

	if( Flavour == VER_HUMBUG ) {
		if( (pipe	= popen( cmd, "r" )) ==	(FILE *)0 ) {
			PRINTF(	"%m\n"	);
		} else {
			while(	fgets( buff, sizeof( buff ), pipe )	!= (char *)0 )
				if(	PRINTF(	buff ) < 0 ) {
					pclose(	pipe	);
					return;
				}
			pclose(	pipe );
		}
	} else
		system(	cmd );
	return;
}
#endif

/* MAIN	*/

void BASIC_main( int argc,	char *argv[] )
{
	const char *rcsid = "tinybasic.c,v 1.3 2012/11/10 18:05:34 afl Exp";
	char	 input[BUFSIZ];
	char	 tmpbuff[BUFSIZ];
	char	**cmdp;
	char	*cp;
	char	*lineText;
	char	*firstWord;
	char	*fileToLoad	= (char	*)0;
	char	*progbuf;
	int	 rval;
	int	 foundmatch	= 0;
	int	 rflag = 0;
	int	 fd;
	struct timeval tv;
	struct timezone	tz;
	struct commands	*basicCmds;
	FILE	  *FD;
	char	*ll2buffer();
	void	 parseLine(), loadProgram();
	void	 FPE_handler();
	void	 INT_handler();
	/*
	* initialise filename variable
	*/
	currFileName	= (char	*)0;

	/*
	* initialise the random	number generator
	*/
	gettimeofday( &tv, &tz );
	srand( (unsigned	int)tv.tv_sec );

#if	0
	/*
	* set up signal	handlers for SIGINT	and	SIGFPE
	*/
	struct sigaction	sa_FPE,	sa_INT;
	sa_FPE.sa_handler = FPE_handler;
	sa_INT.sa_handler = INT_handler;

	sigaction( SIGINT, &sa_INT, (struct sigaction *)0 );
	sigaction( SIGFPE, &sa_FPE, (struct sigaction *)0 );
#endif
	/*
	* initialise command history
	*/
	stredit(	(char *)0, 0, STREDIT_HIST_INIT	);

	/*
	* did we get any command-line args?
	*/
	if( argc	> 1	) {
		for( argv++; *argv; argv++ ) {
			//printf( "argv->\"%s\"\n", *argv );
			if( **argv	== '-' ) {
				//printf( "found a hyphen...\n"	);
				/*
				 * looks like an option...
				 */
				switch(	(*argv)[1] ) {
				case 'r':
					rflag++;
					break;
				case 'f':
					/*
					* flavour, what's the next character?
					*/
					switch(	(*argv)[2] ) {
					case	'v'	:
						Flavour	=	VER_VANILLA;
						break;
					case	'r'	:
						Flavour	=	VER_RASPBERRY;
						break;
					case	'h'	:
						Flavour	=	VER_HUMBUG;
						break;
					default	:
						printf(	"%s: unknown flavour option \"%s\"\n", progname, *argv );
						exit(	1 );
						break;
					}
					break;
				default	:
					printf(	"%s: unknown option \"-%c\"\n",	progname, (*argv)[1]	);
					exit( 1	);
					break;
				}
				//printf( "rflag=%d\n",	rflag );
			} else {
				/*
				 * option looks	like a filename...
				 */
				//printf( "next arg is \"%s\"\n", *(argv+1)	);
				if(	*(argv+1) != (char *)0 ) {
					printf(	"usage: %s [-fv|-fr] [[-r] program]\n",	progname	);
					exit( 1	);
				}
				fileToLoad = *argv;
			}
		}
	}

#if		0
	/*
	* if we're in humbug flavour, open an SDL window
	*/
	if( Flavour == VER_HUMBUG ) {
		SDL_TimerID tid;

		/* Start SDL */
		SDL_Init(	SDL_INIT_EVERYTHING	);
		SDL_Init(	SDL_INIT_TIMER );
		SDL_Init(	SDL_INIT_EVENTTHREAD );

		/*
		 * set up	the	VDU
		 */
		Vmem.rows	= HUMBUG_WIN_ROWS;
		Vmem.cols	= HUMBUG_WIN_COLS;
		Vmem.cursor_pos	=	0;
		Vmem.cursor_row	=	0;
		Vmem.showcursor	=	1;		/* display the cursor */
		/*
		 * read	the Triton Font
		 */
		if( loadFont(	TRITONFONT,	tmpbuff, sizeof( tmpbuff ) ) < 0 ) {
			printf(	"sorry: %s\n",	tmpbuff	);
			exit( 1	);
		} else {
			puts( tmpbuff );
		}
		/*
		 * Set up	screen
		 * window	size is	always 1024x384	for	the	Triton 16x64 format
		 */
		if( (Screen =	SDL_SetVideoMode( Vmem.cols*GlyphWidth,
						  Vmem.rows*GlyphHeight,
						  0, SDL_HWSURFACE )) == (SDL_Surface *)0 ) {
			printf(	"Can't create the screen for Humbug: %s\n", SDL_GetError()	);
			exit( 1	);
		}
		SDL_WM_SetCaption( "TinyBASIC (Humbug)", 0 );
		SDL_FillRect(Screen, NULL, SDL_MapRGB(Screen->format,	0, 0, 0));

		/*
		 * now set the default font to myfont
		 */
		gfxPrimitivesSetFont(	(const char	*)FontBase,
					(Uint32)GlyphWidth,
					(Uint32)GlyphHeight	);
		/*
		 * create	the	mutex for screen operations
		 */
		if( (Vmem.scn_lock = SDL_CreateMutex())	== (SDL_mutex	*)0	) {
			printf(	"Can't create screen mutex lock: %s\n",	SDL_GetError()	);
			exit( 1	);
		}
		/*
		 * start SDL displaying	the cursor
		 */
		if( (tid = SDL_AddTimer( HUMBUG_CURSOR_BLINK_RATE, showCursor, (void *)0 )) == (SDL_TimerID)0	) {
			printf(	"Can't start display cursor!  Error was %s\n",	SDL_GetError( )	);
		}
		/*
		 * and clear the VDU screen
		 */
		vdu( 0,	12 );
		/*
		 * enable	unicode	mode for getting keyboard input
		 */
		SDL_EnableUNICODE( SDL_ENABLE	);
	}
#endif
	/*
	* print	welcome	message, version & flavour
	*/
	PRINTF(	"\x0c" );
	
	switch(	Flavour	) {
	case	VER_VANILLA:
		PRINTF(	welcomeString, "vanilla" );
		break;
	case	VER_RASPBERRY:
		PRINTF(	welcomeString, "raspberry" );
		break;
	case	VER_HUMBUG:
		PRINTF(	welcomeString, "humbug"	);
		break;
	}
	PRINTF(	" (%s)\n", versionString	);

	if( fileToLoad != (char	*)0 ) {
		loadProgram( fileToLoad	);
		if( rflag	) {
			/*
			 *	create a working buffer	(in	memory)	from the linked-list
			 *	pass this to the run() function
			 */
			if( (progbuf =	ll2buffer()) ==	(char *)0 ) {
				PRINTF(	"sorry, cannot allocate enough memory to run program\n"	);
				return;
			}
			PRINTF(	", running...\n" );
			run( progbuf, 1	);
			free( progbuf );
		} else
			PRINTF(	"\n" );
	}

	/*
	* we're	ready to accept	data, so print ready message
	*/
	PRINTF(	"%s\n",	readyString );

	/*
	* now we enter the main	loop
	* print	a prompt, get a	line of	input and process it...
	*/
	while( 1	) {
		PRINTF(	"%s",	promptString );
		fflush(	stdout );

		strcpy(	input, ""	);				/* clear string	ready for a	new	input */
		/* read	line with	edit */
		if( (rval	= stredit( input, sizeof( input	)-1, STREDIT_HIST_ENABLE ))	!= 0) {
			if( rval == -1	) {
				/*
				 * user	pressed	^C
				 */
				if(	Work.bufferUnsaved ) {
					if( yesORno(	"Program has unsaved changes. Really quit?", "yes",	"no" ) == 1	)
						exit(	0 );
				} else
					exit( 0	);
			} else {
				PRINTF(	"string edit failed (%d)\n", rval );
				exit( 2	);
			}
		}
		if( strlen( input	) == 0 )  continue;		/* empty line, get next	line */

		if( input[ strlen(input)-1 ] ==	'\n' )
			input[	strlen(input)-1	] =	'\0';		/* remove the trailing newline */
		/*
		 * lines will	be formed of a word, 1 or more spaces and then additional text
		 * see if	we can match the first word
		 */
		parseLine( input,	&firstWord,	&lineText );

		if( firstWord	== (char *)0 ) {
			/*	empty string */
			continue;
		}
		/*
		 * scan	the commandTable looking for a match with	firstWord
		 */
		foundmatch = 0;
		for( cmdp=commandTable;	*cmdp; cmdp++	) {
			if( strcasecmp(	firstWord,	*cmdp )	== 0 ) {
				foundmatch++;
				break;
			}
		}
		if( foundmatch ) {
			doEditCommand(	cmdp, lineText );
			continue;
		}
		/*
		 * we	don't recognise	this command
		 * is	the	first word a decimal number?
		 */
		for( cp=firstWord; *cp &&	isdigit( (int)(*cp) );	cp++ ) ;
		if( *cp	) {
			/*
			 *	not	a command we recognise or a	statement number.
			 *	is it a	BASIC statement?
			 */
			for( basicCmds=table; basicCmds->tok; basicCmds++ )
				if(	strcasecmp(	basicCmds->command,	firstWord )	== 0 )
					break;
			if( basicCmds->tok	== 0 ) {
				PRINTF(	"%s: unrecognised command.  Try \"help\"\n", firstWord );
				continue;
			}
			snprintf( tmpbuff,	BUFSIZ-1, "%s %s\n", firstWord,	lineText );
			/*
			 *	and	call run() ...
			 */
			run( tmpbuff, 0	);
			continue;
		}
		/*
		 * this	is a line	of code	to be added	to the buffer
		 */
		if( strlen( firstWord	) >= LAB_LEN ) {
			PRINTF(	"%s: line number too long.  %d digits maximum\n", firstWord, (LAB_LEN-1) );
			continue;
		}
		if( addtoBuffer( atoi( firstWord ), lineText,	&Work )	!= 0 ) {
			PRINTF(	"Sorry, out of memory\n" );
		}
		continue;
	}
}

/*
 * LL2BUFFER
 * Create a	heap-allocated buffer to hold the linked-list program
 * Return a	pointer	to the allocated buffer, or	0 if malloc	fails
 */

char *ll2buffer()
{

	char	tmp[BUFSIZ];
	char	*progbuf;
	char	*cp;
	int	progsize = 0;
	struct _progListElem	*LP	  =	(struct	_progListElem *)0;

	/*
	* first, work-out the size of the heap buffer we need
	*/
	for(	LP=Work.first; LP; LP=LP->next ) {
		snprintf(	tmp, BUFSIZ-1, "%d %s\n", LP->lineNumber, LP->lineText );
		progsize += strlen( tmp	);
	}
	progsize++;	/* add space for terminating null */
	if( (progbuf	= (char	*)malloc( progsize )) == (char *) 0) {
		return(	(char	*)0	);
	}
	*progbuf	= '\0';
	/*
	* now "print" program into the buffer
	*/
	for(	LP=Work.first, cp=progbuf; LP; LP=LP->next ) {
		sprintf( tmp,	"%d %s\n", LP->lineNumber, LP->lineText	);
		strcat(	cp, tmp	);
		cp +=	strlen(	tmp	);
	}
	//printf( "buffer: >>%s<<", progbuf );
	return(	progbuf	);
}

/*
 * COLOUR2UNIT
 * Given a colour string, convert it to	a Uint32 value
 */

Uint32 colour2Uint(	char *col, Uint32 *val )
{
	struct _colours	*C;

	/*
	* we need to trundle through the predef_colours	array looking for the
	* give colour, then	return it's	value
	*/
	for(	C=predef_colours; C->name; C++ ) {
		if( strcasecmp(	C->name, col ) ==	0 ) {
			*val =	C->value;
			return(	0 );
		}
	}
	return(	-1 );
}
#if	0
/*
 * EXEC_DRAW
 * This	is the basic command to	interface to the SDL_gfx library
 * This	function implements	a sub-set of the libSDL_gfx	library	functions:
 *
 * Current support for
 * DRAW	START[,TITLE[,WIDTH,HEIGHT]]
 * DRAW	CLEAR[,COLOUR]
 * DRAW	END
 * DRAW	POINT,X,Y[,COLOUR[,ALPHA]]
 * DRAW	CIRCLE,X,Y,R[,FILL[,COLOUR[,ALPHA]]]
 * DRAW	TRIANGLE,X1,Y1,X2,Y2,X3,Y3[,FILL[,COLOUR[,ALPHA]]]
 * DRAW	BOX,X1,Y1,X2,Y2[,FILL[,COLOUR[,ALPHA]]]
 * DRAW	LINE,X1,Y1,X2,Y2[,WIDTH[,COLOUR[,ALPHA]]]
 * DRAW	TEXT,X1,Y1,"text"[,COLOUR[,ALPHA]]
 *
 * COLOUR is one of	the	predefined colour values
 */

int	exec_draw( void	)
{
	int	  x, y,	x1,	y1,	x2,	y2,	x3,	y3,	r, fill	= 0;
	int	  width	= 1;
	int	  a, b,	c, d, t;
	int	  alpha;
	int	  err;
	int	  args;
	Uint32 colour, workingColour;
	char	 *drawtype,	*wintitle, *string;

	if( Trace ) PRINTF( "%s draw\n",	progLineNum( prog )	);

	tok = get_token();					/* get next	list item */
	if( tok	!= VARIABLE ) serror( 17	);	/* draw-type expected */
	else	drawtype = strdup( token );

	//printf( "draw type: %s\n",	drawtype );

	if( strcasecmp(	drawtype, "start" ) == 0	) {
		//printf(	"entering the new parser...\n" );
		Screen_height	= WINDOW_HEIGHT;
		Screen_width	= WINDOW_WIDTH;
		wintitle = (char *)WINDOW_TITLE;
		//gfxPrimitivesSetFont(	(const void *)0, (Uint32)0, (Uint32)0	);
		if( (args	= argParser( DRAWTP_START, &err, &wintitle,	&Screen_width, &Screen_height )) ==	-1 ) {
			//printf( "start: arg parse error=%d\n", err );
			serror(	err );
		} else {
			//printf( "got %d args from draw start command\n",	args );
			if( (Screen = SDL_SetVideoMode(	(Sint16)Screen_width, (Sint16)Screen_height, 0,	SDL_HWSURFACE )) == (SDL_Surface *)0 ) {
				printf(	"cannot open window: %s\n",	SDL_GetError() );
				return;
			}
			SDL_WM_SetCaption(	wintitle, 0	);
			/*
			 *	initialise the default colour
			 */
			colour2Uint( DEFAULT_COLOUR, &currentColour );
			gfxPrimitivesSetFont( (const char *)FontBase,
					      (Uint32)GlyphWidth,
					      (Uint32)GlyphHeight );
			return;
		}
	}
	if( strcasecmp(	drawtype, "end"	) ==	0 ) {
		SDL_Quit();
		Screen = (SDL_Surface	*)0;		/* flag	the	window was closed */
		return;
	}
	if( strcasecmp(	drawtype, "clear" ) == 0	) {
		/*
		 * has a surface been	opened?
		 */
		if( Screen ==	(SDL_Surface *)0 ) {
			serror(	18	);
		}
		if( (args	= argParser( DRAWTP_CLEAR, &err, &colour ))	== -1 )	{
			//printf( "clear: arg parse error=%d\n", err );
			serror(	err );
		}
		//printf(	"got %d args from draw clear command\n", args );
		/*
		 * we	might have 0 or	1 argument
		 * if	we have	1 we have a	colour
		 */
		if( args == 1	) {
			currentColour =	colour;
			workingColour =	currentColour;
		} else {
			/*
			 *	no colour specified, use black
			 */
			colour2Uint( "black", &workingColour );
		}
		/*
		 * for the clear only	always use alpha 0xff
		 */
		workingColour	|= 0xff;
		/*
		 * draw	a	filled rectangle (box) of the size of the surface
		 * in	order to "clear" it
		 */
		//printf(	"about to draw box, size %dx%d in colour 0x%lx\n", Screen_height, Screen_width,	workingColour );
		boxColor(	Screen,	(Sint16)0, (Sint16)0, (Sint16)Screen_width,	(Sint16)Screen_height, workingColour );
		SDL_Flip(	Screen );
		return;
	}
	if( strcasecmp(	drawtype, "point" ) == 0	) {
		/*
		 * has a surface been	opened?
		 */
		if( Screen ==	(SDL_Surface *)0 ) {
			serror(	18	);
		}
		if( (args	= argParser( DRAWTP_POINT, &err, &x, &y, &colour, &alpha ))	== -1 )	{
			//printf( "point: arg parse error=%d\n", err );
			serror(	err );
		}
		//printf(	"got %d args from draw point command\n", args );
		/*
		 * we	must have 2	args (x,y)
		 * if	we have	three we have a	colour
		 * if	we have	4 we have a	colour and alpha
		 */
		if( args == 4	) {
			currentAlpha  =	alpha &	0xff;
			currentColour =	colour;
		} else if( args	== 3 ) {
			currentColour =	colour;
		}
		workingColour	= (currentColour & ~0xff) |	currentAlpha;
		/*
		 * draw	the point
		 */
		//printf(	"about to draw pixel at %d, %d in colour 0x%lx\n", x, y, workingColour );
		pixelColor( Screen, (Sint16)x, (Sint16)y,	workingColour );
		SDL_Flip(	Screen );
		return;
	}
	if( strcasecmp(	drawtype, "circle" )	== 0 ) {
		/*
		 * has a surface been	opened?
		 */
		if( Screen ==	(SDL_Surface *)0 ) {
			serror(	18	);
		}
		if( (args	= argParser( DRAWTP_CIRCLE,	&err, &x, &y, &r, &fill, &colour, &alpha ))	== -1 )	{
			//printf( "circle arg parse error=%d\n", err );
			serror(	err );
		}
		//printf(	"got %d args from draw circle command\n", args );
		/*
		 * we	must have 3	args (x,y,r)
		 * if	4 then we have fill
		 * if	5 then we have fill	and	colour
		 * if	6 then we have fill, colour	and	alpha
		 */
		if( args == 6	) {
			currentAlpha  =	alpha &	0xff;
			currentColour =	colour;
		} else if( args	== 5 ) {
			currentColour =	colour;
		}
		workingColour	= (currentColour & ~0xff) |	currentAlpha;
		/*
		 * draw	the circle
		 */
		if( fill ) {
			//printf( "about to draw filledcircle at %d, %d, rad=%d in colour 0x%lx\n", x,	y, r, workingColour	);
			filledCircleColor(	Screen,	(Sint16)x, (Sint16)y, (Sint16)r, workingColour );
		} else {
			//printf( "about to draw circle at %d, %d, rad=%d in colour 0x%lx\n", x, y, r,	workingColour );
			circleColor( Screen, (Sint16)x,	(Sint16)y,	(Sint16)r, workingColour );
		}
		SDL_Flip(	Screen );
		return;
	}
	if( strcasecmp(	drawtype, "triangle"	) == 0 ) {
		/*
		 * has a surface been	opened?
		 */
		if( Screen ==	(SDL_Surface *)0 ) {
			serror(	18	);
		}
		if( (args	= argParser( DRAWTP_TRIANGLE, &err,
					 &x, &y,
					 &x2, &y2,
					 &x3, &y3,
					 &fill,	&colour, &alpha	))	== -1 )	{
			//printf( "triangle arg parse error=%d\n",	err	);
			serror(	err );
		}
		//printf(	"got %d args from draw triangle command\n",	args );
		/*
		 * we	must have 6	args (x1,y1,x2,y2,x3,y3)
		 * if	7 then we have fill
		 * if	8 then we have fill	and	colour
		 * if	9 then we have fill, colour	and	alpha
		 */
		if( args == 9	) {
			currentAlpha  =	alpha &	0xff;
			currentColour =	colour;
		} else if( args	== 8 ) {
			currentColour =	colour;
		}
		workingColour	= (currentColour & ~0xff) |	currentAlpha;
		/*
		 * draw	the triangle
		 */
		if( fill ) {
			//printf( "about to draw filledtriangle at (%d, %d) (%d, %d) (%d, %d) in colour 0x%lx\n", x, y,	x2, y2,	x3, y3,	workingColour );
			filledTrigonColor(	Screen,
					    (Sint16)x, (Sint16)y,
					    (Sint16)x2,	(Sint16)y2,
					    (Sint16)x3,	(Sint16)y3,
					    workingColour );
		} else {
			//printf( "about to draw triangle at (%d, %d) (%d, %d) (%d, %d) in colour 0x%lx\n", x,	y, x2, y2, x3, y3, workingColour );
			trigonColor( Screen,
				     (Sint16)x,	(Sint16)y,
				     (Sint16)x2,	(Sint16)y2,
				     (Sint16)x3,	(Sint16)y3,
				     workingColour );
		}
		SDL_Flip(	Screen );
		return;
	}
	if( strcasecmp(	drawtype, "box"	) ==	0 ) {
		/*
		 * has a surface been	opened?
		 */
		if( Screen ==	(SDL_Surface *)0 ) {
			serror(	18	);
		}
		if( (args	= argParser( DRAWTP_RECT, &err,
					 &x, &y,
					 &x2, &y2,
					 &fill,	&colour, &alpha	))	== -1 )	{
			//printf( "box arg parse error=%d\n", err );
			serror(	err );
		}
		//printf(	"got %d args from draw box command\n", args	);
		/*
		 * we	must have 4	args (x1,y1,x2,y2)
		 * if	5 then we have fill
		 * if	6 then we have fill	and	colour
		 * if	7 then we have fill, colour	and	alpha
		 */
		if( args == 7	) {
			currentAlpha  =	alpha &	0xff;
			currentColour =	colour;
		} else if( args	== 6 ) {
			currentColour =	colour;
		}
		workingColour	= (currentColour & ~0xff) |	currentAlpha;
		/*
		 * draw	the box
		 */
		if( fill ) {
			//printf( "about to draw box at (%d, %d) (%d, %d) in colour 0x%lx\n", x, y, x2,	y2, workingColour );
			boxColor( Screen,
				  (Sint16)x, (Sint16)y,
				  (Sint16)x2,	(Sint16)y2,
				  workingColour	);
		} else {
			//printf( "about to draw rectangle at (%d, %d) (%d, %d) in colour 0x%lx\n", x,	y, x2, y2, workingColour );
			rectangleColor(	Screen,
					(Sint16)x, (Sint16)y,
					(Sint16)x2,	(Sint16)y2,
					workingColour );
		}
		SDL_Flip(	Screen );
		return;
	}
	if( strcasecmp(	drawtype, "line"	) == 0 ) {
		/*
		 * has a surface been	opened?
		 */
		if( Screen ==	(SDL_Surface *)0 ) {
			serror(	18	);
		}
		if( (args	= argParser( DRAWTP_LINE, &err,
					 &x1, &y1,
					 &x2, &y2,
					 &width,
					 &colour, &alpha )) ==	-1 ) {
			//printf( "line arg parse error=%d\n",	err	);
			serror(	err );
		}
		//printf(	"got %d args from draw line command\n",	args );
		/*
		 * we	must have 4	args (x1,y1,x2,y2)
		 * if	5 then we have width
		 * if	6 then we have width and colour
		 * if	7 then we have width, colour  and alpha
		 */
		if( args == 7	) {
			currentAlpha  =	alpha &	0xff;
			currentColour =	colour;
		} else if( args	== 6 ) {
			currentColour =	colour;
		} else if( args	== 5 ) {
			if( width > 1 && width%2 == 0 )	{
				/*
				 * think line selected,	but	even-width.	 Increment width to	makde it odd
				 */
				//printf( "width even, adjusted to %d\n", width	);
				width++;
			}
		}
		workingColour	= (currentColour & ~0xff) |	currentAlpha;
		/*
		 * draw	the line using the primary coordinates
		 */
		//printf(	"about to draw line from (%d, %d) to (%d, %d), [width=%d] in colour 0x%lx\n", x1, y1, x2, y2, width, workingColour );
		lineColor( Screen,
			   x1, y1,
			   x2, y2,
			   workingColour );

		if( width	> 1	) {
			/*
			 *	now	draw additional	lines each side	if the first line
			 *	to create a	"thick"	line
			 */
			/*
			 *	normally we	will draw the additional lines "above" and "below"
			 *	the	primary	line, except if	the	primary	line is	vertical
			 */
			/*
			 *	is the primary line	vertical?
			 */
			if( x1	== x2 )	{
				/*
				 * its a vertical line
				 */
				for( t=1; width>1; width-=2,t++	) {
					/*
					* draw a line "left" of	the	primary	line
					*/
					a=x1-t,	b=y1;
					c=x2-t,	d=y2;
					//printf( "about to draw extra line left from (%d, %d) to (%d, %d)\n", a, b,	c, d );
					lineColor( Screen, a, b,	c, d, workingColour	);
					/*
					* draw a line "right" of the primary line
					*/
					a=x1+t,	b=y1;
					c=x2+t,	d=y2;
					//printf( "about to draw extra line right from (%d, %d) to (%d, %d)\n",	a, b, c,	d );
					lineColor( Screen, a, b,	c, d, workingColour	);
				}
			} else {
				for( t=1; width>1; width-=2, t++ ) {
					/*
					* draw a line "above" the primary line
					*/
					a=x1, b=y1-t;
					c=x2, d=y2-t;
					//printf( "about to draw extra line above from (%d, %d) to (%d, %d)\n",	a, b, c,	d );
					lineColor( Screen, a, b,	c, d, workingColour	);
					/*
					* draw a line "below" the primary line
					*/
					a=x1, b=y1+t;
					c=x2, d=y2+t;
					//printf( "about to draw extra line below from (%d, %d) to (%d, %d)\n",	a, b, c,	d );
					lineColor( Screen, a, b,	c, d, workingColour	);
				}
			}
			/*
			 *	finally, we	draw a filled circle centre	x1,y1 and x2,y2
			 *	both with radius "(width/2)"
			 *
			t--;
			//printf( "now using filled circles with radius=%d\n",	t );
			filledCircleColor(	Screen,	(Sint16)x1,	(Sint16)y1,	(Sint16)t, workingColour );
			filledCircleColor(	Screen,	(Sint16)x2,	(Sint16)y2,	(Sint16)t, workingColour );
			*/
		}
		SDL_Flip(	Screen );
		return;
	}
	if( strcasecmp(	drawtype, "text"	) == 0 ) {
		/*
		 * has a surface been	opened?
		 */
		if( Screen ==	(SDL_Surface *)0 ) {
			serror(	18	);
		}
		if( (args	= argParser( DRAWTP_TEXT, &err,
					 &x1, &y1,
					 &string,
					 &colour, &alpha )) ==	-1 ) {
			//printf( "line arg parse error=%d\n",	err	);
			serror(	err );
		}
		//printf(	"got %d args from draw text command\n",	args );
		/*
		 * we	must have 3	args (x1,y1) and string
		 * if	4 then we have colour
		 * if	5 then we have colour  and alpha
		 */
		if( args == 5	) {
			currentAlpha  =	alpha &	0xff;
			currentColour =	colour;
		} else if( args	== 4 ) {
			currentColour =	colour;
		}
		workingColour	= (currentColour & ~0xff) |	currentAlpha;
		/*
		 * draw	the text
		 */
		//printf(	"about to draw text \"%s\" at (%d, %d) in colour 0x%lx\n", string, x1, y1, workingColour );
		stringColor( Screen,
			     x1, y1,
			     string,
			     workingColour );

		SDL_Flip(	Screen );
		/*
		 * free-up memory	used by	the	string
		 */
		free(	string );
		return;
	}
}

/*
 * EXEC_WAIT
 * WAIT	implements a number	of different ways for a	BASIC program to
 * wait	for	something to happen
 *
 * WAIT	<TYPE>[,<OPTIONS>]
 *
 * WAIT	DELAY,X		  -	wait X milliseconds
 * WAIT	KEY[,X[,VAR]  -	wait for key X (in graphics	window or humbug).
 *						If absent means	"any key".	If VAR is present
 *						return ASCII code of key in	variable
 */

void exec_wait()
{
	char		 *waittype;
	char		 *keyname;
	char		 *varname;
	int		 delay,	must_wait;
	int		 var;
	SDL_Event event;

	if( Trace ) PRINTF( "%s wait\n",	progLineNum( prog )	);
	tok = get_token();					/* get next	list item */
	if( tok	!= VARIABLE ) serror( 22	);	/* wait-type expected */
	else	waittype = strdup( token );

	//printf( "wait type: %s\n",	waittype );

	if( strcasecmp(	waittype, "delay" ) == 0	) {
		get_token();
		//printf(	"tok=%d, token_type=%d, *token='%c'\n",	tok, token_type, *token	);
		if( token_type !=	DELIMITER ) {
			serror(	0 );
			return;
		}
		get_exp( &delay, 0 );		/* get the delay time */
		if( usleep( delay*1000 ) != 0	) {
			//printf( "usleep failed!, errno=%d\n",	errno );
		}
		return;
	}
	if( strcasecmp(	waittype, "key"	) ==	0 ) {
		//printf(	"in wait key\n"	);
		get_token();
		//printf(	"token->'%c'\n", *token	);
		if( tok	== EOL ||	tok	== FINISHED	|| *token == ';' ) {
			/*
			 *	no particular key requested, wait for any keypress
			 */
			keyname	= (char	*)0;
			varname	= (char	*)0;
		} else {
			get_token();			/* point to	the	key-name argument */
			keyname	= strdup( token	);
			if( strcmp( keyname, "," )	== 0 ) {
				free( keyname );
				keyname	= (char	*)0;
			}
			//printf( "got keyname. keyname='%s'\n", keyname );
			if( keyname ) {
				get_token();
				if(	tok	== EOL || tok == FINISHED || *token	== ';' ) {
					/*
					* no variable given
					*/
					varname	= (char	*)0;
				}
			} else {
				get_token();
				//printf( "got varname. token->'%c'\n",	*token );
				if(	!isalpha( *token ) ) {
					serror(	4 );
					return;
				}
				varname	= token;
				var	= toupper( *varname	) -	'A';
			}
		}
		/*
		printf(	"WAIT: waiting for " );
		if( keyname )	 printf( "keyname='%s'", keyname );
		else			 printf( "any" );
		printf(	" keypress\n"	);
		if( varname )	 printf( "assign to %c\n", 'A'+var );
		else			 printf( "do not assign\n" );
		*/

		/*
		 * is	SDL	initialised?
		 */
		if( Flavour != VER_HUMBUG	) {
			SDL_EnableUNICODE(	SDL_ENABLE );
			if( Screen	== (SDL_Surface	*)0	)
				serror(	18 );
		}
		while( SDL_WaitEvent(	&event ) ) {
			if( event.type	== SDL_KEYDOWN ) {
				/*
				printf(	"key value = %02x, key name = %s\n",
				   event.key.keysym.unicode,
				   SDL_GetKeyName( event.key.keysym.sym	) );
				*/
				if(	keyname	== (char *)0 ) {
					if( event.key.keysym.unicode	!= 0 ) break;
				} else {
					if( strcasecmp(	keyname,	SDL_GetKeyName(	event.key.keysym.sym ) ) ==	0 )
						break;
				}
			}
		}
		/*
		 * if	there was a	variable name, return unicode value	of the key
		 */
		if( varname ) {
			variables[	var	] =	(Uint16)event.key.keysym.unicode;
		}
		if( Flavour != VER_HUMBUG	)  SDL_EnableUNICODE( SDL_DISABLE );
		if( keyname )	 free( keyname );
	}
	return;
}

/*
 * CLOSESCREEN
 * Called when the BASIC program stops.
 * Print a message in the top-left of the screen and waits for "escape"
 * Then	closes SDL Screen
 */

void closeScreen( char *s )
{
	int x=50, y=50;		/* position	of the message */
	char	*endMessage	= "Press Escape to close";
	char	string[100];
	int	wait=1;
	SDL_Event event;
	Uint32 msgColour;

	if( Screen == (SDL_Surface *)0 )	 return;

	/*
	* is there a DISPLAY env var, indicating we're running under X11?
	*/
	if( getenv( "DISPLAY" )	!= (char	*)0	|| Flavour == VER_HUMBUG )
		return;			/* yes,	so no need to print	message	*/

	/*
	* start	by copying the supplied	string
	*/
	strncpy(	string,	s, sizeof( string )-1 );
	string[	sizeof(string)-1	] =	'\0';
	/*
	* add a	space
	*/
	strncat(	string,	" ", sizeof(string)-1 );
	string[	sizeof(string)-1	] =	'\0';
	/*
	* and then catenate	the	"press escape" message
	*/
	strncat(	string,	endMessage,	sizeof(string)-1 );
	string[	sizeof(string)-1	] =	'\0';

	colour2Uint(	"raspberry", &msgColour	);
	msgColour = (msgColour &	~0xff) | 0xff;		/* force alpha to 255 */
	stringColor(	Screen,	x, y, string, msgColour	);
	SDL_Flip( Screen	);
	/*
	* now wait for escape
	*/
	while( wait ) {
		while( SDL_WaitEvent(	&event ) ) {
			if( event.type	== SDL_KEYDOWN ) {
				if(	strcasecmp(	"escape", SDL_GetKeyName( event.key.keysym.sym ) ) == 0	)  wait	= 0;
			}
		}
	}
	/*
	* now quit SDL
	*/
	SDL_Quit();
	Screen =	(SDL_Surface *)0;
}
#endif
/*
 * The original	tiny basic C program was designed to run
 * a program given a filename.	Its	main() function	has	been changed
 * to run()	and	is now called by the editor
 *
 * RUN
 * given: pointer to buffer	containing the code	to run
 *		  showstop (1=show where code stops, 0=be silent
 * return: 1 indicates failure to execute, 0 indicates success
 */

int	run( char *runme, int showstop )
{
//	char in[80];
//	int  answer;
//	int  array_index;
//	char *t;
	char *progLineNum();
	struct timeval progStartTime;
	struct timezone	tz;
//	SDL_Event	event;

	p_buf	= runme;
	/*
	 * strip-off the terminating newline (if there is	one)
	 */
	if( strlen( runme	)-1	== '\n'	)
		*(runme	+ strlen( runme	)-1) =	'\0';

	/*
	 * prepare the ^C	interrupt control
	 */
	userStopRequest	=	0;

	if(setjmp(e_buf))	return(1); /* initialize the long jump buffer */

	// reset the stack
	sp = 0;

	prog = p_buf;
	scan_labels();		/* find	the	labels in the program */

	strcpy(	Lastif.line_num, "" );		/* clear last-if knowledge */

	/*
	 * get the time	we start...
	 */
	gettimeofday(	&progStartTime,	&tz	);

	do {
#if	0
		if(	Flavour	== VER_HUMBUG )	{
			/*
			* do a check for ^C	here...
			*/
			SDL_PollEvent( &event );
			switch(	event.type ) {
			case	SDL_KEYDOWN:
				if( (Uint16)event.key.keysym.unicode ==	3	)
					userStopRequest++;
			}
		}
#endif
		if(	userStopRequest	) {
			PRINTF(	"\nprogram interrupted at line %s\n", progLineNum( prog	) );
			execute_time( progStartTime );
			return(0);
		}
		get_token();
		if(	token_type == DELIMITER	&& tok == FINISHED ) {
			//printf( "FINISHED token found\n" );
			break;
		}
		//printf( "in main loop...token_type=%d %d\n",	token_type,tok );
		/* check for assignment	statement */
		if(	token_type == VARIABLE || token_type ==	ARRAY )	{
			putback(); /*	return the var name	to the input stream	*/
			assignment();	/* must	be assignment statement	*/
		} else { /*	is command */
			//printf(	"line %s\n", progLineNum( prog ) );
			switch(tok) {
			case PRINT:
				exec_print();
				break;
			case GOTO:
				exec_goto();
				break;
			case IF:
				exec_if();
				break;
			case FOR:
				exec_for();
				break;
			case NEXT:
				exec_next();
				break;
			case INPUT:
				exec_input();
				break;
			case GOSUB:
				exec_gosub();
				break;
			case RETURN:
				exec_return();
				break;
			case LET:
				exec_let();
				break;
#if	0
			case DRAW:
				exec_draw();			/* interface with SDL_gfx */
				break;
			case WAIT:
				exec_wait();			/* pause/wait for various events */
				break;
#endif
			case TRACE:
				exec_trace();
				break;
			case WHILE:
				exec_while();
				break;
			case WEND:
				exec_wend();
				break;
			case BREAK:
				exec_break();
				break;
			case CONTINUE:
				exec_continue();
				break;
			case REPEAT:
				exec_repeat();
				break;
			case UNTIL:
				exec_until();
				break;
			case ELSE:
				exec_else();
				break;
			case VDU :
				exec_vdu();
				break;
			case REM:
				find_eol();
				break;
//			default:	// $$
//				printf( "case: token_type=%d tok=%d ???\n",	token_type,tok );
//				break;
			case STOP:
				if( showstop ) {
					PRINTF(	"program stopped at line %s\n",	progLineNum( prog ) );
					closeScreen( "Program Stop." );
					execute_time( progStartTime );
				}
				return(0);
				break;
			}
		}
	}	while (tok != FINISHED);
	/*
	 * can we	tell the user what the last	line number	was?
	 */
	putback();
	if( showstop ) {
		PRINTF(	"program stopped after last line %s\n",	progLineNum( prog ) );
		closeScreen( "Program Stop." );
		execute_time( progStartTime );
	}
	return(	0	);
}

/*
 * functions for the basic interpretor
 */
#if	0
/*
 * EXECUTE_TIME
 * show	how	long the program took to run
 */

void execute_time( struct timeval begin	)
{
	suseconds_t	  u;
	int			  s;
	struct timeval end;
	struct timezone	tz;

	gettimeofday( &end, &tz	);
	//printf( "start %d.%d  end %d.%d\n", begin.tv_sec, begin.tv_usec, end.tv_sec, end.tv_usec );

	s = end.tv_sec -	begin.tv_sec;
	if( (u =	end.tv_usec	- begin.tv_usec) < 0 ) {
		u	+= 1000000;
		s--;
	}
	PRINTF(	"program took %d.%d seconds\n",	s, (u/1000) );
	return;
}
#endif
/* ASSIGNMENT: Assign a	variable a value.
 * extended	by AFL to support assignment to	the	@()	array
 */

void assignment()
{
	int var, index,	value;

	if( Trace	) PRINTF( "%s assignment\n", progLineNum( prog ) );
	Lastif.skip_else = 0;			/* reset as	this is	not	an else	statement */

	//printf(	"in assignment...\n" );
	/* get the variable name */
	get_token();
	if( isalpha( (int)(*token) ) ) {
		var = toupper( (int)(*token) ) -'A';

		/*	get	the	equals sign	*/
		get_token();
		if(*token!='=')	{
			serror(3);
			return;
		}

		//printf( "var assign: about get get value, *prog->\"%s\"\n", prog	);
		/*	get	the	value to assign	to var */
		get_exp(&value,	0);

		/*	assign the value */
		//printf( "assigning %d to %c\n", value, 'A'+var );
		variables[var]	= value;
	} else if( *token == '@' ) {
		//printf( "assign to array\n" );
		/*
		 *	assignment to array.  Get index	value
		 */
		get_exp( &index, 1	);
		if( index < 0 )	{
			serror(	14 );			/* array index error */
			return;
		}
		//printf( "array index = %d\n",	index );
		//printf( "(before get_token) token -> '%c'\n",	*token	);
		//printf( "(before get_token) prog -> '%c'\n",	*prog );
		/*	get	the	equals sign	*/
		get_token();
		//printf( "(after get_token) token -> '%c'\n",	*token );
		//printf( "(after get_token) prog -> '%c'\n", *prog );
		if(*token!='=')	{
			serror(3);
			return;
		}

		//printf( "@ assign: about get get value, *prog->\"%s\"\n", prog );
		/*	get	the	value to assign	to var */
		get_exp(&value,	1);

		/*	assign the value */
		//printf( "assigning %d to @(%d)\n", value, index );
		arrayStore( index,	value );
	} else {
		serror(4);
		return;
	}
}

/*
 * EXEC_LET
 * Assign value	to a variable
 */
void exec_let()
{
	if( Trace ) PRINTF( "%s let\n",	progLineNum(	prog ) );
	token_type =	get_token();
	/* check	for	assignment statement */
	if( token_type != VARIABLE && token_type	!= ARRAY ) {
		serror(	0	);
	} else {
		putback();		/* put the variable	token back into	the	queue */
		assignment();		/* and do assignment */
	}
}

/* EXEC_PRINT
 * Execute a simple	version	of the BASIC PRINT statement
 * The original	version	of PRINT used ";" a	legal separator	of
 * valus to	be printed but that	means you cannot use ";" as	a
 * command separator.
 *
 * PRINT has been modified to make it compatible with Palo Alto	BASIC
 * by makeing the following	changes:
 * - fields	are	always separated by	","
 * - fields	are	normally printed in	a field-width of 6 characters
 * - special fields	consisting of "#nnn" change	the	field-width	for
 *	 the following values to nnn spaces.  This is reset	back to	6
 *	 spaces	for	the	next PRINT stmt
 */
void exec_print()
{
	int	answer;
	int	fwidth = 8;		/* until changed by	a #	field */
	char	last_delim;
	char	cs[20];

	if( Trace ) PRINTF( "%s print\n", progLineNum( prog ) );
	Lastif.skip_else	= 0;		 /*	reset as this is not an	else statement */

	do {
		get_token(); /*	get next list	item */
		//printf(	"tok=%d, *token->'%c'\n", tok, *token );
		if( tok==EOL ||	tok==FINISHED	|| (token_type==DELIMITER && *token==';') ) {
			//printf( "breaking...\n" );
			break;
		}
		if( *token ==	'#'	) {
			/*
			 *	field-width	setting
			 */
			get_token();
			sscanf(	token,	"%d", &fwidth );
			//printf( "fwidth=%d\n", fwidth	);
			get_token();
		} else if( token_type==QUOTE ) {
			/*	string */
			PRINTF(token);
			get_token();
			//printf( "string: *token->'%c'\n", *token	);
		} else {
			/*	expression */
			putback();
			get_exp(&answer, 0);
			sprintf( cs, "%%%dd", fwidth );
			PRINTF(	cs, answer	);
			get_token();
		}
		last_delim = *token;
		//printf(	"last_delim=>%c<\n", last_delim	);

		//printf(	"tok=%d\n",	tok	);

		if( tok!=EOL &&	tok!=FINISHED	&& token_type!=DELIMITER )	serror(0);

	} while(	*token==','	);

	if(tok==EOL || tok==FINISHED	|| (token_type==DELIMITER && *token==';') )	{
		if( last_delim !=	','	) PRINTF("\n");
		else					  fflush( stdout );
	} else	serror(0); /* error	is not , or	; */
}

/* Find	all	labels.	*/
void scan_labels()
{
	int addr;
	char *temp;

	label_init();	 /*	zero all labels	*/
	temp = prog;	 /*	save pointer to	top	of program */

	/* if	the	first token	in the file	is a label */
	get_token();
	if(token_type==NUMBER) {
		strcpy(label_table[0].name,token);
		label_table[0].p=prog;
	}

	find_eol();
	do {
		get_token();
		if(token_type==NUMBER) {
			addr = get_next_label(token);
			if(addr==-1 || addr==-2) {
				(addr==-1) ?serror(5):serror(6);
			}
			strcpy(label_table[addr].name, token);
			label_table[addr].p =	prog;  /* current point	in program */
		}
		/* if not on a blank line, find	next line */
		if(tok!=EOL) find_eol();
	}	while(tok!=FINISHED);
	prog = temp;	/* restore to original */
}

/* Find	the	start of the next line.	*/
void find_eol()
{
	while(*prog!='\n'	 &&	*prog!='\0') ++prog;
	if(*prog)	prog++;
}

/* Return index	of next	free position in label array.
   A -1	is returned	if the array is	full.
   A -2	is returned	when duplicate label is	found.
*/
int get_next_label(char	*s)
{
	register int t;

	for(t=0; t<NUM_LAB; ++t) {
		if(label_table[t].name[0]==0) return t;
		if(!strcmp(label_table[t].name,s)) return -2; /* dup */
	}

	return -1;
}

/* Find	location of	given label.  A	null is	returned if
   label is	not	found; otherwise a pointer to the position
   of the label	is returned.
*/
char *find_label(s)
char *s;
{
	register int t;

	for(t=0; t<NUM_LAB; ++t)
		if(!strcmp(label_table[t].name,s)) return label_table[t].p;
	return '\0'; /*	error	condition */
}

/*
 * EXEC_GOTO
 * The original	version	of this	code only allowed the GOTO line	number
 * to be a liternal	value.	The	code has been altered to permit	the	use	of
 * an expresion	so that	"computed GOTOs" are supported
 */
void exec_goto()
{

	int  line;
	char line_str[16];
	char *loc;

	if( Trace	) PRINTF( "%s: goto\n",	progLineNum( prog )	);

	Lastif.skip_else = 0;			/* reset as	this is	not	an else	statement */

	get_exp( &line,	0	);
	/*
	 * convert the expression	result into	a string for find_label()
	 */
	sprintf( line_str, "%d", line	);
	//printf(	"GOTO found line# '%s'\n", line_str	);
	loc =	find_label(	line_str );
	if(loc=='\0')
		serror(7); /* label	not	defined	*/

	else {
		prog=loc;	/* start program running at	that loc */
		if( tok	==	FINISHED ) {
			/*
			 * the program hasn't really finished...
			 */
			tok	= DELIMITER;		/* fake	we're not at the end of	the	file */
		}
	}
}

/* Initialize the array	that holds the labels.
   By convention, a	null label name	indicates that
   array position is unused.
*/
void label_init()
{
	register int t;

	for(t=0; t<NUM_LAB; ++t) label_table[t].name[0]='\0';
}

/*
 * EXEC_IF
 * The original	version	of this	code only supported	three operators
 * between two expressions.	 The operators were	=, < and >
 * It also required	the	THEN keyword to	be used.
 *
 * This	version	simply calls get_exp() to evaluate the test, as	the
 * new get_exp() includes the comparison operators and Which were
 * extended	to support #, <= and >=
 * THEN	is now optional	to make	the	code compatible	with Palo Alto
 *
 * Oct 2012: added crude support for ELSE (in raspberry	flavour)
 */
void exec_if()
{
	int tst;

	if( Trace ) PRINTF( "%s if\n", progLineNum( prog	) );

	/*
	* keep a track on this if.	We refer to	this if	we encounter ELSE
	*/
	strcpy(	Lastif.line_num,	progLineNum( prog )	);

	get_exp(	&tst, 0	);	/* evaluate	the	test expression	*/
	Lastif.test = tst;	/* save	the	result */
	Lastif.skip_else	= 0;/* reset as	this is	not	an else	statement */

	if( tst	) {
		get_token();
		if( tok	!= THEN	) {
			putback();		/* no THEN,	put	token back to process next */
		}
		return;			/* and process the remainder of	the	line */
	} else	find_eol();		/* skip	this line (find	start of next line)	*/
}

/*
 * EXEC_ELSE
 * execute an else if flavour is raspberry
 */

void exec_else()
{
//	int	tst;
	int	l;
	char	*thisline, *prevline;

	if( Trace ) PRINTF( "%s else\n",	progLineNum( prog )	);
	/*
	* are we skipping else's?
	*/
	if( Lastif.skip_else	)
		find_eol();		/* skip	this line */

	else {
		/*
		 * is	this else connected	to the previous	if?
		 * we	test this by comparing line	numbers
		 */
		thisline = progLineNum(	prog );
		/*
		 * find	the previous line	number
		 */
		prevline = (char *)0;		/* until proved	otherwise... */
		for( l=0;	l<NUM_LAB; l++ ) {
			if( strcmp( label_table[l].name, thisline ) ==	0) {
				if(	l ) {
					prevline	= label_table[l-1].name;
					break;
				}
			}
		}
		if( prevline ==	(char	*)0	) {
			/*
			 *	cannot find	a previous line
			 */
			serror(	33	);			/* else	without	matching if	*/
			return;
		}
		if( strcmp( prevline,	Lastif.line_num	) != 0 ) {
			/*
			 *	previous line of this else isn't if
			 */
			serror(	33	);			/* else	without	matching if	*/
			return;
		}
		/*
		 * was the previous if "true"?	If so, skip this else, if	not, execute else
		 */
		if( Lastif.test	) {
			/*
			 *	skip this else and all that	follow...
			 */
			Lastif.skip_else =	1;
			find_eol();
		}
	}
	return;
}

/*
 * EXEC_VDU
 * execute a vdu if	flavour	is humbug
 */

void exec_vdu()
{
	int	pos, chr;
	int	row, col;
	int	x, y;

	if( Trace ) PRINTF( "%s vdu\n",	progLineNum(	prog ) );
	get_exp(	&pos, 0	);	/* read	screen position	*/

	//printf( "pos=%d\n", pos );
	/* check	for	a following	","	*/
	get_token();
	if( *token != ',' )	serror(0);

	get_exp(	&chr, 0	);	/* read	character value	*/
	//printf( "chr=%d\n", chr );

	if( pos	< 0 || pos >	1024 )
		serror(	0	);

	chr %= 128;

	//printf( "vdu @ %d, chr=%02x\n", pos, chr );

	vdu(	pos, chr );
}

/*
 * EXEC_FOR
 * Execute a FOR loop
 */
void exec_for()
{
	//struct for_stack i;
//	int i;
	int var;
	int value;
	int step;
	int target;
	int will_loop	= 0;

	if( Trace	) PRINTF( "%s for\n", progLineNum( prog	) );
	Lastif.skip_else = 0;			/* reset as	this is	not	an else	statement */

	get_token(); /*	read the control variable	*/
	if(!isalpha(*token)) {
		serror(4);
		return;
	}

	var=toupper( (int)(*token) ) -'A'; /* save its index */

	get_token(); /*	read the equals	sign */
	if(*token!='=')	{
		serror(3);
		return;
	}

	get_exp(&value,	1); /* get initial value */

	variables[var] = value;
	step			 = 1;			/* steps are "1" until changed */

	get_token();
	if(tok!=TO) serror(9); /*	read and discard the TO	*/

	get_exp(&target, 0); /*	get target value */

	get_token();			/* was a STEP specified? */
	//printf(	"FOR: after target, tok=%d, token_type=%d\n", tok, token_type );
	if( tok	== STEP	) {
		get_exp( &step,	0 );				/* read	value */
		if( step == 0 )	 serror( 20 );		/* bad value */
	}

	/* if	loop can execute at	least once,	push info on stack */

	if( step > 0 ) {
		if( target	>= variables[var] )	will_loop =	1;
	} else {
		if( target	<= variables[var] )	will_loop =	1;
	}
	/*
	 * do	we create a	stack frame	for	at least one pass through the loop?
	 * STRICT_FORNEXT	forces at least	one	pass
	 */
	if( STRICT_FORNEXT ||	will_loop ) {
		if( sp+1 == STACK_SIZE	) {
			serror(	10 );
			return;
		}
		stack[sp].frame_type =	STACK_FOR;
		stack[sp].var	  =	var;
		stack[sp].target =	target;
		stack[sp].step	  =	step;
		stack[sp].loc	  =	prog;
		sp++;
		//printf( "pushed FOR onto stack at #%d\n", sp-1 );
	} else {	/* otherwise, skip loop	code altogether	*/
		while(tok!=NEXT) get_token();
		get_token();				/* and eat-up the next variable	name */
	}
}

/*
 * EXEC_NEXT
 * Execute a NEXT statement.
 * In the original version this	code didn't	require	the	loop-variable to
 * be given.  Now it's mandatory.  We check	ditch all FOR-LOOPs	on the stack
 * "below" the corresponding FOR for this NEXT.
 * If we can't find	a corresponding	FOR	on the stack (or before	a GOSUB-frame)
 * then	we throw an	error
 */

void exec_next()
{
	//struct for_stack i;
	int	 varname = -1;

	if( Trace	) PRINTF( "%s next\n", progLineNum(	prog ) );
	Lastif.skip_else = 0;			/* reset as	this is	not	an else	statement */
	/*
	 * was a variable	name given?
	 */
	get_token();
	if( token_type ==	VARIABLE ) {
		/*
		 *	was	was	the	variable name?
		 */
		//printf( "NEXT: varname is %c\n",	*token );
		varname	= toupper(	*token ) - 'A';
	} else {
		/*
		 *	it's now an	error for NEXT not to include it's variable-name
		 *	If the token was EOL, call putback() so	the	correct	line is
		 *	identified by serror()
		 */
		putback();
		serror(	8 );
		return;
	}
	//printf(	"NEXT: var=%d (sp=%d)\n", varname, sp );

	/*
	 * Find	the stack	location corresponding with	the	NEXT var
	 * discard all FOR-frames	which don't	correspond to the variable-name
	 * until you find	a GOSUB-frame or the stack empties.	 This allows the
	 * program to	jump out of	an inner FOR-LOOP and for the FOR-LOOP to be
	 * cleaned-away
	 */
	for( --sp; sp>=0 &&
		stack[sp].frame_type==STACK_FOR	&&
		stack[sp].var != varname; sp--	)
		;
	//printf(	"NEXT: sp now %d\n", sp	);
	if( sp < 0 ||	stack[sp].frame_type ==	STACK_GOSUB	) {
		serror(	11	);
		return;
	}
	/*
	 * found matching	FOR
	 * use the frame to see	if the loop needs	to re-execute
	 */
	//i =	fpop();							/* read	the	loop info */

	variables[stack[sp].var] += stack[sp].step;	/* increment control variable */
	if( stack[sp].step > 0 ) {
		if(variables[stack[sp].var]>stack[sp].target) return;	/* all done	*/
	} else {
		if( variables[stack[sp].var] <	stack[sp].target )	return;
	}
	//fpush(i);  /*	otherwise, restore the info */

	prog = stack[sp++].loc;	 /* loop */
}

/*
 * EXEC_INPUT: Execute a simple	form of	the	BASIC INPUT	command
 * The original	version	of this	routine	lacked the following features of
 * Palo	Alto:
 * - ability to	allow expressions as input
 * - short-form	prompt (used if	a repeat was necessary)
 * - prompt	using the variable name	if no prompt provided
 * differences with	Palo Alto (unlikely	to be resolved)
 * - items must	be separated by	commas (Palo Alto allowed spaces)
 */

void exec_input()
{
	char	str[80], var;
	char	*prompt0 = (char *)0;
	char	*prompt1 = (char *)0;	/* two possible	prompt strings */
	char	varname;
	int	pass = 0;
	int	i;
	void	str_exp();

	if( Trace ) PRINTF( "%s input\n", progLineNum( prog ) );
	Lastif.skip_else	= 0;		 /*	reset as this is not an	else statement */
	get_token();	/* see if prompt string	is present */
	if(token_type==QUOTE) {
		prompt0	=	strdup(	token );

		/* check for a following "," */
		get_token();
		if( *token !=	','	)  serror(0);

		get_token();
		/*
		 * check for a second	prompt string
		 */
		if( token_type==QUOTE	) {
			prompt1	= strdup( token	);

			/*	check for a	following "," */
			get_token();
			if(*token!=',')	serror(0);

			get_token();
		}
	}
	varname	= toupper( *token );
	var = varname -	'A';			/* get the input var */

	strcpy(	str,	"" );			/* clear the input string */
	while( strlen( str )	<= 1 ) {
		pass++;
		if( prompt0 ) {
			if( pass == 1 )
				PRINTF(	prompt0	);
			else {
				if(	prompt1	)  PRINTF( prompt1 );
				else		   PRINTF( prompt0 );
			}
			fflush(	stdout	);
		} else {
			PRINTF(	"%c ",	varname	);
			fflush(	stdout	);
		}
		if( FGETS( str,	sizeof(	str )-1, stdin ) == 0	)
			return;
	}
	/*
	* in the original version of this BASIC	the	code tried to read
	* an integer from stdin.
	* However, Palo	Alto accepts full expressions...
	* scanf("%d", &i);
	*/
	str_exp(	str, &i, 0 );
	variables[var] =	i; /* store	it */

	if( prompt0 )  free(	prompt0	);
	if( prompt1 )  free(	prompt1	);
}

/*
 * EXEC_GOSUB
 * Execute a GOSUB command
 * In the original version of this code, the GOSUB had to have a liternal number
 * To be compatible	with Palo Alto,	we allow the GOSUB line	to be an expression
 * (so-called "computed goto")
 * The code	has	been further modified to use a unified stack
 */
void exec_gosub()
{
	char *loc;
	int  line;
	char line_str[16];

	if( Trace	) PRINTF( "%s gosub\n",	progLineNum( prog )	);
	Lastif.skip_else = 0;			/* reset as	this is	not	an else	statement */
	//printf(	"gosub: token->\"%s\"\n", token	);
	/* find	the label	to call	*/
	get_exp( &line,	0	);
	/*
	 * convert the expression	result into	a string for find_label()
	 */
	sprintf( line_str, "%d", line	);
	loc =	find_label(	line_str );
	if(loc=='\0')
		serror(7); /* label	not	defined	*/
	else {
		if(	sp+1 ==	STACK_SIZE ) {
			serror(	10 );			/* stack full */
			return;
		}
		stack[sp].frame_type = STACK_GOSUB;
		stack[sp].loc		 = prog;
		sp++;
		//printf( "pushed GOSUB onto stack at #%d\n", sp-1 );

		prog = loc;	 /*	start program running at that loc */
	}
}

/*
 * EXEC_RETURN
 * Return from GOSUB
 */
void exec_return()
{
	/*
	* decrement	sp and check its a GOSUB frame
	* keep decrementing	sp until we	find a GOSUB frame or the stack	empties
	*/

	if( Trace ) PRINTF( "%s return\n", progLineNum(	prog	) );
	Lastif.skip_else	= 0;		 /*	reset as this is not an	else statement */
	//printf( "RETURN: sp=%d\n",	sp );
	while( --sp && stack[sp].frame_type != STACK_GOSUB )
		;
	if( sp <	0 ) {
		serror(	13 );		/* RETURN without GOSUB	*/
		return;
	}
	//printf( "recovered GOSUB frame from stack #%d\n", sp );
	/*
	* sp points	to the GOSUB frame.	 Grab the return address
	* then remove the frame	from the stack
	*/
	prog	= stack[sp].loc;
	return;
}

/* SERROR: display an error	message
 * where the error has occured.
 * error is	a small	integer	which is printed in	english	by this	function
 * In the original version of the code this	function just printed the error.
 * The code	has	been extended to print the line	affected, the position in the
 * line	where the error	has	occured	and	to support more	error messages.
 */

void serror( int error )
{
	static char *e[]=	{
		/* 0 */	"syntax error",
		/* 1 */	"unbalanced parentheses",
		/* 2 */	"no expression present",
		/* 3 */	"syntax error (or equals sign expected)",
		/* 4 */	"not a variable",
		/* 5 */	"too many lines",
		/* 6 */	"duplicate line number",
		/* 7 */	"no line with that number",
		/* 8 */	"NEXT without variable-name",		/* was "THEN expected" */
		/* 9 */	"TO expected",
		/* 10 */ "too many nested FOR loops or GOSUBs",		/* stack overflow in vanilla flavour */
		/* 11 */ "NEXT without matching FOR",
		/* 12 */ "too many nested GOSUBs",
		/* 13 */ "RETURN without GOSUB",
		/* 14 */ "negative index with @()",
		/* 15 */ "non existing array value",
		/* 16 */ "division by zero",
		/* 17 */ "DRAW parameter missing",
		/* 18 */ "DRAW start not called",
		/* 19 */ "bad colour name",
		/* 20 */ "STEP must not be zero",
		/* 21 */ "variable already in-use by previous FOR",
		/* 22 */ "WAIT parameter missing",
		/* 23 */ "command not permitted in current flavour",
		/* 24 */ ""
		/* 25 */ "WHILE without corresponding WEND",
		/* 26 */ "WEND without matching WHILE",
		/* 27 */ "REPEAT without corresponding UNTIL",
		/* 28 */ "UNTIL without matching REPEAT",
		/* 29 */ "too many nested loops or GOSUBs",	/* stack overflow in raspberry flavour */
		/* 30 */ "CONTINUE outside FOR, WHILE or REPEAT loop",
		/* 31 */ "BREAK outside FOR, WHILE or REPEAT loop",
		/* 32 */ "TRACE requires ON/OFF option",
		/* 33 */ "ELSE without matching IF"
	};
	char *lp;
	int  err_point = 0;	/* exact error point from  start of	the	line */

	/*
	 * figure-out	the	start of the current line in order to print	it
	 * start with	a pointer to be	start of the current token
	 */
	lp = cur_token;

	if( *token ==	'\n' )	lp--;
	if( *lp	== '\n'	) {
		/*
		 *	the	current	pointer	is at a	line-end, the error	occurs on this line
		 */
		//printf( "serror: pointing to \\n\n" );
		lp--;
	}
	for( ; *lp!='\n' && lp>p_buf;	lp--, err_point++ )
		;
	if( lp > p_buf )	lp++;
	/*
	 * lp	now	points to the start	of the current line
	 */
	PRINTF(	"\n" );				/* start error message on new line */
	while( *lp &&	*lp	!= '\n'	)  PRINTF( "%c", *lp++ );
	PRINTF(	"\n" );
	for( ; err_point > 1;	err_point--	) PRINTF( " " );
	PRINTF(	"^\n%s\n", e[error] );
	closeScreen( "Program Error" );
	longjmp(e_buf, 1);			/* return to save point	*/
}

/*
 * GET_TOKEN: Get a	token.
 * This	function has been changed from the original	as follows:
 * - quoted	strings	can	be 'single'	or "double"
 * - the ^ character is	no longer a	delimiter
 * - the code now assumes lines	end	in \n and not \r\n
 * - code supports ARRAY and FUNCTION types
 * - now sets global tok to	DELIMITER when current token is	a DELIMITER
 *	 (was this a bug?)
 * Take	a copy of *prog	before parsing the input, which	is used	in
 * printing	error diagnostics
 * $$
 * AFL,	August 2012
 */

int get_token()
{
	register char	*temp;
//	char bitbuff[20];		/* an extract from the main	buffer */
	char **funcp;
	char match_quote;

	token_type=0;
	tok=0;
	temp=token;

	if(*prog=='\0')	{	/* end of file */
		*token=0;
		tok	= FINISHED;
		//printf( "FINISHED found!\n" );
		return(token_type=DELIMITER);
	}

	/*
	 * show	a	snippet	of the buffer ahead
	strncpy( bitbuff,	prog, 10 );
	bitbuff[10] =	'\0';
	printf(	"get_token(): examining \"%s...\": ",	bitbuff	);
	 */

	while(iswhite(*prog))	++prog;	 /*	skip over white	space */

	if(*prog=='\n')	{	/* newline */
		prog++;
		tok	= EOL;
		*token='\n';
		token[0]='\n';
		token[1]=0;
		//printf( "NEWLINE\n" );
		return (token_type = DELIMITER);
	}

	if(strchr("+-*/%=#;(),><", *prog)) { /*	delimiter */
		*temp=*prog;
		prog++;	/* advance to next position	*/
		temp++;
		*temp=0;
		//printf( "DELIM\n"	);
		return (token_type=DELIMITER);
	}

	/*
	 * now that we've	skipped	spaces and delimiters, take	a copy of *prog
	 * for improved	error	reporting
	 */
	cur_token	= prog;

	if(*prog=='"' || *prog == '\'') { /* quoted string */
		match_quote	= *prog;
		prog++;
		while( *prog !=	match_quote	&& *prog!='\n')	*temp++=*prog++;
		if(*prog=='\n')	serror(1);
		prog++;
		*temp=0;
		//printf( "QUOTE\n"	);
		return(token_type=QUOTE);
	}

	if(isdigit( (int)(*prog))) { /*	number */
		while(!isdelim(*prog)) *temp++=*prog++;
		*temp =	'\0';
		//printf( "TT:NUMBER\n" );
		return(token_type =	NUMBER);
	}

	if(isalpha( (int)(*prog))	|| *prog=='@') {
		/* var or command */
		while(!isdelim(*prog))
			*temp++=*prog++;
		token_type=STRING;
	}

	*temp	= '\0';

	/* see if	a string is	a command or a variable	*/
	if(token_type==STRING) {
		/* can we find this	string in the command table? */
		//printf( "checking the command table for \"%s\"\n", token );
		tok=look_up(token);
		if(!tok) {
			/* no.	Is it an	array? */
			if( *token == '@' )
				token_type = ARRAY;
			else {
				/* no.	Is it	a function?	*/
				for( funcp=functions;	*funcp && strcasecmp( token, *funcp	); funcp++ )
					;
				if( *funcp ==	(char *)0 ) {
					/*	no.	must be	a variable */
					token_type	= VARIABLE;
				} else
					token_type	= FUNCTION;
			}
		} else
			token_type =	COMMAND; /*	is a command */
	}
	//printf(	"TT:%d\n",	token_type );
	return token_type;
}



/* PUTBACK:	Return a token to input	stream.	*/
void putback()
{
	char *t;

	t	= token;
	for(;	*t;	t++) prog--;
}

/*
 * LOOKUP
 * Look	up a token's internal representation in	the	command	token table.
 */
int look_up(char *s)
{
	register int i;	//,j;
	char *p;

	/* convert to	lowercase */
	p	= s;
	while(*p) {
		*p	= tolower( (int)(*p) );
		p++;
	}

	/* see if	token is in	table */
	for(i=0; *table[i].command; i++) {
		if(!strcmp(table[i].command, s)) {
			/*
			* match	with table entry 'i'
			* check	the	command	can	be used	in the current flavour
			*/
			if( (table[i].flavours &	Flavour) ==	0 )
				serror(	23 );			/* wrong flavour */

			return table[i].tok;
		}
	}
	return 0;	/* unknown command */
}

/* Return true if c	is a delimiter.	*/
/* AFL:	added the hash-mark	as this	now	means "not equal to" */
int isdelim(char c)
{
	if(strchr(" ;,+-<>/*%^=#()", c)	|| c==9	|| c=='\n' ||	c==0)
		return 1;
	return 0;
}

/* Return 1	if c is	space or tab. */
int iswhite(char c)
{
	if(c==' ' || c=='\t')	return 1;
	else return 0;
}

/*
 * COMPARE
 * Given an	operator and two values, return	1 (true) or	0 (false)
 */

void compare( char op, int *left, int *right )
{
	switch(	op ) {
	case	'=':
		*left	= (*left ==	*right);
		break;
	case	'#':
		*left	= (*left !=	*right);
		break;
	case	'<':
		*left	= (*left < *right);
		break;
	case	'>':
		*left	= (*left > *right);
		break;
	case	'l':
		*left	= (*left <=	*right);
		break;
	case	'g':
		*left	= (*left >=	*right);
		break;
	}
	return;
}

/*
 * STR_EXP
 * Like	get_exp() but parse	the	given string expression
 */

void str_exp( char *s, int *val, int isassign )
{
	char	*keep;

	/*
	* take a copy of the program pointer as	get_exp() uses globals...
	*/
	keep	= prog;
	prog	= s;
	get_exp(	val, isassign );
	prog	= keep;			/* restore the previous	value of "prog"	*/
	return;
}

/*
 * GET_EXP
 * Entry point into	expression parser
 * The original	parser implemented the following precedence	order
 * level2 +,-
 * level3 *,/,%
 * level4 ^	(raise to the power)
 * level5 unary	+,-
 * level6 parenthesis
 *
 * AFL has altered this	to the following
 * level2: (comparison operators) #, = (see	below),	<. >, <=, >=
 * level3: +, -
 * level4: *, /, %
 * level5: unary +,	-
 * level6: functions (rnd()	and	abs()) and array @()
 * level7: parenthesis
 *
 * BIG HACK:
 * Because "=" looks like both an assignment or	an equality-test, calling
 * get_exp() must be told which	to use.	 If	isassign is	true, then "="
 * is treated as a delimiter of	an expression.	Thus for an	assignment rvalue
 * isassign	is set true	and	for	most other cases it's set false
 */

void get_exp( int *result, int isassign	)
{
	get_token();
	//printf(	"isassign=%d, token->'%s'\n", isassign,	token );

	if(!*token) {
		serror(2);			/* oops	no expression */
		return;
	}
	level2(result, isassign);
	putback(); /*	return last	token read to input	stream */
}

/* L2: comparison operators	*/

void level2( int *result, int isassign )
{
	register	char  op;
	int	hold;

	//printf( "in L2...\n" );
	level3(	result,	isassign	);
	//printf( "L2: checking for comparison...\n"	);
	if( isassign	&& *token == '=' ) {
		//printf(	"L2: this is assignment!\n"	);
		//printf(	"L2: before putback prog -> '%c'\n", *prog );
		//putback(), putback();
		//printf(	"L2: after 2 putbacks prog -> '%c'\n", *prog );
		return;
	}
	while((op = *token) == '#' || op	== '=' || op ==	'<'	|| op == '>') {
		//printf(	"L2: op='%c'\n", op	);
		if( op ==	'<'	|| op == '>' ) {
			/*
			 *	we need	to look	at the next	character to check for <= or >=
			 */
			get_token();
			//printf( "L2: op='%c', *token='%c'\n",	op, *token	);
			if( *token	!= '=' )
				putback();
			else {
				if(	op == '<' )	 op	= 'l';			/* short for "less than or equal */
				else if( op	== '>' )  op = 'g';		/* short for "greater than or equal */
			}
		}
		get_token();
		//printf(	"L2: calling L3...\n" );
		level3(	&hold, isassign	);
		//printf(	"L2: about to call compare(%c, %d, %d)...\n", op, *result, hold	);
		compare( op, result, &hold );
		//printf(	"L2: compare was %d\n",	*result	);
	}
}


/*	Add	or subtract	two	terms. */
void level3( int *result, int isassign )
{
	register char	 op;
	int hold;

	//printf(	"in L3...\n" );
	level4(	result,	isassign );
	//printf(	"L3: checking for +-\n"	);
	while((op	= *token) == '+' ||	op == '-') {
		//printf( "L3: token='%c'\n", op );
		get_token();
		//printf( "L3: calling L4...\n"	);
		level4(&hold, isassign);
		arith(op, result, &hold);
	}
}

/* Multiply	or divide two factors. */
void level4( int *result, int isassign )
{
	register char	 op;
	int hold;

	//printf(	"in L4...\n" );
	level5(result, isassign	);
	//printf(	"L4: checking for */%\n" );
	while((op	= *token) == '*' ||	op == '/' || op	== '%')	{
		//printf( "L4: token='%c'\n", op );
		get_token();
		//printf( "L4: calling L5...\n"	);
		level5(&hold, isassign );
		arith(op, result, &hold);
	}
}

/* Is a	unary +	or -. */
void level5( int *result, int isassign )
{
	register char	 op;

	//printf(	"in L5: token is '%c'...\n", *token	);
	op = 0;
	if((token_type==DELIMITER) &&	*token=='+'	|| *token=='-')	{
		op = *token;
		get_token();
	}
	level6(result, isassign);
	//printf(	"L5: checking for unary op\n" );
	if(op)
		unary(op, result);
}

/* Is it a function	or array? */

void level6( int *result, int isassign )
{
	char	f[16];

	//printf( "L6: token=\"%s\"\n",	token );
	if( strcasecmp(	token, "rnd"	) == 0 ||
		strcasecmp( token, "abs"	) == 0 ||
		*token == '@' )	{
		strncpy( f, token, sizeof( f )-1 );
		get_token();
	} else
		strcpy(	f, ""	);

	level7(	result,	isassign	);
	if( strlen( f )	) {
		//printf(	"L6: envaluating function/array %s(%d)\n", f, *result );
		if( strcasecmp(	f, "rnd" ) ==	0 )
			*result	= (rand() % *result)+1;
		else if( strcasecmp( f,	"abs"	) == 0 )
			*result	= abs(	*result	);
		else if( *f == '@' )
			arrayRetrieve(	*result, result	);

		//printf(	"L6 returning.  *token->'%c'\n", *token	);
		return;
	}
}

/* Process parenthesized expression. */
void level7( int *result, int isassign )
{
	//printf(	"in L7...token='%c'\n",	*token );
	if((*token ==	'(') &&	(token_type	== DELIMITER)) {
		get_token();
		//printf( "L7: found '(' calling L2...\n" );
		level2(result, isassign);
		//printf( "L7: got result=%d\n", *result );
		if(*token != ')')
			serror(1);
		get_token();
	} else {
		primitive(result);
		//printf( "L7: primitive result=%d\n", *result );
	}
}

/* PRIMITIVE: Find value of	number or variable.	*/
void primitive(result)
int	*result;
{
//	int	exp;
//	char *f;

	//printf(	"primitive: token_type=%d, token->\"%s\"\n", token_type, token );
	switch(token_type) {
	case VARIABLE:
		*result	= find_var(token);
		get_token();
		return;
	case NUMBER:
		*result	= atoi(token);
		get_token();
		return;
	default:
		serror(0);
	}
}

/* Perform the specified arithmetic. */
void arith(o, r, h)
char o;
int	*r,	*h;
{
	register int t,	ex;

	switch(o)	{
	case '-':
		*r = *r-*h;
		break;
	case '+':
		*r = *r+*h;
		break;
	case '*':
		*r = *r	*	*h;
		break;
	case '/':
		*r = (*r)/(*h);
		break;
	case '%':
		t	= (*r)/(*h);
		*r = *r-(t*(*h));
		break;
	case '^':
		ex = *r;
		if(*h==0)	{
			*r = 1;
			break;
		}
		for(t=*h-1; t>0; --t)	*r = (*r) *	ex;
		break;
	}
}

/* Reverse the sign. */
void unary(o, r)
char o;
int	*r;
{
	if(o=='-') *r	= -(*r);
}

/* Find	the	value of a variable. */
int	find_var(char *s)
{
	if(!isalpha( (int)(*s))	) {
		serror(4); /* not a	variable */
		return 0;
	}
	return variables[toupper( (int)(*token)) - 'A' ];
}

/*
 * PROGLINENUM
 * Given a program position	address	(*prog)	find the label (line number)
 * of this statement
 */

char *progLineNum( char	*p )
{
	int i;

	for(	i=0; i<NUM_LAB && label_table[i].p && label_table[i].p < p;	i++	)
		;
	//printf( "proglineNum: i=%d\n",	i );
	if( i ==	NUM_LAB	)  return( label_table[	NUM_LAB-1 ].name );
	else	if(	i == 0 )   return( label_table[0].name );
	else				   return( label_table[i-1].name );
}

/*
 * EXEC_TRACE
 * Enable or disable tracing of	execution
 */

void exec_trace()
{
	if( Trace ) PRINTF( "%s trace\n", progLineNum( prog ) );
	Lastif.skip_else	= 0;		 /*	reset as this is not an	else statement */
	tok = get_token();
	if( tok	!= VARIABLE ) {
		printf(	"1\n"	);    /* trace on/off	expected */
		serror(	32 );
		return;
	}

	if( strcasecmp(	token, "on" ) ==	0 )		   Trace = 1;
	else	if(	strcasecmp(	token, "off" ) == 0	)  Trace = 0;
	else	{
		serror(	32 );
	}
	return;
}

/*
 * WHILE/WEND
 * Implements a	simple while loop.	Only permitted in "raspberry" flavour
 *
 * EXEC_WHILE
 * Evaluate	the	test;
 *	 If	true, save our position	on the stack in	a "while" frame	then continue
 *	 to	execute
 *	 If	false scan through the tokens looking for the corresponding	WEND
 */

void exec_while()
{
	char	*loc;
	int	 tst;
//	int	 found_wend	= 0;
	int	 while_depth = 0;

	if( Trace ) PRINTF( "%s while\n", progLineNum( prog ) );
	Lastif.skip_else	= 0;		 /*	reset as this is not an	else statement */

	//printf( "while (entry) sp=%d\n", sp );
	/*
	* we need the location of the start	of the while token,
	* so adjust	the	prog pointer by	the	length of "while"
	*/
	loc = (prog - strlen( "while" ));

	get_exp(	&tst, 0	);	/* evaluate	the	test expression	*/

	if( tst	) {
		/*
		 * while loop	will run at	least once
		 * push	a	while frame	onto the stack
		 */
		if( sp+1 == STACK_SIZE ) {
			serror(	29	);			/* stack full in raspberry flavour */
			return;
		}
		stack[sp].frame_type = STACK_WHILE;
		stack[sp].loc		   = loc;
		sp++;
	} else {
		/*
		 * test	failed,	so go	find the corresponding WEND...
		 */
		while( 1 ) {
			/*	scan input looking for commands	*/
			do {
				get_token();
				if(	tok	== FINISHED	) {
					serror(	25 );
					return;
				}
			} while( token_type !=	COMMAND	);
			/*
			 *	deal with nested while loops
			 */
			if( tok	==	WHILE )	{
				while_depth++;
				continue;
			}
			if( tok	==	WEND ) {
				while_depth--;
				if(	while_depth	< 0	)
					break;
			}
		}
	}
	//printf( "while (exit) sp=%d\n", sp	);
}

/*
 * EXEC_WEND
 * Called when WEND	is executed
 * pop frames off the stack	looking	for	the	last WHILE frame.
 * Discard frames removed.
 * When	we find	the	WHILE frame, we	"goto" the start of	the	WHILE statement
 */

void exec_wend()
{
	if( Trace ) PRINTF( "%s wend\n",	progLineNum( prog )	);
	Lastif.skip_else	= 0;		 /*	reset as this is not an	else statement */

	//printf( "wend (entry) sp=%d\n", sp	);
	/*
	* pop frames from the stack, looking for last WHILE
	*/
	for(	--sp; sp>=0	&&
		stack[sp].frame_type !=	STACK_WHILE &&
		stack[sp].frame_type !=	STACK_GOSUB;
		sp-- )
		;
	if( sp <	0 || stack[sp].frame_type == STACK_GOSUB ) {
		serror(	26 );		/* WEND	without	matching WHILE */
		return;
	}
	prog	= stack[sp].loc;  /* go	back to	the	WHILE statement	*/
	//printf( "wend (exit) sp=%d\n",	sp );
}

/*
 * EXEC_CONTINUE
 * Implements the continue (re-execute)	a loop
 * pop stack objects looking for a FOR,	WHILE or REPEAT	loop,
 * discarding other	frames
 * goto	the	loop re-evaluation statement.
 * For FOR/NEXT	loops, this	is FOR,	for	WHILE/WEND this	is WHILE
 * and for REPEAT/UNTIL	this is	UNTIL
 */

void exec_continue()
{
	int repeat_depth	= 0;

	if( Trace ) PRINTF( "%s continue\n",	progLineNum( prog )	);
	Lastif.skip_else	= 0;		 /*	reset as this is not an	else statement */

	/*
	* examine frames from the stack, looking for last WHILE	or REPEAT
	* stop if the stack	is exhausted or	we find	a GOSUB	frame
	*/
	for(	--sp; sp>=0	&&
		stack[sp].frame_type !=	STACK_FOR	&&
		stack[sp].frame_type !=	STACK_WHILE &&
		stack[sp].frame_type !=	STACK_REPEAT &&
		stack[sp].frame_type !=	STACK_GOSUB;
		sp-- )
		;
	if( sp <	0 || stack[sp].frame_type == STACK_GOSUB ) {
		serror(	30 );		/* CONTINUE	outside	FOR/WHILE/REPEAT */
		return;
	}
	if( stack[sp].frame_type	== STACK_WHILE ||
		stack[sp].frame_type	== STACK_FOR )
		prog = stack[sp].loc;	 /*	go back	to the loop	start */
	else {
		/*
		 * scan	forward	to find	the corresponding	UNTIL
		 */
		while( 1 ) {
			do {
				get_token();
				if(	tok	== FINISHED	) {
					serror(	30 );	/* oops, no	UNTIL */
					return;
				}
			} while( token_type !=	COMMAND	);
			/*
			 *	deal with nested repeat/until loops
			 */
			if( tok	==	REPEAT ) {
				repeat_depth++;
				continue;
			}
			if( tok	==	UNTIL )	{
				repeat_depth--;
				if(	repeat_depth < 0 ) {
					putback();				/* we want to execute this UNTIL */
					/*
					* before we	go and execute the UNTIL, put the REPEAT/UNTIL
					* frame	back on	the	stack
					*/
					sp++;
					return;
				}
			}
		}
	}
}

/*
 * EXEC_BREAK
 * Implements break	from innermost FOR,	WHILE or REPEAT	loop
 * pop stack objects looking for FOR, WHILE	or REPEAT loop,	discarding frames
 * then	scan forward to	find WEND or UNTIL,	which both must	be skipped
 */

void exec_break()
{
	int while_depth	= 0;
	int repeat_depth	= 0;
	int for_depth =	0;
	int i;

	if( Trace ) PRINTF( "%s break\n", progLineNum( prog ) );
	Lastif.skip_else	= 0;		 /*	reset as this is not an	else statement */
	/*
	* examine frames from the stack, looking for last WHILE	or REPEAT
	* stop if the stack	is exhausted or	we find	a GOSUB	frame
	*/
	for(	--sp; sp>=0	&&
		stack[sp].frame_type !=	STACK_FOR	&&
		stack[sp].frame_type !=	STACK_WHILE &&
		stack[sp].frame_type !=	STACK_REPEAT &&
		stack[sp].frame_type !=	STACK_GOSUB;
		sp-- )
		;
	if( sp <	0 || stack[sp].frame_type == STACK_GOSUB ) {
		serror(	31 );		/* BREAK without matching FOR/WHILE/REPEAT */
		return;
	}
	/*
	* skip over	code looking for the end of	the	loop...
	*/
	while( 1	) {
		/* scan	input	looking	for	commands */
		do {
			get_token();
			if( tok	==	FINISHED ) {
				serror(	31 );
				return;
			}
		}	while( token_type != COMMAND );
		switch(	tok ) {
		case FOR:
			for_depth++;
			break;
		case WHILE:
			while_depth++;
			break;
		case REPEAT:
			repeat_depth++;
			break;
		case NEXT:
			for_depth--;
			if( stack[sp].frame_type == STACK_FOR )	{
				if(	for_depth <	0 ) {
					get_token();		/* eat up the loop variable	*/
					return;
				}
			}
			break;
		case WEND:
			while_depth--;
			if( stack[sp].frame_type == STACK_WHILE	) {
				if(	while_depth	< 0	) {
					return;
				}
			}
			break;
		case UNTIL:
			repeat_depth--;
			if( stack[sp].frame_type == STACK_REPEAT ) {
				if(	repeat_depth < 0 ) {
					get_exp(	&i,	0 );  /* eat up	the	expression to discard it */
					return;
				}
			}
			break;
		}
	}
}

/*
 * EXEC_REPEAT
 * Create a	stack frame	to allow UNTIL to return here
 */

void exec_repeat()
{
	char	*loc;

	if( Trace ) PRINTF( "%s repeat\n", progLineNum(	prog	) );
	Lastif.skip_else	= 0;		 /*	reset as this is not an	else statement */

	/*
	* we need the location of the start	of the repeat token,
	* so adjust	the	prog pointer by	the	length of "repeat"
	*/
	loc = (prog - strlen( "repeat" ));

	/*
	* repeat loop will run at least	once push a	repeat frame onto the stack
	*/
	if( sp+1	== STACK_SIZE )	{
		serror(	29 );				/* stack full in raspberry flavour */
		return;
	}
	stack[sp].frame_type	= STACK_REPEAT;
	stack[sp].loc		= loc;
	sp++;
}

/*
 * EXEC_UNTIL
 * pop frames off the stack	looking	for	corresponding REPEAT
 * Evaluate	the	test.  If false, go	back to	REPEAT,	else continue
 */

void exec_until()
{
	int tst;

	if( Trace ) PRINTF( "%s until\n", progLineNum( prog ) );
	Lastif.skip_else	= 0;		 /*	reset as this is not an	else statement */
	/*
	* pop frames from the stack, looking for last REPEAT
	*/
	for(	--sp; sp>=0	&&
		stack[sp].frame_type !=	STACK_REPEAT &&
		stack[sp].frame_type !=	STACK_GOSUB;
		sp-- )
		;
	if( sp <	0 || stack[sp].frame_type == STACK_GOSUB ) {
		serror(	28 );		/* UNTIL without REPEAT	*/
		return;
	}
	/*
	* evaluate the test...
	*/
	get_exp(	&tst, 0	);
	if( !tst	) {
		prog = stack[sp].loc;	 /*	go back	to the REPEAT statement	*/
	}
	return;
}

