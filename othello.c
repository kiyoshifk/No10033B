#include "app.h"

#define macrox(pos) ((pos)%9)	/* pos からＸ座標への変換 x=1～8	*/
#define macroy(pos) ((pos)/9)	/* pos からＹ座標への変換 y=1～8	*/
#define macropos(x,y) ((x)+(y)*9)	/* X,Y 座標から pos への変換	*/
#define abs1(x) ((x)<0?-(x):(x))
#define ISHI_N		0
#define ISHI_K		1
#define ISHI_S		2
#define ISHI_O		3

struct othe_v{
	int tekazu;
	int teban;
	int white;						//0:cpu, 1:human
	int black;						//0:cpu, 1:human
	char goban[100];
	int timesetting;
	int brink_flag;
	int eva_cnt;
	unsigned rnd_init;
	int time1;
};

static struct othe_v othe_v;

static const int othe_dir3[8]={-8,-9,-10,-1,1,8,9,10};

static const struct menu play_menu[] = {
	{0, 30,  1,"黒：ＣＰＵ、白：ＣＰＵ"},
	{0, 50,  2,"黒：人間、　白：人間"},
	{0, 70,  3,"黒：ＣＰＵ、白：人間"},
	{0, 90,  4,"黒：人間、　白：ＣＰＵ"},
	{0,0,0,0}
};
static const struct menu strong_menu[] = {
	{0, 50,  1,"　強い　"},
	{0, 70,  2,"　普通　"},
	{0, 90,  3,"　弱い　"},
	{0,0,0,0}
};
static const struct menu brink_menu[] = {
    {0, 60, 1,"　点滅　ＯＮ　　"},
    {0, 80, 2,"　点滅　ＯＦＦ　"},
    {0,0,0,0}
};

#define T1 8000
#define T2 (-250)
#define T3 30
#define T4 0
#define T5 (-2000)
#define T6 (-10)
#define T7 (-10)
#define T8 (-10)
#define T9 (-10)
#define Ta (-10)
static const int tenpat_std[]={0,0,0,0,0,0,0,0,0,0,
               T1,T2,T3,T4,T4,T3,T2,T1,0,
               T2,T5,T6,T7,T7,T6,T5,T2,0,
               T3,T6,T8,T9,T9,T8,T6,T3,0,
               T4,T7,T9,Ta,Ta,T9,T7,T4,0,
               T4,T7,T9,Ta,Ta,T9,T7,T4,0,
               T3,T6,T8,T9,T9,T8,T6,T3,0,
               T2,T5,T6,T7,T7,T6,T5,T2,0,
               T1,T2,T3,T4,T4,T3,T2,T1,0,
               };

#define U1 8000
#define U2 (-250)
#define U3 30
#define U4 0
#define U5 (-2000)
#define U6 (-10)
#define U7 (-10)
#define U8 (-10)
#define U9 (-10)
#define Ua (-10)
static const int tenpat_test[]={0,0,0,0,0,0,0,0,0,0,
               U1,U2,U3,U4,U4,U3,U2,U1,0,
               U2,U5,U6,U7,U7,U6,U5,U2,0,
               U3,U6,U8,U9,U9,U8,U6,U3,0,
               U4,U7,U9,Ua,Ua,U9,U7,U4,0,
               U4,U7,U9,Ua,Ua,U9,U7,U4,0,
               U3,U6,U8,U9,U9,U8,U6,U3,0,
               U2,U5,U6,U7,U7,U6,U5,U2,0,
               U1,U2,U3,U4,U4,U3,U2,U1,0,
               };

static const int tenpat_end[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				T1, T1, T1, T1, T1, T1, T1, T1, 0,
				T1, T1, T1, T1, T1, T1, T1, T1, 0,
				T1, T1, T1, T1, T1, T1, T1, T1, 0,
				T1, T1, T1, T1, T1, T1, T1, T1, 0,
				T1, T1, T1, T1, T1, T1, T1, T1, 0,
				T1, T1, T1, T1, T1, T1, T1, T1, 0,
				T1, T1, T1, T1, T1, T1, T1, T1, 0,
				T1, T1, T1, T1, T1, T1, T1, T1, 0,
};


