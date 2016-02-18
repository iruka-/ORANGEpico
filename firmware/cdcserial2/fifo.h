#ifndef	_fifo_h_
#define	_fifo_h_

#define	_MIPS32 __attribute__((nomips16,noinline))


typedef	char FIFO_t;	// FIFOに溜めるデータの型.

/****************************************************************
 *	■ fifo ■ 先入れ先出し
 ****************************************************************
 *	
 */
typedef	struct {
	FIFO_t *buf;			// FIFOバッファ.
	int    size;		// FIFOバッファのサイズ.
	int    inPtr;		// 詰め込みポインタ. (0 .. size-1)
	int    outPtr;		// 取り出しポインタ. (0 .. size-1)
} FIFO;

int FIFO_init(   FIFO *queue,FIFO_t *buf,int size);	//初期化.
int FIFO_enqueue(FIFO *queue,FIFO_t *buf,int size);	//キューにためる.
int FIFO_dequeue(FIFO *queue,FIFO_t *buf,int size);	//キューから取り出す.
int FIFO_getsize(FIFO *queue);					//キューにたまっているバイト数を得る.
int FIFO_getfreesize(FIFO *queue);				//キューの空き容量バイト数を得る.

#define	FIFO_reset( queue )	(queue)->outPtr=(queue)->inPtr

/****************************************************************
 *	
 ****************************************************************
 */

#endif
