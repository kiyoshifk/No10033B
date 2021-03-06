
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


#define ISHI_N		0x0000		/* 石ナシ					*/
#define ISHI_K		0x0100		/* 黒石						*/
#define ISHI_S		0x0200		/* 白石						*/
#define ISHI_O		0x0300		/* 盤外						*/

#define TEN_INFINIT 0x70000000

#define MV_NARI		0x10000
#define MV_TORI		0x20000

//	int move;	0x000000ff:source, 0x0000ff00:destination, 0x00010000:成フラグ


//	1:	下手歩
//	2:	下手と
//	3:	下手香
//	4:	下手成香
//	5:	下手桂
//	6:	下手成桂
//	7:	下手銀
//	8:	下手成銀
//	9:	下手金
//	10:	下手角
//	11:	下手馬
//	12:	下手飛
//	13:	下手竜
//	14:	下手玉

//	15:	上手歩
//	16:	上手と
//	17:	上手香
//	18:	上手成香
//	19:	上手桂
//	20:	上手成桂
//	21:	上手銀
//	22:	上手成銀
//	23:	上手金
//	24:	上手角
//	25:	上手馬
//	26:	上手飛
//	27:	上手竜
//	28:	上手王


//		int ban[131+29], 盤面 for(i=21; i<110; i++)

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

static const int dir1[5]={-10,-1,1,10,0};				//	上下左右
static const int dir2[5]={-11,-9,9,11,0};				//	ななめ
static const int dir3[9]={-11,-10,-9,-1,1,9,10,11,0};	//	８方向
static const int dir10[28+1][9]={
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },
	{-10,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//1		下手歩
	{-11,-10,-9 ,-1 ,1  ,10 ,0  ,0  ,0  },	//2		下手と
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//3		下手香
	{-11,-10,-9 ,-1 ,1  ,10 ,0  ,0  ,0  },	//4		下手成香
	{-21,-19,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//5		下手桂
	{-11,-10,-9 ,-1 ,1  ,10 ,0  ,0  ,0  },	//6		下手成桂
	{-11,-10,-9 ,9  ,11 ,0  ,0  ,0  ,0  },	//7		下手銀
	{-11,-10,-9 ,-1 ,1  ,10 ,0  ,0  ,0  },	//8		下手成銀
	{-11,-10,-9 ,-1 ,1  ,10 ,0  ,0  ,0  },	//9		下手金
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//10	下手角
	{-10,-1 ,1  ,10 ,0  ,0  ,0  ,0  ,0  },	//11	下手馬
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//12	下手飛
	{-11,-9 ,9  ,11 ,0  ,0  ,0  ,0  ,0  },	//13	下手竜
	{-11,-10,-9 ,-1 ,1  ,9  ,10 ,11 ,0  },	//14	下手玉
	{10 ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//15	上手歩
	{9  ,10 ,11 ,-1 ,1  ,-10,0  ,0  ,0  },	//16	上手と
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//17	上手香
	{9  ,10 ,11 ,-1 ,1  ,-10,0  ,0  ,0  },	//18	上手成香
	{19 ,21 ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//19	上手桂
	{9  ,10 ,11 ,-1 ,1  ,-10,0  ,0  ,0  },	//20	上手成桂
	{9  ,10 ,11 ,-11,-9 ,0  ,0  ,0  ,0  },	//21	上手銀
	{9  ,10 ,11 ,-1 ,1  ,-10,0  ,0  ,0  },	//22	上手成銀
	{9  ,10 ,11 ,-1 ,1  ,-10,0  ,0  ,0  },	//23	上手金
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//24	上手角
	{-10,-1 ,1  ,10 ,0  ,0  ,0  ,0  ,0  },	//25	上手馬
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//26	上手飛
	{-11,-9 ,9  ,11 ,0  ,0  ,0  ,0  ,0  },	//27	上手竜
	{-11,-10,-9 ,-1 ,1  ,9  ,10 ,11 ,0  },	//28	上手王
};

static const int tenall[29]={
	0,
	87,			//歩	1
	534,		//と	2
	232,		//香	3
	489,		//成香	4
	257,		//桂	5
	510,		//成桂	6
	369,		//銀	7
	495,		//成銀	8
	444,		//金	9
	569,		//角	10
	827,		//馬	11
	642,		//飛	12
	945,		//竜	13
	100000,		//玉	14
	-87,		//歩	15
	-534,		//と	16
	-232,		//香	17
	-489,		//成香	18
	-257,		//桂	19
	-510,		//成桂	20
	-369,		//銀	21
	-495,		//成銀	22
	-444,		//金	23
	-569,		//角	24
	-827,		//馬	25
	-642,		//飛	26
	-945,		//竜	27
	-100000,	//玉	28
};
	
#define macrox(pos) ((pos)%10)			/* pos からＸ座標への変換	*/
#define macroy(pos) ((pos)/10-1)		/* pos からＹ座標への変換	*/
#define macropos(x,y) ((x)+(y)*10+10)	/* X,Y 座標から pos への変換	*/

					/* 白黒の反転			*/
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
//	int games;							/* この手を探索した回数					*/
//	double win;							/* 勝った回数							*/
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