extern const char o_ban[15][10];
extern const char o_ban_c[15][10];
extern const char o_kuro[15][10];
extern const char o_kuro_c[15][10];
extern const char o_shiro[15][10];
extern const char o_shiro_c[15][10];

//static void char_disp(int x, int y, int c, int rev_flag);
//static void display_xn(int x, int y, const char * str, int n);
//static void circle1(int x0, int y0, int r, int color);
static void othe_game_init();
static void othe_game();
//static int keyinwait();


/********************************************************************************/
/*		othe_main																*/
/********************************************************************************/

void othe_main()
{
	int cmd;

	othe_v.rnd_init = TickCount;
	lcd_clear(BRUE_U);
	str_cdisp(60, "オセロゲーム");
	str_cdisp(85, "K.Fukumoto");
	sw_updown();
	
	lcd_clear(GRAY);
	cmd = menu_select(play_menu);
	if(cmd==1){
		othe_v.black = 0;
		othe_v.white = 0;
	}
	else if(cmd==2){
		othe_v.black = 1;
		othe_v.white = 1;
	}
	else if(cmd==3){
		othe_v.black = 0;
		othe_v.white = 1;
	}
	else{
		othe_v.black = 1;
		othe_v.white = 0;
	}
	
	lcd_clear(GRAY);
	cmd = menu_select(strong_menu);
	if(cmd==1){
		othe_v.timesetting = 1000;
	}
	else if(cmd==2){
		othe_v.timesetting = 100;
	}
	else{
		othe_v.timesetting = 10;
	}
    
    lcd_clear(GRAY);
    cmd = menu_select(brink_menu);
    if(cmd==1){
        othe_v.brink_flag = 1;
    }
    else{
        othe_v.brink_flag = 0;
    }

	lcd_clear(WHITE);
	if(othe_v.timesetting==1000)
		str_disp(100,20, "強さ 強い");
	else if(othe_v.timesetting==100)
		str_disp(100,20, "強さ 普通");
	else
		str_disp(100,20, "強さ 弱い");
	
	othe_game_init();
	othe_game();
}
/********************************************************************************/
/*		othe_koma0																*/
/*		石無しの駒																*/
/********************************************************************************/
//void othe_koma_pri(int x, int y)
//{
//	lcd_area_set(20+(x-1)*24, 28+(y-1)*24, 20+x*24-1, 28+y*24-1);
//	lcd_write_cmd(MemoryWrite);
//}

//void othe_koma(int x, int y, const short *pat)
//{
//	int i;
//	
//	if(x<1 || x>8 || y<1 || y>8)
//		return;
////	LCD_ALL_S;								// idle
////	LCD_CS_C;								// CS clear
//	othe_koma_pri(x, y);
//	for(i=0; i<24*24; i++){
//		lcd_write_color(pat[i]);
//	}
////	LCD_ALL_S;								// idle
//}

