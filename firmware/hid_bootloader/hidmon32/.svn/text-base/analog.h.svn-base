
#define	AIN_SCREEN_X0	4
#define	AIN_SCREEN_W	384
#define	AIN_SCREEN_H	460

#define	AIN_CENTER_X	(AIN_SCREEN_W/2)
#define	AIN_CENTER_R	(AIN_SCREEN_W*6/10)
#define	AIN_CENTER_Y	(AIN_CENTER_R + 48)
#define	AIN_CENTER_H	(AIN_CENTER_R - 32)

#define	AIN_GRAPH_H		(AIN_SCREEN_H-AIN_CENTER_H-80)

//						R G B
#define	BKCOL		0x00e0d8e8
#define	BKCOL2		0x00403030
#define	REDCOL		0x00800000

enum {
	AIN_MODE_NONE = 0,			// A/D変換停止.
	AIN_MODE_VREF = 0xa0,		//内部基準電圧を計測.
	AIN_MODE_AIN0 = 0xa1,		//AIN0    電圧を計測.
	AIN_MODE_REGISTANCE = 0xa2,	//AIN0-PD6間の抵抗を計測.
};

