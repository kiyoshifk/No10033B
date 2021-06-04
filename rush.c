#include "app.h"



//	┏━━━━━━┓
//	┃・・・・・・┃
//	┃・・・・・・┃
//	┃・・・・・・
//	┃・・・・・・┃
//	┃・・・・・・┃
//	┃・・・・・・┃
//	┗━━━━━━┛

static char rush_ban[8][8];	//	aaab bbbb
//	rush_ban[y][x]
//	aaa=1～4:車番号、5,6:赤い車
//	bbbbb=1からの通し番号

static const char rush_car_pat[7][3][3]={
	{
		{0,0,0},	// dummy data
		{0,0,0},
		{0,0,0},
	},
	{
		{1,0,0},	// 車番号１
		{1,0,0},
		{0,0,0}
	},
	{
		{1,1,0},	// 車番号２
		{0,0,0},
		{0,0,0}
	},
	{
		{1,0,0},	// 車番号３
		{1,0,0},
		{1,0,0}
	},
	{
		{1,1,1},	// 車番号４
		{0,0,0},
		{0,0,0}
	},
	{
		{1,1,0},	// 車番号５ 赤い車
		{0,0,0},
		{0,0,0}
	},
	{
		{1,1,1},	// 車番号６ 赤い車
		{0,0,0},
		{0,0,0}
	}
};

static const unsigned char rush_ban_test[8][8]={
	{20,     20,     20,     20,     20,     20,     20,     20},
	{20,      0,      0,      0,      0,      0,      0,     20},
	{20,      0,      0,      0,      0,      0,      0,     20},
	{20,      0,      0,      0,      0,      0,      0,      0},
	{20,      0,      0,      0,      0,      0,      0,     20},
	{20,      0,      0,      0,      0,      0,      0,     20},
	{20,      0,      0,      0,      0,      0,      0,     20},
	{20,     20,     20,     20,     20,     20,     20,     20},
};
static const unsigned char rush_ban_test11[8][8]={
	{20,     20,     20,     20,     20,     20,     20,     20},
	{20, 32*2+1, 32*2+1, 32*1+4,      0,      0,      0,     20},
	{20, 32*1+2,      0, 32*1+4, 32*4+7, 32*4+7, 32*4+7,     20},
	{20, 32*1+2,      0, 32*5+5, 32*5+5, 32*1+8,      0,      0},
	{20,      0,      0,      0,      0, 32*1+8,      0,     20},
	{20, 32*2+3, 32*2+3, 32*4+6, 32*4+6, 32*4+6, 32*1+9,     20},
	{20,      0,      0,      0,      0,      0, 32*1+9,     20},
	{20,     20,     20,     20,     20,     20,     20,     20},
};
static const unsigned char rush_ban_test10[8][8]={
	{20,     20,     20,     20,     20,     20,     20,     20},
	{20,      0,      0,      0, 32*1+4,      0,      0,     20},
	{20,      0,      0,      0, 32*1+4, 32*2+7, 32*2+7,     20},
	{20, 32*5+1, 32*5+1,      0, 32*1+5,      0,      0,      0},
	{20, 32*3+2,      0, 32*3+3, 32*1+5, 32*2+8, 32*2+8,     20},
	{20, 32*3+2,      0, 32*3+3, 32*2+6, 32*2+6, 32*1+9,     20},
	{20, 32*3+2,      0, 32*3+3,      0,      0, 32*1+9,     20},
	{20,     20,     20,     20,     20,     20,     20,     20},
};