static void othe_koma0(int x, int y)
{
	write_square(10+(x-1)*10, 20+(y-1)*15, 10, 15, (char*)o_ban);
}
static void othe_koma0_c(int x, int y)				// カーソルマーク付き
{
	write_square(10+(x-1)*10, 20+(y-1)*15, 10, 15, (char*)o_ban_c);
//	othe_koma(x, y, o_ban_c);
}
/********************************************************************************/
/*		othe_koma1																*/
/*		黒駒																	*/
/********************************************************************************/
static void othe_koma1(int x, int y)
{
	write_square(10+(x-1)*10, 20+(y-1)*15, 10, 15, (char*)o_kuro);
//	othe_koma(x, y, o_kuro);
}
static void othe_koma1_c(int x, int y)				// カーソルマーク付き
{
	write_square(10+(x-1)*10, 20+(y-1)*15, 10, 15, (char*)o_kuro_c);
//	othe_koma(x, y, o_kuro_c);
}
/********************************************************************************/
/*		othe_koma2																*/
/*		白駒																	*/
/********************************************************************************/
static void othe_koma2(int x, int y)
{
	write_square(10+(x-1)*10, 20+(y-1)*15, 10, 15, (char*)o_shiro);
//	othe_koma(x, y, o_shiro);
}
static void othe_koma2_c(int x, int y)				// カーソルマーク付き
{
	write_square(10+(x-1)*10, 20+(y-1)*15, 10, 15, (char*)o_shiro_c);
//	othe_koma(x, y, o_shiro_c);
}
/********************************************************************************/
/*		othe_dispban																*/
/*		lcd_area_set(20+(x-1)*24, 28+(y-1)*24, 20+x*24-1, 28+y*24-1);			*/
/********************************************************************************/
static void othe_dispban(char ban[100], int pos)
{
	int x, y, p, k;
	char str[2];
	
	lcd_clearA(     10, 20+15*8, 10+10*8, 20+15*8, BLACK);
	lcd_clearA(10+10*8,      20, 10+10*8, 20+15*8, BLACK);
	for(x=1; x<=8; x++){
		if(x==1 || x==3 || x==5 || x==7){
			drawcount = 0;
			while(drawcount==0)
				;
		}
		for(y=1; y<=8; y++){
			p = macropos(x,y);
			k = ban[p];
			if(k==ISHI_N)
				othe_koma0(x,y);
			else if(k==ISHI_K)
				othe_koma1(x,y);
			else if(k==ISHI_S)
				othe_koma2(x,y);
		}
	}
	
	drawcount = 0;
	while(drawcount==0)
		;
	str[1] = 0;
	for(x=1; x<=8; x++){
		str[0] = (x-1)+'A';
		str_disp((x-1)*10+12, 10, str);
	}
	for(y=1; y<=8; y++){
		str[0] = (y-1)+'1';
		str_disp(2, (y-1)*15+23, str);
	}
    x = macrox(pos);
    y = macroy(pos);
    if(ban[pos]==ISHI_K)
    	othe_koma1_c(x,y);						// 黒カーソル
    else if(ban[pos]==ISHI_S)
    	othe_koma2_c(x,y);						// 白カーソル
    else
    	othe_koma0_c(x, y);						// 盤カーソル
}
/********************************************************************************/
/*		othe_human_input														*/
/********************************************************************************/
static int othe_human_input(int *pos)
{
	int x, y, sw;
	char *ban = othe_v.goban;
    
	x = macrox(*pos);
	y = macroy(*pos);
	for(;;){
		*pos = macropos(x,y);
	    if(ban[*pos]==ISHI_K)
	    	othe_koma1_c(x,y);					// 黒カーソル
	    else if(ban[*pos]==ISHI_S)
	    	othe_koma2_c(x,y);					// 白カーソル
	    else
	    	othe_koma0_c(x, y);					// 盤カーソル
	    
		sw = sw_updown();
		
		*pos = macropos(x,y);
	    if(ban[*pos]==ISHI_K)
	    	othe_koma1(x,y);					// 黒
	    else if(ban[*pos]==ISHI_S)
	    	othe_koma2(x,y);					// 白
	    else
	    	othe_koma0(x, y);					// 盤
		if(sw & UP){
			if(--y < 1)
				y = 1;
		}
		if(sw & DOWN){
			if(++y > 8)
				y = 8;
		}
		if(sw & RIGHT){
			if(++x > 8)
				x = 8;
		}
		if(sw & LEFT){
			if(--x < 1)
				x = 1;
		}
		if(sw & (A_SW | B_SW)){
			*pos = macropos(x, y);
			return *pos;
		}
	}
}
/********************************************************************************/
/*      othe_brinking                                                           */
/********************************************************************************/
static void othe_brinking(int k, int pos)
{
    int i, x, y;
    
    x = macrox(pos);
    y = macroy(pos);
    for(i=0; i<3; i++){
        if(k==ISHI_K)
            othe_koma1(x,y);
        else
            othe_koma2(x,y);
        wait_ms(500);
        othe_koma0(x,y);
		wait_ms(500);
    }
}


