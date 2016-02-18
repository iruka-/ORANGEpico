/*
 *	シンボル検索結果を入れる構造体
 */

typedef struct {
	long   adrs;		/* 論理アドレス  */
	char  *name;		/* シンボル名	 */
} SYMBOL;

#define	Ulong	unsigned long
int	find_symbol_by_name(SYMBOL *symbol,char *name  );
int	find_symbol_by_adrs(SYMBOL *symbol,long address);

#define	SYMLEN	48		/* シンボル名の長さの最大長 */
