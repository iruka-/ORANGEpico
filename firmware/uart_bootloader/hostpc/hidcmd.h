//
//	HIDcmd
//
//======================================================================
//	基本ポリシー
//		共通コマンド 0x00～0x0f
//		PICライター  0x10～0x17
//		JTAGライター 0x18～0x1f
//		AVRライター  0x20～0x2f
//	LSB=1(奇数)コマンドは、基本的に返答パケットを返す.
//======================================================================

//	共通コマンド
#define HIDASP_TEST           0x01	//接続テスト

#define	HIDASP_SAMPLING		  0x03	//サンプリングの実行.
#define HIDASP_POKE           0x04	//メモリー書き込み.
#define HIDASP_PEEK           0x05	//メモリー読み出し.
#define HIDASP_JMP	          0x06	//指定番地のプログラム実行.
#define HIDASP_SET_MODE       0x07	//データ引取りモードの指定(00=poll c0=puts c9=exit)
#define HIDASP_PAGE_ERASE	  0x08  //Flash消去.
#define HIDASP_KEYINPUT		  0x09	//ホスト側のキーボードを送る.
#define HIDASP_PAGE_WRITE     0x0a	//Flash書込.

#define HIDASP_USER_CMD		  0x0c	//ユーザー定義のコマンドを実行する.
#define HIDASP_GET_STRING	  0x0d	//文字列を引き取る.
#define HIDASP_FLASH_LOCK     0x0e	//ARM FlashROMエリアの書き込みロック/解除を行なう.

//	PICライター系 (0x10～0x17) 
//======================================================================
#define PICSPX_SETADRS24      0x10	// 24bitのアドレスをTBLPTRにセットする.
// SETADRS24 [ CMD ] [ --- ] [ADRL] [ADRH] [ADRU] 
#define PICSPX_GETDATA8L      0x11	//  8bitデータ列を取得する.
// GETDATA8L [ CMD ] [ LEN ] [ADRL] [ADRH] [ADRU] 
#define PICSPX_SETCMD16L      0x12	// 16bitデータ列を書き込む.
// SETCMD16L [ CMD ] [ LEN ] [ADRL] [ADRH] [ADRU] [CMD4] [ MS ] data[32]
#define PICSPX_BITBANG        0x14	// MCLR,PGM,PGD,PGC各bitを制御する.
// BITBANG   [ CMD ] [ LEN ] data[60]
#define PICSPX_BITREAD        0x15	// MCLR,PGM,PGD,PGC各bitを制御し、読み取り結果も返す.
// BITREAD   [ CMD ] [ LEN ] data[60]


#define PICSPX_WRITE24F       0x16	// PIC24Fデータ列を書き込む.
#define PICSPX_READ24F     	  0x17	// PIC24Fデータ列を読み出す.

//	JTAGライター系 (0x18～0x1f) 
//======================================================================
#define HIDASP_JTAG_WRITE	  0x18	//JTAG 書き込み.
#define HIDASP_JTAG_READ	  0x19	//JTAG 読み書き.
#define HIDASP_JTAG_CTRLW	  0x1a	//JTAG 各信号線の制御.
#define HIDASP_JTAG_CTRLR	  0x1b	//JTAG 各信号線の制御.


//	AVR書き込みコマンド系 (0x20～0x2f)
//======================================================================
// +0      +1      +2        +3         +35
//[ID] [HIDASP_PAGE_TX_*] [len=32] [data(32)] [ isp(4) ]
#define HIDASP_PAGE_TX        0x20	//MODE=00
#define HIDASP_PAGE_RD        0x21	//MODE=01
#define HIDASP_PAGE_TX_START  0x22	//MODE=02  ADDR 初期化.
#define HIDASP_PAGE_TX_FLUSH  0x24	//MODE=04  転送後 ISP
//                            0x27 まではPAGE_TXで使用する.

#define CMD_MASK              0xf8	//AVR書き込みコマンドの検出マスク.
#define MODE_MASK             0x07	//AVR書き込みコマンドの種別マスク.

//	AVR書き込みコマンド(Control)
#define HIDASP_CMD_TX         0x28	//ISPコマンドを4byte送る.
#define HIDASP_CMD_TX_1       0x29	//ISPコマンドを4byte送って、結果4byteを受け取る.
#define HIDASP_SET_PAGE       0x2a	//AVRのページアドレスをセットする.
#define HIDASP_SET_STATUS     0x2b	//AVRライターモードの設定
#define HIDASP_SET_DELAY      0x2c	//SPI書き込み遅延時間の設定.
#define HIDASP_WAIT_MSEC      0x2e	//1mS単位の遅延を行う.

//	コマンドはここまで.
//======================================================================



//デバイスID
#define	DEV_ID_FUSION			0x55
#define	DEV_ID_STD				0x5a
#define	DEV_ID_MEGA88			0x88
#define	DEV_ID_MEGA88_USERMODE	0x89
#define	DEV_ID_PIC				0x25
#define	DEV_ID_PIC18F14K		0x14
#define	DEV_ID_SH2A				0x2a
#define	DEV_ID_RX62N			0x62
#define	DEV_ID_ARM_M0			0xa0
#define	DEV_ID_ARM_M3			0xa3
#define	DEV_ID_ARM_FM3			0xf3