//#define macrox(pos) ((pos)%9)	/* pos からＸ座標への変換 x=1～8	*/
//#define macroy(pos) ((pos)/9)	/* pos からＹ座標への変換 y=1～8	*/
//#define macropos(x,y) ((x)+(y)*9)	/* X,Y 座標から pos への変換	*/
//#define macroinv(koma) ((koma)==ISHI_K ? ISHI_S : ((koma)==ISHI_S ? ISHI_K : message("macroinv error k=%d\n",koma)))
#define macroinv(koma) (3-(koma))
//#define abs1(x) ((x)<0 ? -(x) : (x))
//
//#define ISHI_N		0
//#define ISHI_K		1
//#define ISHI_S		2
//#define ISHI_O		3


//extern struct othe_v othe_v;
//extern int othe_dir3[];
//extern const int tenpat_std[];
//extern const int tenpat_end[];


//int genrand_int31(void);
static int othe_human_input(int *pos);
static void othe_dispban(char ban[100], int pos);
//int Dispbug(const char *fmt, ...);
//void msg_printf(const char *fmt, ...);
//void msg_main_printf(const char *fmt, ...);
static int othe_gametop(int *pten);
static void othe_brinking(int k, int pos);

/********************************************************************************/
/*		othe_initban															*/
/********************************************************************************/
static void othe_initban(char *ban)
{
	int x, y, p;
	
	memset(ban, ISHI_O, 100);
	for(x=1; x<=8; x++){
		for(y=1; y<=8; y++){
			p = macropos(x,y);
			ban[p] = ISHI_N;
		}
	}
	ban[macropos(4,4)] = ISHI_S;
	ban[macropos(5,5)] = ISHI_S;
	ban[macropos(4,5)] = ISHI_K;
	ban[macropos(5,4)] = ISHI_K;
}
/********************************************************************************/
/*		othe_MCrnd																*/
/********************************************************************************/
//static int othe_MCrnd(int i)
//{
//    return MCrnd(i);
//}
/********************************************************************************/
/*		othe_game_init															*/
/********************************************************************************/
static void othe_game_init()
{
	othe_initban(othe_v.goban);
	othe_v.tekazu = 1;
	othe_v.teban = ISHI_K;
	init_genrand(TickCount);
}
/********************************************************************************/
/*		othe_yomitst															*/
/********************************************************************************/
static int othe_yomitst_s(char *ban, int k, int pos, int dir)
{
	int kk;
	
	if(ban[pos])							// 石無しでなければ
		return 0;							// 打てない
	pos += dir;
	if(ban[pos] != macroinv(k))				// 敵駒に接触していなければ
		return 0;							// 打てない
	for(;;){
		pos += dir;
		kk = ban[pos];
		if(kk==k)							// はさんでいる
			return 1;						// 打てる
		if(kk==ISHI_N || kk== ISHI_O)
			return 0;						// 打てない
	}
}

static int othe_yomitst(char *ban, int k, int pos)
{
	int i;
	
	if(ban[pos])							// 石無しでなければ
		return 0;							// 打てない
	for(i=0; i<8; i++){
		if(othe_yomitst_s(ban, k, pos, othe_dir3[i]))	// dir3 方向で打てるか？
			return 1;						// 打てる
	}
	return 0;								// 打てない
}

/********************************************************************************/
/*		othe_get_upos															*/
/********************************************************************************/
static void othe_get_upos(char *ban, int k, char upos[60], int *upos_cnt)
{
	int p;
	
	*upos_cnt = 0;
	for(p=10; p<81; p++){
		if(othe_yomitst(ban, k, p))				// 打てるか？
			upos[(*upos_cnt)++] = p;
	}
}

