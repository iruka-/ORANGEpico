
	/* portlist.h */

#ifndef	_portlist_h_
#define	_portlist_h_

#define	_SFR_IO8( p )   (0x20+(p)),1
#define	_SFR_IO16( p )  (0x20+(p)),2
#define	_PIC_IO8( p )   (p),1
#define	_PIC_IO8E( p )  (p),(1|EX_PORT)
#define	_PIC_IO8a( p )  (p),4				// Alias

#define	QQ		0x10	//良く参照するポートに印を付ける.
#define	EX_PORT 0x20	//18F4550で拡張されているポート.



#define	A_uint8_t	0x0001
#define	A_uint16_t	0x0002
#define	A_uint32_t	0x0004
#define	A_MASK		0x0007

#define	A_char		0x0001
#define	A_short		0x0002
#define	A_long		0x0004

#define	A__I		0x0100
#define	A__O		0x0200
#define	A__IO		0x0300
#define	A_CAN_TxMailBox_TypeDef		4
#define	A_CAN_FIFOMailBox_TypeDef		4
#define	A_CAN_FilterRegister_TypeDef		4

typedef	struct {
	char *name;
	int   adrs;
	int	  attr;	// bit0:8bit , bit1:16bit ,bit4:quickviewflag
} PortList;


/* portlist.c */
int str_casecmpx (char *s, char *t, int *pmask);

//	ポート名称(小文字を許容する)を番地に変換する.
int portAddress (char *s);

//	ポート名称(小文字を許容する)を番地に変換する.(bit指定も許容する)
int portAddress_b (char *s, int *pmask);

//	番地をポート名称(小文字を許容する)に変換する.
char *GetPortName(int adrs);

void PrintPortNameList (void);
char *radix2str (char *buf, int d);
void PrintPortColumn (void);
void PrintPortAll (int mask);

int	InitPortList(int dev_id);

//	ポート名称(小文字を許容する)を正しい名称に変換.
char *realPortName(char *s);
//	ポート名称(小文字を許容する)からポート属性値(アクセス幅など)を得る.
int getPortAttrib(char *s);

//	ポート名称(小文字や部分文字列を許容する)を正しい名称に変換.
char *suggestPortName(char *s);

//	ポートの型情報(A_uint8_t～A_uint32_tまでのenum)を、アクセス幅(バイトサイズ)に変換.
int	type2size(int type);

#endif	//_portlist_h_