// poll mode
#define	POLL_NONE				0
#define	POLL_ANALOG				0xa0
#define	POLL_DIGITAL			0xa0
#define	POLL_PORT				0xc0
#define	POLL_PRINT				0xc1
#define	POLL_END				0xc9

//  BitBangデータビット.

#define	JCMD_bitVcc2 0x80
#define	JCMD_bitVcc1 0x40
#define	JCMD_bitTRST 0x20
#define	JCMD_bitSRST 0x10
#define	JCMD_bitTMS	 0x08	// TMS 
#define	JCMD_bitTDI	 0x04	// TDI MOSI
#define	JCMD_bitTCK	 0x02	// TCK
#define	JCMD_bitTDO	 0x01	// TDO MISO

//	方向レジスタ (0=出力)

#define	dirTMS		0x80
#define	dirTDI		0x40
#define	dirTCK		0x20
#define	dirTDO		0x10	// このPINのみ入力(==1)


/*
 * ドキュメント

  +--------------+-------------------------------------------------------------+
  |   コマンド   |  パラメータ              | 動作                             |
  +--------------+-----------+-------------------------------------------------+
  |TEST          | ECHO      |          ＝接続チェックを行う.
  +--------------+------+----+------------+---------------+--------------------+
  |POKE          |subcmd|size| address    | data ....     | ＝内蔵ＲＡＭ書き込み
  +--------------+------+----+------------+---------------+--------------------+
  |PEEK          |subcmd|size| address    |                 ＝内蔵ＲＡＭ読み出し
  +--------------+------+----+------------+------------------------------------+
   subcmd はByte,Half,Wordの指定. sizeは読み書きバイト数(16bit) address(32bit)


AVRライター系:
  +--------------+-------------------------------------------------------------+
  |   コマンド   |  パラメータ              | 動作                             |
  +--------------+-----------+--------+----------------------------------------+
  |SET_STATUS    | PORTD     | PORTB  | ＝LED制御を行う.
  +--------------+-----------+--------+----------------------------------------+
  |CMD_TX        | ISP_COMMAND[4]           | ＝ISPコマンドを４バイト送信
  +--------------+--------------------------+----------------------------------+
  |CMD_TX_1      | ISP_COMMAND[4]           | ＝ISPコマンドを４バイト送信して受信
  +--------------+-----------+--------------+----------------------------------+
  |SET_PAGE      | PAGE_MODE |   ＝ページモードの初期化とアドレスリセット
  +--------------+-----------+-------------------------------------------------+
  |PAGE_TX       | LENGTH    |   DATA[ max 32byte ]       | ＝ページモード書き込み
  +--------------+-----------+-------------------------------------------------+
  |PAGE_RD       | LENGTH    |                              ＝ページモード読み出し
  +--------------+-----------+-------------------------------------------------+
  |PAGE_TX_START | LENGTH    |   DATA[ max 32byte ]       | ＝ページモードの初期化とページモード書き込み
  +--------------+-----------+-------------------------------------------------+
  |PAGE_TX_FLUSH | LENGTH    |   DATA[ 32byte 固定]       | ISP_COMMAND[4]    | ＝ページモード書き込み後Ｆｌｕｓｈ書き込み実行.
  +--------------+-----------+-------------------------------------------------+
  |SET_DELAY     | delay     |   ＝ＳＰＩクロック速度の設定(delay=0..255).
  +--------------+-----------+-------------------------------------------------+

 *注1: PAGE_TX_STARTとPAGE_TX_FLUSHはどちらもPAGE_TXに対する修飾ビットなので、
       両方を併せ持つパケットも作成可能です。

 *注2: コマンドのLSBが１になっている場合はコマンド実行後、読み取りフェーズが実行されます。
       読み取りサイズは、ホスト側から指定されます。

  応答フェーズで返却されるパケットの構造
  +--------------+-------------------------------------------------------------+
  |   コマンド   |  返却値                                                     |
  +--------------+--------+-----+-----+----------+------+-----------+----------+
  |TEST          | DEV_ID |VER_L|VER_H|DEV_LOADER| ECHO | FLASH_USED|FLASH_END |
  +--------------+--------+-----+-----+----------+------+-----------+----------+
  |PEEK          | DATA[ max 60 byte ]      | ＝内蔵ＲＡＭ読み出しデータ.
  +--------------+--------------------------+----------------------------------+

 *注3: HID Reportは先頭に必ずReportIDが入るため、
       上記返却値は HID Reportの２バイト目以降の意味となります。

 *注4: DEV_ID は現在の版では、以下の値です。 
		0x55 : フュージョンありAVRファームウェア
		0x5A : フュージョンなしAVRファームウェア
		0x88 : AVR ATMEGA88			
		0x89 : AVR ATMEGA88_USERMODE
		0x25 : PIC18F2550/4550	
		0x14 : PIC18F14K		

		0x2A : Renesas SH-2A
		0x62 : Renesas RX62N
		0xa0 : ARM_Cortex_M0			
		0xa3 : ARM_Cortex_M3			
		0xf3 : ARM_Fujitsu_FM3			
       ECHOは渡された値をそのまま返します.


*/