//		この盤面に打つ手があるか？
static int upos_tst(char *ban, int k)
{
	char upos[60];
	int upos_cnt;
	
	othe_get_upos(ban, k, upos, &upos_cnt);
	if(upos_cnt==0)
		return 0;								// 打てない
	return 1;									// 打てる
}
/********************************************************************************/
/*		othe_yomiutu															*/
/*		開放度を返す															*/
/********************************************************************************/
static int othe_yomiutu(char *ban, int k, int pos)
{
	int i, j, s, p, dir, kaiho;
	
	s = macroinv(k);
	kaiho = 0;
	for(i=0; i<8; i++){
		dir = othe_dir3[i];
		if(othe_yomitst_s(ban, k, pos, dir)){	// dir 方向で打てるか？
			p = pos;
			for(;;){
				p += dir;
				if(ban[p]==s){
					ban[p] = k;
					for(j=0; j<8; j++){
						if(ban[p+othe_dir3[j]]==ISHI_N)
							++kaiho;
					}
				}
				else{
					break;
				}
			}
		}
	}
	ban[pos] = k;
	return kaiho;
}
/********************************************************************************/
/*		othe_result																*/
/*		石数を数える、黒+														*/
/********************************************************************************/
static int othe_result(char *ban)
{
	int ten, p;
	
	ten = 0;
	for(p=10; p<81; p++){
		if(ban[p]==ISHI_K)
			++ten;
		else if(ban[p]==ISHI_S)
			--ten;
	}
	return ten;
}
/********************************************************************************/
/*		othe_game																*/
/*		ゲーム進行のメインルーチン												*/
/********************************************************************************/
static void othe_game()
{
	int player;								// 0:cpu, 1:human
	int pos, ten;
	int eva;
	char str[50];
	
	othe_v.time1 = TickCount;
	player = othe_v.teban==ISHI_K ? othe_v.black : othe_v.white;
	str_disp(100,40, player ? "人間　" : "ＣＰＵ");
	pos = macropos(1,1);
	for(;;){
		/***	パスの処理	***/
		if(upos_tst(othe_v.goban, othe_v.teban)==0){		// 打てない
			othe_v.teban = macroinv(othe_v.teban);
			if(upos_tst(othe_v.goban, othe_v.teban)==0){	// 相手番でも打てない
				ten = othe_result(othe_v.goban);
				if(ten==0)
					str_disp(100,60, "引き分け");
				else{
                    sprintf(str, "%s %d 石勝", ten>0 ? "黒" : "白", abs1(ten));
					str_disp(100,60, str);
				}
				othe_dispban(othe_v.goban, pos);
				sw_updown();
				return;
			}
			str_disp(100,80, "打てません");
            othe_dispban(othe_v.goban, pos);
			sw_updown();
			str_disp(100,80, "　　　　　");
		}
		player = othe_v.teban==ISHI_K ? othe_v.black : othe_v.white;
		str_disp(100,40, player ? "人間　" : "ＣＰＵ");
		if(player==0){						// cpu の手番
			str_disp(100,100, "thinking");
			othe_dispban(othe_v.goban, pos);
			pos = othe_gametop(&eva);
			str_disp(100,100, "        ");
			buzzer_div = 8;					// 1KHz
			buzzer_ms = 50;
			if(othe_yomitst(othe_v.goban, othe_v.teban, pos)==0){
				printf("game error [%d,%d]", macrox(pos), macroy(pos));
				for(;;)
					;
			}
		}
		else{								// 人間の手番
			str_disp(100,100, "貴方の番");
			for(;;){
				othe_dispban(othe_v.goban, pos);
				othe_human_input(&pos);
				str_disp(100,80, "　　　");
				if (othe_yomitst(othe_v.goban, othe_v.teban, pos))
					break;
				str_disp(100,80, "エラー");
				buzzer_div = 16;				// 500Hz
				buzzer_ms = 500;
			}
			str_disp(100,100, "　　　　");
			buzzer_div = 8;					// 1KHz
			buzzer_ms = 50;
		}
        if(othe_v.brink_flag){
            othe_brinking(othe_v.teban, pos);
        }
		othe_yomiutu(othe_v.goban, othe_v.teban, pos);
		othe_v.teban = macroinv(othe_v.teban);
		++othe_v.tekazu;
	}
}
/********************************************************************************/
/*		othe_gametop															*/
/********************************************************************************/
#define INF 10000000

#define T1 8000
#define T2 (-250)
#define T3 30
#define T4 0
#define T5 (-2000)
#define T6 (-10)
#define T7 (-10)
#define T8 (-10)
#define T9 (-10)
#define Ta (-10)
//int tenpat_std[]={0,0,0,0,0,0,0,0,0,0,
//               T1,T2,T3,T4,T4,T3,T2,T1,0,
//               T2,T5,T6,T7,T7,T6,T5,T2,0,
//               T3,T6,T8,T9,T9,T8,T6,T3,0,
//               T4,T7,T9,Ta,Ta,T9,T7,T4,0,
//               T4,T7,T9,Ta,Ta,T9,T7,T4,0,
//               T3,T6,T8,T9,T9,T8,T6,T3,0,
//               T2,T5,T6,T7,T7,T6,T5,T2,0,
//               T1,T2,T3,T4,T4,T3,T2,T1,0,
//               };

