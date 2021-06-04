
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

/****************************************************************************/
/*		define                                                              */
/****************************************************************************/

//#define BANSIZE		9
//#define VERSION "2.08-No3"
//#define SINGLETHREAD
//#define MAXTHREAD 16

//#define BAN_TOP		80
//#define BAN_LEFT	80
//#define BAN_STEP	36


#define ISHI_N		0x0000		/* ÎƒiƒV					*/
#define ISHI_K		0x0100		/* •Î						*/
#define ISHI_S		0x0200		/* ”’Î						*/
#define ISHI_O		0x0300		/* ”ÕŠO						*/

#define TEN_INFINIT 0x70000000

#define MV_NARI		0x10000
#define MV_TORI		0x20000

//	int move;	0x000000ff:source, 0x0000ff00:destination, 0x00010000:¬ƒtƒ‰ƒO


//	1:	‰ºè•à
//	2:	‰ºè‚Æ
//	3:	‰ºè
//	4:	‰ºè¬
//	5:	‰ºèŒj
//	6:	‰ºè¬Œj
//	7:	‰ºè‹â
//	8:	‰ºè¬‹â
//	9:	‰ºè‹à
//	10:	‰ºèŠp
//	11:	‰ºè”n
//	12:	‰ºè”ò
//	13:	‰ºè—³
//	14:	‰ºè‹Ê

//	15:	ãè•à
//	16:	ãè‚Æ
//	17:	ãè
//	18:	ãè¬
//	19:	ãèŒj
//	20:	ãè¬Œj
//	21:	ãè‹â
//	22:	ãè¬‹â
//	23:	ãè‹à
//	24:	ãèŠp
//	25:	ãè”n
//	26:	ãè”ò
//	27:	ãè—³
//	28:	ãè‰¤


//		int ban[131+29], ”Õ–Ê for(i=21; i<110; i++)

