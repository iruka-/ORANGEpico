/****************************************************************
 *	■ fifo ■ 先入れ先出し
 ****************************************************************
int FIFO_init(   FIFO *queue,FIFO_t *buf,int size);	//初期化.
int FIFO_enqueue(FIFO *queue,FIFO_t *buf,int size);	//キューにためる.
int FIFO_dequeue(FIFO *queue,FIFO_t *buf,int size);	//キューから取り出す.
int FIFO_getsize(FIFO *queue);					//キューにたまっている要素数を得る.
int FIFO_getfreesize(FIFO *queue);				//キューの空き容量要素数を得る.
 */
#include "fifo.h"

#define	_BOOTROM_  __attribute__((section(".bootrom")))

/****************************************************************
 *	ワーク
 ****************************************************************
#define	RXBUF_SIZE	1024			// 受信FIFOサイズ.
static uchar rxbuf[RXBUF_SIZE];	// 受信FIFO.
 */
/****************************************************************
 *	初期化
 ****************************************************************
 */
int FIFO_init( FIFO *queue,FIFO_t *buf,int size)
{
	queue->buf = buf;
	queue->size = size;
	queue->inPtr = 0;
	queue->outPtr = 0;
	return 0;
}
#if	0
int FIFO_reset( FIFO *queue )
{
	queue->outPtr = queue->inPtr;
	return 0;
}
#endif
/****************************************************************
 *	先入れ
 ****************************************************************
 *	引数：
 *			queue   : FIFOキュー
 *			data  : 詰め込むデータ.
 *			size  : 詰め込む要素数.
 *	戻り値：詰め込みに成功した要素数. (0 もしくは size)
 *			キューに充分な空きがないときは詰め込みはキャンセル.
 */
int FIFO_enqueue(FIFO *queue,FIFO_t *data,int size)
{
	int i;
	int n = queue->inPtr;				// FIFOの書き込みポインタ.
	for(i=0; i<size; i++) {
		queue->buf[n] = *data++;
		n++;
		if(	n >= queue->size) {
			n = 0;
		}
		// FIFO取り出しポインタに追いついたら、ポインタを更新しないでキャンセル.
		if(n == queue->outPtr) {
			return 0;
		}
	}
	queue->inPtr = n;
	return i;
}
/****************************************************************
 *	溜まっている要素数を返す.
 ****************************************************************
 *	戻り値： queue に溜まっている要素数
 */
int FIFO_getsize(FIFO *queue)
{
	int n = queue->inPtr - queue->outPtr;
	if(n<0) {
		n = n + queue->size;
	}
	return n;
}
/****************************************************************
 *	空いている要素数を返す.
 ****************************************************************
 *	戻り値： queue の空いている要素数
 */
int FIFO_getfreesize(FIFO *queue)
{
	return queue->size - FIFO_getsize(queue);
}
/****************************************************************
 *	後出し
 ****************************************************************
 *	引数：
 *			queue   : FIFOキュー
 *			result: 取り出したデータの受け取り場所.
 *			size  : 取り出したい要素数.
 *	戻り値：取り出しに成功した要素数. (0 .. size)
 */
int FIFO_dequeue(FIFO *queue,FIFO_t *result,int size)
{
	int	n = queue->outPtr;					// FIFOの取り出しポインタ.
	int	i;
	for(i=0; i<size; i++) {
		if( n == queue->inPtr ) break;		// FIFOのトップに追いついた.
		result[i] = queue->buf[n];			// 受信文字をコピーする.
		n++;
		if(	n >= queue->size) {				// 巡回バッファ処理.
			n = 0;
		}
	}
	queue->outPtr = n;						// FIFOの取り出しポインタ更新.
	return i;
}

/******************************************************************
 *
 ******************************************************************
 */