#define U1 8000
#define U2 (-250)
#define U3 30
#define U4 0
#define U5 (-2000)
#define U6 (-10)
#define U7 (-10)
#define U8 (-10)
#define U9 (-10)
#define Ua (-10)
//int tenpat_test[]={0,0,0,0,0,0,0,0,0,0,
//               U1,U2,U3,U4,U4,U3,U2,U1,0,
//               U2,U5,U6,U7,U7,U6,U5,U2,0,
//               U3,U6,U8,U9,U9,U8,U6,U3,0,
//               U4,U7,U9,Ua,Ua,U9,U7,U4,0,
//               U4,U7,U9,Ua,Ua,U9,U7,U4,0,
//               U3,U6,U8,U9,U9,U8,U6,U3,0,
//               U2,U5,U6,U7,U7,U6,U5,U2,0,
//               U1,U2,U3,U4,U4,U3,U2,U1,0,
//               };

//int tenpat_end[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//				T1, T1, T1, T1, T1, T1, T1, T1, 0,
//				T1, T1, T1, T1, T1, T1, T1, T1, 0,
//				T1, T1, T1, T1, T1, T1, T1, T1, 0,
//				T1, T1, T1, T1, T1, T1, T1, T1, 0,
//				T1, T1, T1, T1, T1, T1, T1, T1, 0,
//				T1, T1, T1, T1, T1, T1, T1, T1, 0,
//				T1, T1, T1, T1, T1, T1, T1, T1, 0,
//				T1, T1, T1, T1, T1, T1, T1, T1, 0,
//};
//int eva_cnt;

//static void othe_sort_upos(char *ban, int k, int depth, int maxdepth, char *upos, int upos_cnt);

static int othe_evaluate(char *ban, int k, const int *tenpat, int gamma, int kaiho)
{
	int ten;
	int p;
	
	++othe_v.eva_cnt;
	ten = 0;
	for(p=10; p<81; p++){
		if(ban[p]==ISHI_K)
			ten += tenpat[p];
		else if(ban[p]==ISHI_S)
			ten -= tenpat[p];
	}
	if(othe_v.tekazu <= 15){
//		ten += othe_MCrnd(40)-20;				// 乱数加算
		ten += (genrand_int31() & 0x1f) - 0x10;
	}
	ten = k==ISHI_K ? ten : -ten;

	ten += 20*gamma;
	ten += 10*kaiho;
	return ten;
}

static int othe_negamax(char *ban, int k, int depth, int maxdepth, int *pos, int alpha, int beta, int pass, int gamma, int kaiho)
{
	int i, upos_cnt, dummy, kho, ret;
	char ban1[100], upos[60];
	int ten, max;
	
    if(TickCount-othe_v.time1 > 6){
		drawcount = 0;
		while(drawcount==0)
			;
		othe_v.time1 = TickCount;
    }
	if(depth>=maxdepth){
		return othe_evaluate(ban, k, tenpat_std, gamma, kaiho);	/* 中間評価関数						*/
	}
	
	max = -INF;
	*pos = 0;
	othe_get_upos(ban, k, upos, &upos_cnt);
	if (upos_cnt == 0){
		if (pass){												// 2回連続パス
			return othe_evaluate(ban, k, tenpat_end, gamma, kaiho);	/* 中間評価関数						*/
		}
		ret = -othe_negamax(ban, macroinv(k), depth + 1, maxdepth, &dummy, -beta, -alpha, 1, -gamma, -kaiho);
		if(TickCount-othe_v.time1 > 6){
			drawcount = 0;
			while(drawcount==0)
				;
			othe_v.time1 = TickCount;
		}
		return ret;
	}
//	if(maxdepth-depth >= 4 && upos_cnt >= 2)
//		othe_sort_upos(ban, k, depth+4, maxdepth, upos, upos_cnt);
	gamma = (gamma >> 1) + upos_cnt;
	for(i=0; i<upos_cnt; i++){
		memcpy(ban1, ban, 100);
		if(othe_yomitst(ban1, k, upos[i])==0)			/* 打てない					*/
			continue;
		kho = othe_yomiutu(ban1, k, upos[i]);
		if(depth<=1)
			kho = kho-kaiho;
		else
			kho = -kaiho;

		ten = - othe_negamax(ban1, macroinv(k), depth+1, maxdepth, &dummy, -beta, -alpha, 0, -gamma, kho);
	    if(TickCount-othe_v.time1 > 6){
			drawcount = 0;
			while(drawcount==0)
				;
			othe_v.time1 = TickCount;
		}
		if(ten > beta)							/* 枝刈							*/
			return ten;
		if(max < ten){
			*pos = upos[i];
			max = ten;
			if(max > alpha)
				alpha = max;
		}
	}
	return max;
}