static const int startpos[160]={
	ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,
	ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,
	ISHI_O		,ISHI_S+17	,ISHI_S+19	,ISHI_S+21	,ISHI_S+23	,ISHI_S+28	,ISHI_S+23	,ISHI_S+21	,ISHI_S+19	,ISHI_S+17	,
	ISHI_O		,0			,ISHI_S+26	,0			,0			,0			,0			,0			,ISHI_S+24	,0			,
	ISHI_O		,ISHI_S+15	,ISHI_S+15	,ISHI_S+15	,ISHI_S+15	,ISHI_S+15	,ISHI_S+15	,ISHI_S+15	,ISHI_S+15	,ISHI_S+15	,
	ISHI_O		,0			,0			,0			,0			,0			,0			,0			,0			,0			,
	ISHI_O		,0			,0			,0			,0			,0			,0			,0			,0			,0			,
	ISHI_O		,0			,0			,0			,0			,0			,0			,0			,0			,0			,
	ISHI_O		,ISHI_K+1	,ISHI_K+1	,ISHI_K+1	,ISHI_K+1	,ISHI_K+1	,ISHI_K+1	,ISHI_K+1	,ISHI_K+1	,ISHI_K+1	,
	ISHI_O		,0			,ISHI_K+10	,0			,0			,0			,0			,0			,ISHI_K+12	,0			,
	ISHI_O		,ISHI_K+3	,ISHI_K+5	,ISHI_K+7	,ISHI_K+9	,ISHI_K+14	,ISHI_K+9	,ISHI_K+7	,ISHI_K+5	,ISHI_K+3	,
	ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,
	ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static const int dir1[5]={-10,-1,1,10,0};				//	ã‰º¶‰E
static const int dir2[5]={-11,-9,9,11,0};				//	‚È‚È‚ß
static const int dir3[9]={-11,-10,-9,-1,1,9,10,11,0};	//	‚W•ûŒü
static const int dir10[28+1][9]={
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },
	{-10,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//1		‰ºè•à
	{-11,-10,-9 ,-1 ,1  ,10 ,0  ,0  ,0  },	//2		‰ºè‚Æ
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//3		‰ºè
	{-11,-10,-9 ,-1 ,1  ,10 ,0  ,0  ,0  },	//4		‰ºè¬
	{-21,-19,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//5		‰ºèŒj
	{-11,-10,-9 ,-1 ,1  ,10 ,0  ,0  ,0  },	//6		‰ºè¬Œj
	{-11,-10,-9 ,9  ,11 ,0  ,0  ,0  ,0  },	//7		‰ºè‹â
	{-11,-10,-9 ,-1 ,1  ,10 ,0  ,0  ,0  },	//8		‰ºè¬‹â
	{-11,-10,-9 ,-1 ,1  ,10 ,0  ,0  ,0  },	//9		‰ºè‹à
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//10	‰ºèŠp
	{-10,-1 ,1  ,10 ,0  ,0  ,0  ,0  ,0  },	//11	‰ºè”n
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//12	‰ºè”ò
	{-11,-9 ,9  ,11 ,0  ,0  ,0  ,0  ,0  },	//13	‰ºè—³
	{-11,-10,-9 ,-1 ,1  ,9  ,10 ,11 ,0  },	//14	‰ºè‹Ê
	{10 ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//15	ãè•à
	{9  ,10 ,11 ,-1 ,1  ,-10,0  ,0  ,0  },	//16	ãè‚Æ
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//17	ãè
	{9  ,10 ,11 ,-1 ,1  ,-10,0  ,0  ,0  },	//18	ãè¬
	{19 ,21 ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//19	ãèŒj
	{9  ,10 ,11 ,-1 ,1  ,-10,0  ,0  ,0  },	//20	ãè¬Œj
	{9  ,10 ,11 ,-11,-9 ,0  ,0  ,0  ,0  },	//21	ãè‹â
	{9  ,10 ,11 ,-1 ,1  ,-10,0  ,0  ,0  },	//22	ãè¬‹â
	{9  ,10 ,11 ,-1 ,1  ,-10,0  ,0  ,0  },	//23	ãè‹à
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//24	ãèŠp
	{-10,-1 ,1  ,10 ,0  ,0  ,0  ,0  ,0  },	//25	ãè”n
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//26	ãè”ò
	{-11,-9 ,9  ,11 ,0  ,0  ,0  ,0  ,0  },	//27	ãè—³
	{-11,-10,-9 ,-1 ,1  ,9  ,10 ,11 ,0  },	//28	ãè‰¤
};

static const int tenall[29]={
	0,
	87,			//•à	1
	534,		//‚Æ	2
	232,		//	3
	489,		//¬	4
	257,		//Œj	5
	510,		//¬Œj	6
	369,		//‹â	7
	495,		//¬‹â	8
	444,		//‹à	9
	569,		//Šp	10
	827,		//”n	11
	642,		//”ò	12
	945,		//—³	13
	100000,		//‹Ê	14
	-87,		//•à	15
	-534,		//‚Æ	16
	-232,		//	17
	-489,		//¬	18
	-257,		//Œj	19
	-510,		//¬Œj	20
	-369,		//‹â	21
	-495,		//¬‹â	22
	-444,		//‹à	23
	-569,		//Šp	24
	-827,		//”n	25
	-642,		//”ò	26
	-945,		//—³	27
	-100000,	//‹Ê	28
};
	
#define macrox(pos) ((pos)%10)			/* pos ‚©‚ç‚wÀ•W‚Ö‚Ì•ÏŠ·	*/
#define macroy(pos) ((pos)/10-1)		/* pos ‚©‚ç‚xÀ•W‚Ö‚Ì•ÏŠ·	*/
#define macropos(x,y) ((x)+(y)*10+10)	/* X,Y À•W‚©‚ç pos ‚Ö‚Ì•ÏŠ·	*/

					/* ”’•‚Ì”½“]			*/
#define macroinv(koma) ((koma)==ISHI_K ? ISHI_S : ISHI_K)
//#define abs1(x) ((x)>=0?(x):-(x))
#define min1(x,y) ((x)>(y)?(y):(x))
#define max1(x,y) ((x)>(y)?(x):(y))

/****************************************************************************/
/*	struct	                                                            */
/****************************************************************************/
//struct Ten {
//    double  ten;
//    int  pos;
//    int  rot;
//};
//
//
//#define MCHASHSIZE    10000
//
//struct MCchild {
//	int pos;
//	int ten;
//	float value;
//	int games;							/* ‚±‚Ìè‚ğ’Tõ‚µ‚½‰ñ”					*/
//	double win;							/* Ÿ‚Á‚½‰ñ”							*/
//};
//
//struct MChash {
//	__int64 code;
//	struct MChash *next;
//	int ban[274];
//	int child_num;
//	struct MCchild child[225];
//	int games_sum;
//	int count;
//	int k;
//	CRITICAL_SECTION lock;
//};