#define C (char)
static const char rush_ban_test1[8][8]={	//
	{20,         20,         20,         20,         20,         20,         20,      20},
	{20,       32+1,          0,     32*2+7,     32*2+7,    32*1+10,          0,      20},
	{20,       32+1,     32*2+4,     32*2+4,     32*3+8,    32*1+10,    32*3+12,      20},
	{20,          0,  C(32*5+5),  C(32*5+5),     32*3+8,    32*1+11,    32*3+12,       0},
	{20,     32*2+2,     32*2+2,     32*1+6,     32*3+8,    32*1+11,    32*3+12,      20},
	{20,          0,          0,     32*1+6,  C(32*4+9),  C(32*4+9),  C(32*4+9),      20},
	{20,     32*2+3,     32*2+3,          0,          0,          0,          0,      20},
	{20,         20,         20,         20,         20,         20,         20,      20},
};
static const char rush_ban_test2[8][8]={	//
	{20,         20,         20,         20,         20,         20,         20,      20},
	{20,          0,     32*2+1,     32*2+1,          0,     32*1+4,     32*3+5,      20},
	{20,          0,     32*1+2,          0,          0,     32*1+4,     32*3+5,      20},
	{20,          0,     32*1+2,  C(32*5+3),  C(32*5+3),          0,     32*3+5,       0},
	{20,          0,          0,          0,     32*1+9,          0,          0,      20},
	{20,     32*1+6,     32*1+7,     32*1+8,     32*1+9,    32*2+10,    32*2+10,      20},
	{20,     32*1+6,     32*1+7,     32*1+8, C(32*4+11), C(32*4+11), C(32*4+11),      20},
	{20,         20,         20,         20,         20,         20,         20,      20},
};
static const char rush_ban_test3[8][8]={	// 解いた
	{20,         20,         20,         20,        20,          20,         20,      20},
	{20,          0,          0,     32*3+1,         0,      32*2+2,     32*2+2,      20},
	{20,          0,          0,     32*3+1,         0,           0,          0,      20},
	{20,  C(32*5+3),  C(32*5+3),     32*3+1,         0,           0,          0,       0},
	{20,  C(32*4+4),  C(32*4+4),  C(32*4+4),         0,           0,     32*3+5,      20},
	{20,          0,          0,          0,         0,           0,     32*3+5,      20},
	{20,          0,          0,          0,         0,           0,     32*3+5,      20},
	{20,         20,         20,         20,        20,          20,         20,      20},
};
static const char rush_ban_test4[8][8]={ // 解いた
	{20,         20,         20,         20,         20,         20,         20,      20},
	{20,  C(32*4+1),  C(32*4+1),  C(32*4+1),     32*1+2,     32*3+3,     32*1+4,      20},
	{20,          0,          0,          0,     32*1+2,     32*3+3,     32*1+4,      20},
	{20,  C(32*5+7),  C(32*5+7),    32*3+10,          0,     32*3+3,     32*1+5,       0},
	{20,     32*1+8,     32*1+9,    32*3+10,          0,          0,     32*1+5,      20},
	{20,     32*1+8,     32*1+9,    32*3+10,          0,     32*2+6,     32*2+6,      20},
	{20,          0,          0,          0,          0,          0,          0,      20},
	{20,         20,         20,         20,         20,         20,         20,      20},
};
static const char rush_ban_test5[8][8]={	// 
	{20,         20,         20,         20,         20,         20,         20,      20},
	{20,     32*3+1,  C(32*4+2),  C(32*4+2),  C(32*4+2),          0,     32*3+6,      20},
	{20,     32*3+1,          0,          0,     32*1+4,     32*1+5,     32*3+6,      20},
	{20,     32*3+1,  C(32*5+3),  C(32*5+3),     32*1+4,     32*1+5,     32*3+6,       0},
	{20,          0,     32*1+7,     32*1+8,  C(32*4+9),  C(32*4+9),  C(32*4+9),      20},
	{20,          0,     32*1+7,     32*1+8,    32*1+10,          0,          0,      20},
	{20,          0,          0,          0,    32*1+10,          0,          0,      20},
	{20,         20,         20,         20,         20,         20,         20,      20},
};
static const char rush_ban_test6[8][8]={	// 
	{20,         20,         20,         20,         20,         20,         20,      20},
	{20,     32*1+1,          0,          0,     32*3+4,     32*2+5,     32*2+5,      20},
	{20,     32*1+1,          0,     32*1+3,     32*3+4,          0,     32*3+7,      20},
	{20,  C(32*5+2),  C(32*5+2),     32*1+3,     32*3+4,     32*1+6,     32*3+7,       0},
	{20,          0,          0,          0,          0,     32*1+6,     32*3+7,      20},
	{20,     32*1+8,     32*1+9,    32*2+10,    32*2+10,          0,          0,      20},
	{20,     32*1+8,     32*1+9,          0, C(32*4+11), C(32*4+11), C(32*4+11),      20},
	{20,         20,         20,         20,         20,         20,         20,      20},
};
static const char rush_ban_test7[8][8]={	// 
	{20,         20,         20,         20,         20,         20,         20,      20},
	{20,     32*3+1,     32*2+2,     32*2+2,          0,     32*3+5,     32*3+6,      20},
	{20,     32*3+1,     32*1+3,          0,          0,     32*3+5,     32*3+6,      20},
	{20,     32*3+1,     32*1+3,  C(32*5+4),  C(32*5+4),     32*3+5,     32*3+6,       0},
	{20,          0,          0,          0,     32*1+9,    32*2+11,    32*2+11,      20},
	{20,          0,          0,     32*1+8,     32*1+9,    32*2+12,    32*2+12,      20},
	{20,     32*2+7,     32*2+7,     32*1+8,    32*2+10,    32*2+10,          0,      20},
	{20,         20,         20,         20,         20,         20,         20,      20},
};
static const char rush_ban_test8[8][8]={	// 
	{20,         20,         20,         20,         20,         20,         20,      20},
	{20,     32*1+1,          0,          0,     32*2+5,     32*2+5,     32*3+7,      20},
	{20,     32*1+1,     32*2+2,     32*2+2,     32*1+6,          0,     32*3+7,      20},
	{20,  C(32*5+3),  C(32*5+3),     32*1+4,     32*1+6,          0,     32*3+7,       0},
	{20,          0,          0,     32*1+4,  C(32*4+8),  C(32*4+8),  C(32*4+8),      20},
	{20,          0,          0,          0,    32*1+10,    32*2+11,    32*2+11,      20},
	{20,          0,     32*2+9,     32*2+9,    32*1+10,    32*2+12,    32*2+12,      20},
	{20,         20,         20,         20,         20,         20,         20,      20},
};
static const char rush_ban_test9[8][8]={	// 
	{20,         20,         20,         20,         20,         20,         20,      20},
	{20,       32+1,          0,     32*2+5,     32*2+5,    32*3+11,          0,      20},
	{20,       32+1,          0,       32+6,       32+7,    32*3+11,    32*3+12,      20},
	{20,  C(32*5+2),  C(32*5+2),       32+6,       32+7,    32*3+11,    32*3+12,       0},
	{20,          0,       32+3,  C(32*4+8),  C(32*4+8),  C(32*4+8),    32*3+12,      20},
	{20,          0,       32+3,     32*2+9,     32*2+9,    32*2+10,    32*2+10,      20},
	{20,  C(32*4+4),  C(32*4+4),  C(32*4+4),          0,          0,          0,      20},
	{20,         20,         20,         20,         20,         20,         20,      20},
};