static int othe_gametop(int *pten)
{
	int maxdepth, pos;
	int time1, ten;
	char str[100];
	
	othe_v.eva_cnt = 0;
//	maxdepth = 7;
	time1 = TickCount;
	for(maxdepth=1; maxdepth<=10; ++maxdepth){
		ten = othe_negamax(othe_v.goban, othe_v.teban, 0, maxdepth, &pos, -INF, INF, 0, 0, 0);
		if(TickCount-time1 > othe_v.timesetting)
			break;
	}
//	msg_main_printf("depth=%d %ldms ten=%ld cnt=%ld", maxdepth, GetTickCount()-time1, ten, v->eva_cnt);
	sprintf(str, "d=%d %dms ten=%d c=%d  ", maxdepth, TickCount-time1, ten, othe_v.eva_cnt);
	str_disp(0, 0, str);
	*pten = ten;
	return pos;
}
/********************************************************************************/
/*		othe_sort_upos															*/
/********************************************************************************/
#if 0
struct data {int ten; char upos;};

static int othe_cmp_upos(const void *a, const void *b)
{
	struct data *aa = (struct data *)a;
	struct data *bb = (struct data *)b;
	
	if(aa->ten==bb->ten)
		return 0;
	if(aa->ten > bb->ten)
		return -1;
	return 1;
}

static void othe_sort_upos(char *ban, int k, int depth, int maxdepth, char *upos, int upos_cnt)
{
	struct data data[60];
	int i, dummy, kaiho;
	char ban1[100];
	
	for(i=0; i<upos_cnt; i++){
		data[i].upos = upos[i];
		memcpy(ban1, ban, 100);
		kaiho = othe_yomiutu(ban1, k, upos[i]);
		data[i].ten = -othe_negamax(ban1, macroinv(k), 1, maxdepth-depth+1, &dummy, -INF, INF, 0, 0, kaiho);
	}
	qsort(data, upos_cnt, sizeof(struct data), othe_cmp_upos);
	for(i=0; i<upos_cnt; i++){
		upos[i] = data[i].upos;
	}
}
#endif



#define G	GREEN
#define B	BLACK
#define W	WHITE

