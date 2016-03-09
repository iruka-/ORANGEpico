#ifndef _ps2keyb_h_
#define _ps2keyb_h_

#include "utype.h"

void  kbd_init(void);			// : 初期化
uchar kbd_getchar(void);		// : ASCII コードを取得.
uchar kbd_peekcode(void);		// : FIFOキューのTOPを見る.
uchar kbd_getcode(void);		// : FIFOキューの取り出し操作.
uchar kbd_getpress(uchar c);	// : リアルタイム 任意キーの押下情報 の取得.

void  _MIPS32 kbd_int_handler(void);

#endif	//_ps2keyb_h_