static const char rush_ban_init_data[8][8]={
	{20,20,20,20,20,20,20,20},
	{20, 0, 0, 0, 0, 0, 0,20},
	{20, 0, 0, 0, 0, 0, 0,20},
	{20, 0, 0, 0, 0, 0, 0, 0},
	{20, 0, 0, 0, 0, 0, 0,20},
	{20, 0, 0, 0, 0, 0, 0,20},
	{20, 0, 0, 0, 0, 0, 0,20},
	{20,20,20,20,20,20,20,20},
};
static const char rush_color[21]={
	BLACK,								// 車無しの色
	BRUE,YELLOW_D,MAGENTA,GREEN,CYAN,YELLOW,GRAY_D,GRAY,BRUE_U,BRUE_D,RED_U,RED_D,
	MAGENTA_U,MAGENTA_D,GREEN_U,GREEN_D,CYAN_U,CYAN_D,YELLOW_U,
	WHITE								// 壁の色
};

#define R RED
static const short pat1[20*20*3];
static const short pat2[20*20*2]={
	R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
	R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
	R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
	R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
	R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
	R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
	R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
	R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
};

//static char *rush_msg1, *rush_msg2, *rush_msg3, *rush_msg4;
/********************************************************************************/
/*		rush_ban_init															*/
/********************************************************************************/
//static void rush_ban_init()
//{
//	memcpy(rush_ban, rush_ban_init_data, sizeof(rush_ban));
//}
/********************************************************************************/
/*		rush_koma_disp															*/
/*		x,y で指定した位置に正方形を１個表示する、色指定もできる				*/
/********************************************************************************/
static void rush_koma_disp(int x, int y, int color)
{
	lcd_clearA(40+14*x, 20*y, 40+13+14*x, 19+20*y, color);
}
/********************************************************************************/
/*		rush_disp_ban															*/
/*		盤を表示する															*/
/********************************************************************************/
static void rush_disp_ban()
{
	int x, y, num, car;
	
	for(y=0; y<8; y++){
		for(x=0; x<8; x++){
			car = (rush_ban[y][x] >> 5) & 7;
			num = rush_ban[y][x] & 0x1f;
			if(car <= 4){
				rush_koma_disp(x, y, rush_color[num]);
			}
			else{					// 赤い車
				rush_koma_disp(x, y, RED);	// 赤
			}
		}
	}
	for(y=1; y<7; y++){
		for(x=0; x<7; x++){
			if(rush_ban[y][x] != rush_ban[y][x+1])
				lcd_clearA(40+13+14*x, 20*y, 40+14+14*x, 19+20*y, BLACK);
		}
	}
	for(x=1; x<7; x++){
		for(y=0; y<7; y++){
			if(rush_ban[y][x] != rush_ban[y+1][x])
				lcd_clearA(40+14*x, 19+20*y, 40+13+14*x, 20+20*y, BLACK);
		}
	}
}
/********************************************************************************/
/*		rush_disp_car															*/
/*		車を１台表示する														*/
/*		car: 1～6 の車番号														*/
/*		str: 全角１文字															*/
/********************************************************************************/
#if 0
static void rush_disp_car(int x, int y, int car, int tooshi)
{
	int x1, y1, car1;
	
	for(y1=0; y1<3; y1++){
		for(x1=0; x1<3; x1++){
			if(car <= 4){
				if(rush_car_pat[car][y1][x1]){
					rush_koma_disp(x+x1, y+y1, rush_color[tooshi]);
				}
			}
			else{					// 赤い車
				car1 = car==5 ? 2 : 4;
				if(rush_car_pat[car1][y1][x1]){
					rush_koma_disp(x+x1, y+y1, RED);	// 背景色　赤
				}
			}
		}
	}
}
#endif
/********************************************************************************/
/*		rush_put_car_test														*/
/*		return 1: 置ける、0:置けない											*/
/********************************************************************************/
static int rush_put_car_test(char ban[8][8], int x, int y, int car_numb)
{
	int x1, y1;
	
	for(y1=0; y1<3; y1++){
		for(x1=0; x1<3; x1++){
			if(ban[y+y1][x+x1] && rush_car_pat[car_numb][y1][x1])
				return 0;				// 置けない
		}
	}
	return 1;							// 置ける
}
/********************************************************************************/
/*		rush_make_param															*/
/*		x,y 位置の駒から car_numb と tooshi_bango を作る						*/
/********************************************************************************/
static void rush_make_param(int x, int y, int *p_car_numb, int *p_tooshi_bango)
{
	*p_car_numb = (rush_ban[y][x] >> 5) & 7;
	*p_tooshi_bango = rush_ban[y][x] & 0x1f;
}
/********************************************************************************/
/*		rush_move_car															*/
/*		車を移動して遊ぶ														*/
/********************************************************************************/
static void rush_move_car()
{
	int cursor, tmp, i, sw, koma, x, y, x1, y1, nextx, nexty, car_numb, tooshibango;
	
	cursor = 1;
	x = y = x1 = y1 = 0;
    lcd_clear(BLACK);
	rush_disp_ban();
	for(;;){
		rush_disp_ban();
		/***	車を選択する	***/
		for(x=1; x<=6; x++){
			for(y=1; y<=6; y++){
				if(cursor==(rush_ban[y][x] & 0x1f))
					goto next1;
			}
		}
next1:;
		lcd_clearA(40+14*x+5, 20*y+7, 40+13+14*x-5, 19+20*y-7, BLACK);
		sw = sw_updown();
		if(sw & (A_SW | B_SW)){
			if(sw & A_SW){
				tmp = cursor+1;
			}
			else{
				tmp = cursor-1;
				if(tmp<=0)
					tmp = 1;
			}
			for(x=1; x<=6; x++){
				for(y=1; y<=6; y++){
					if(tmp==(rush_ban[y][x] & 0x1f)){
						cursor = tmp;
					}
				}
			}
			continue;
		}
		/***	車を移動する	***/
		rush_make_param(x, y, &car_numb, &tooshibango);	// パラメータ作成
		
		koma = (unsigned char)rush_ban[y][x];	// x,y を左上にする
		while(koma==(unsigned char)rush_ban[y][x-1]){
			x--;
		}
		while(koma==(unsigned char)rush_ban[y-1][x]){
			y--;
		}

		/* 移動したかテストする	*/
		if(car_numb==1 || car_numb==3){	// 上下移動
			nextx = x;
			if(sw & DOWN)
				nexty = y+1;
			else if(sw & UP)
				nexty = y-1;
			else
				continue;
		}
		else{							// 左右移動
			nexty = y;
			if(sw & RIGHT)
				nextx = x+1;
			else if(sw & LEFT)
				nextx = x-1;
			else
				continue;
		}
		/***	移動できるかテストする	***/
		for(i=0; i<sizeof(rush_ban); i++){		// rush_ban から koma 削除
			if(koma==((unsigned char*)rush_ban)[i])
				((unsigned char*)rush_ban)[i] = 0;
		}
		if(rush_put_car_test(rush_ban, nextx, nexty, car_numb)){	// 移動できる
			x = nextx;								// 現在位置更新
			y = nexty;
		}
		for(y1=0; y1<3; y1++){				// 現在位置に駐車する
			for(x1=0; x1<3; x1++){
				if(rush_car_pat[car_numb][y1][x1]){
					rush_ban[y+y1][x+x1] = (car_numb<<5) + tooshibango;
				}
			}
		}
		rush_disp_ban();
		if(((rush_ban[3][7] >> 5) & 0x7)==5){	// パズル完成
			str_cdisp(0,"おめでとう御座います");
			wait_ms(2000);
			sw_updown();
			return;
		}
	}
}
/********************************************************************************/
/*		main																	*/
/********************************************************************************/
//const struct menu rush_file_menu[]={
//	{0,  1, "　例題１　"},
//	{15, 2, "　例題２　"},
//	{30, 3, "　例題３　"},
//	{45, 4, "　例題４　"},
//	{60, 5, "　例題５　"},
//	{75, 6, "　例題６　"},
//	{90, 7, "　例題７　"},
//	{105,8, "　例題８　"},
//	{120,9, "　例題９　"},
//	{0,0,0},
//};