const char o_ban[15][10]={
	{B,B,B,B,B,B,B,B,B,B},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
};
const char o_ban_c[15][10]={
	{B,B,B,B,B,B,B,B,B,B},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,B,B,G,G,G},
	{B,G,G,G,G,B,B,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
	{B,G,G,G,G,G,G,G,G,G},
};
const char o_kuro[15][10]={
	{B,B,B,B,B,B,B,B,B,B},
	{B,G,G,G,B,B,B,G,G,G},
	{B,G,G,B,B,B,B,B,G,G},
	{B,G,B,B,B,B,B,B,B,G},
	{B,G,B,B,B,B,B,B,B,G},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,G,B,B,B,B,B,B,B,G},
	{B,G,B,B,B,B,B,B,B,G},
	{B,G,G,B,B,B,B,B,G,G},
	{B,G,G,G,B,B,B,G,G,G},
};
const char o_kuro_c[15][10]={
	{B,B,B,B,B,B,B,B,B,B},
	{B,G,G,G,B,B,B,G,G,G},
	{B,G,G,B,B,B,B,B,G,G},
	{B,G,B,B,B,B,B,B,B,G},
	{B,G,B,B,B,B,B,B,B,G},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,W,W,B,B,B},
	{B,B,B,B,B,W,W,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,G,B,B,B,B,B,B,B,G},
	{B,G,B,B,B,B,B,B,B,G},
	{B,G,G,B,B,B,B,B,G,G},
	{B,G,G,G,B,B,B,G,G,G},
};
const char o_shiro[15][10]={
	{B,B,B,B,B,B,B,B,B,B},
	{B,G,G,G,W,W,W,G,G,G},
	{B,G,G,W,W,W,W,W,G,G},
	{B,G,W,W,W,W,W,W,W,G},
	{B,G,W,W,W,W,W,W,W,G},
	{B,W,W,W,W,W,W,W,W,W},
	{B,W,W,W,W,W,W,W,W,W},
	{B,W,W,W,W,W,W,W,W,W},
	{B,W,W,W,W,W,W,W,W,W},
	{B,W,W,W,W,W,W,W,W,W},
	{B,W,W,W,W,W,W,W,W,W},
	{B,G,W,W,W,W,W,W,W,G},
	{B,G,W,W,W,W,W,W,W,G},
	{B,G,G,W,W,W,W,W,G,G},
	{B,G,G,G,W,W,W,G,G,G},
};
const char o_shiro_c[15][10]={
	{B,B,B,B,B,B,B,B,B,B},
	{B,G,G,G,W,W,W,G,G,G},
	{B,G,G,W,W,W,W,W,G,G},
	{B,G,W,W,W,W,W,W,W,G},
	{B,G,W,W,W,W,W,W,W,G},
	{B,W,W,W,W,W,W,W,W,W},
	{B,W,W,W,W,W,W,W,W,W},
	{B,W,W,W,W,B,B,W,W,W},
	{B,W,W,W,W,B,B,W,W,W},
	{B,W,W,W,W,W,W,W,W,W},
	{B,W,W,W,W,W,W,W,W,W},
	{B,G,W,W,W,W,W,W,W,G},
	{B,G,W,W,W,W,W,W,W,G},
	{B,G,G,W,W,W,W,W,G,G},
	{B,G,G,G,W,W,W,G,G,G},
};
#if 0
const char o_shiro[15][10]={
	{B,B,B,B,B,B,B,B,B,B},
	{B,G,G,G,B,B,B,G,G,G},
	{B,G,G,B,W,W,W,B,G,G},
	{B,G,B,W,W,W,W,W,B,G},
	{B,G,B,W,W,W,W,W,B,G},
	{B,B,W,W,W,W,W,W,W,B},
	{B,B,W,W,W,W,W,W,W,B},
	{B,B,W,W,W,W,W,W,W,B},
	{B,B,W,W,W,W,W,W,W,B},
	{B,B,W,W,W,W,W,W,W,B},
	{B,B,W,W,W,W,W,W,W,B},
	{B,G,B,W,W,W,W,W,B,G},
	{B,G,B,W,W,W,W,W,B,G},
	{B,G,G,B,W,W,W,B,G,G},
	{B,G,G,G,B,B,B,G,G,G},
};
const char o_shiro_c[15][10]={
	{B,B,B,B,B,B,B,B,B,B},
	{B,G,G,G,B,B,B,G,G,G},
	{B,G,G,B,W,W,W,B,G,G},
	{B,G,B,W,W,W,W,W,B,G},
	{B,G,B,W,W,W,W,W,B,G},
	{B,B,W,W,W,W,W,W,W,B},
	{B,B,W,W,W,W,W,W,W,B},
	{B,B,W,W,W,B,B,W,W,B},
	{B,B,W,W,W,B,B,W,W,B},
	{B,B,W,W,W,W,W,W,W,B},
	{B,B,W,W,W,W,W,W,W,B},
	{B,G,B,W,W,W,W,W,B,G},
	{B,G,B,W,W,W,W,W,B,G},
	{B,G,G,B,W,W,W,B,G,G},
	{B,G,G,G,B,B,B,G,G,G},
};
#endif