void rush_main()
{
	int numb, sw;
	char buf[20];
	char *rush_test[]={
		(char*)rush_ban_test1, (char*)rush_ban_test2, (char*)rush_ban_test3, (char*)rush_ban_test4, (char*)rush_ban_test5,
		(char*)rush_ban_test6, (char*)rush_ban_test7, (char*)rush_ban_test8, (char*)rush_ban_test9, (char*)rush_ban_test10,
		(char*)rush_ban_test11,
	};
	
	lcd_clear(YELLOW);
	str_cdisp(60, "　ラッシュアワーゲーム　");
	str_cdisp(80, "K.Fukumoto");
	sw_updown();
	
	lcd_clear(BLACK);
    numb = 1;
	for(;;){
		memcpy(rush_ban, rush_test[numb-1], sizeof(rush_ban));
        rush_disp_ban();
		str_disp(0,0, "選択");
		sprintf(buf, "No%d  ", numb);
		str_disp(0,15, buf);
        for(;;){
			sw = sw_updown();
			if(sw & UP){
				if(--numb < 1)
					numb = 1;
                break;
			}
			else if(sw & DOWN){
				if(++numb > 11)
					numb = 11;
                break;
			}
			else if(sw & (A_SW | B_SW)){
				lcd_clear(BLACK);
				rush_move_car();			// 車を移動して遊ぶ
				return;
			}
        }
	}
}
