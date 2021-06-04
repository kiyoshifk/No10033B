#include "app.h"


#define macrox(pos) ((pos)%6)
#define macroy(pos) ((pos)/6)
#define macropos(x,y) ((x)+(y)*6)
#define U (-6)
#define D 6
#define L (-1)
#define R 1


extern const char hako_kif[][7*6];

static const char hako_color[12]={
	BLACK,RED,CYAN,BRUE,GREEN,MAGENTA,YELLOW,BRUE_U,RED_U,GREEN_U,CYAN_D,BRUE_D,
};
static const char hako_ban_dat[7][6]={
	{-100, -100, -100, -100, -100,-100},
	{-100,   11,   22,   22,   31,-100},
	{-100,   11,   22,   22,   31,-100},
	{-100,   41,   53,   53,   61,-100},
	{-100,   41,   74,   84,   61,-100},
	{-100,   94,    0,    0,  104,-100},
	{-100, -100, -100, -100, -100,-100}
};

static char hako_ban[7][6];	//	aab ... 十進数
//	hako_ban[y][x]
//	a:1〜10 通し番号
//	b:1〜4  駒番号

static const char hako_koma_pat[5][2][2]={
	{
		{0,0},			// dummy data
		{0,0},
	},
	{
		{1,0},			// 駒番号１
		{1,0},
	},
	{
		{1,1},			// 駒番号２
		{1,1},
	},
	{
		{1,1},			// 駒番号３
		{0,0},
	},
	{
		{1,0},			// 駒番号４
		{0,0},
	}
};

static char *hako_banp = (char*)hako_ban;


static int hako_put_koma_test(char ban[7][6], int x, int y, int koma_numb);
static void hako_put_koma(char ban[7][6], int x, int y, int koma);
static void hako_remove_koma(char ban[7][6], int koma);
static void hako_disp_ban();

/********************************************************************************/
/*		hako_play																*/
/********************************************************************************/
void hako_play()
{
	int cursor, sw, x, y, nextx, nexty, koma, koma_numb;
	
	lcd_clear(GREEN);
	str_cdisp(60, "箱入り娘　プレイ");
	str_cdisp(75, "A/B SW: select");
	str_cdisp(85, "矢印キー：移動");
	sw_updown();
	lcd_clear(BLACK);
	
	cursor = 1;
	memcpy(hako_ban, hako_ban_dat, sizeof(hako_ban));
	hako_disp_ban();					// 表示
	for(;;){
		hako_disp_ban();
		/***	駒を選択する	***/
		for(x=1; x<=4; x++){
			for(y=1; y<=5; y++){
				if(cursor==hako_ban[y][x]/10)
					goto next1;
			}
		}
next1:;
		lcd_clearA(38+(x-1)*14+5, 5+(y-1)*20+7, 38+(x-1)*14+5+4, 5+(y-1)*20+7+6, BLACK);
		/***	駒を移動する	***/
		sw = sw_updown();
		if(sw & A_SW){
			if(++cursor > 10)
				cursor = 1;
			continue;
		}
		else if(sw & B_SW){
			if(--cursor < 1)
				cursor = 10;
			continue;
		}
		koma = (unsigned char)hako_ban[y][x];
		koma_numb = koma % 10;
		while(koma==(unsigned char)hako_ban[y][x-1]){	// x,y を左上にする
			x--;
		}
		while(koma==(unsigned char)hako_ban[y-1][x]){
			y--;
		}
		
		nextx = x;
		nexty = y;
		if(sw & DOWN)
			nexty = y+1;
		else if(sw & UP)
			nexty = y-1;
		else if(sw & RIGHT)
			nextx = x+1;
		else if(sw & LEFT)
			nextx = x-1;
		else
			continue;
		/***	移動できるかテスト	***/
		hako_remove_koma(hako_ban, koma);
		if(hako_put_koma_test(hako_ban, nextx, nexty, koma_numb)){	// 移動できる
			x = nextx;
			y = nexty;
		}
		hako_put_koma(hako_ban, x, y, koma);	// 駒を置く
	}
}
/********************************************************************************/
/*		hako_put_koma_test														*/
/*		return 1: 置ける、0:置けない											*/
/********************************************************************************/
static int hako_put_koma_test(char ban[7][6], int x, int y, int koma_numb)
{
	int x1, y1;
	
	for(y1=0; y1<2; y1++){
		for(x1=0; x1<2; x1++){
			if(ban[y+y1][x+x1] && hako_koma_pat[koma_numb][y1][x1])
				return 0;				// 置けない
		}
	}
	return 1;							// 置ける
}
/********************************************************************************/
/*		hako_put_koma															*/
/********************************************************************************/
static void hako_put_koma(char ban[7][6], int x, int y, int koma)
{
	int koma_numb = koma % 10;
	int x1, y1;
	
	for(y1=0; y1<2; y1++){
		for(x1=0; x1<2; x1++){
			if(hako_koma_pat[koma_numb][y1][x1])
				ban[y+y1][x+x1] = koma;	// put koma
		}
	}
}
/********************************************************************************/
/*		hako_remove_koma														*/
/********************************************************************************/
static void hako_remove_koma(char ban[7][6], int koma)
{
	int x, y;
	
	for(y=0; y<7; y++){
		for(x=0; x<6; x++){
			if(ban[y][x]==koma)
				ban[y][x] = 0;			// remove
		}
	}
}
/********************************************************************************/
/*		hako_disp_ban															*/
/********************************************************************************/
static void hako_disp_ban()
{
	int i, x, y, k, p[5], pcnt, pos, u, d, r, l, color;
	
	drawcount = 0;
	while(drawcount==0)
		;
	for(k=0; k<=10; k++){					// 駒ループ
		//	k 駒の正方形ユニットの位置を p[] に入れる
		memset(p, 0, sizeof(p));
		pcnt = 0;
		for(pos=0; pos<7*6; pos++){
			if(k==hako_banp[pos]/10){
				p[pcnt++] = pos;
				if(pcnt>=5){
					printf("hako_disp_ban err k=%d\n", k);
					for(;;)
						;
				}
			}
		}
		color = hako_color[k];
		for(i=0; i<pcnt; i++){
			x = macrox(p[i]);
			y = macroy(p[i]);
			lcd_clearA(38+(x-1)*14, 5+(y-1)*20, 38+x*14-1, 5+y*20-1, color);
			if(k){
				u = d = r = l = 1;
				if(hako_banp[p[i]+U]/10==k)		// 同じ駒が上に続く
					u = 0;
				if(hako_banp[p[i]+D]/10==k)
					d = 0;
				if(hako_banp[p[i]+R]/10==k)
					r = 0;
				if(hako_banp[p[i]+L]/10==k)
					l = 0;
				if(u)
					lcd_clearA(38+(x-1)*14, 5+(y-1)*20, 38+x*14-1,   5+(y-1)*20, WHITE);
				if(d)
					lcd_clearA(38+(x-1)*14,   5+y*20-1, 38+x*14-1,   5+y*20-1,   WHITE);
				if(l)
					lcd_clearA(38+(x-1)*14, 5+(y-1)*20, 38+(x-1)*14, 5+y*20-1,   WHITE);
				if(r)
					lcd_clearA(38+x*14-1,   5+(y-1)*20, 38+x*14-1,   5+y*20-1,   WHITE);
			}
		}
	}
	lcd_clearA(38-1,     5-1, 38+4*14,     5-1, WHITE);
	lcd_clearA(38-1,  5+5*20, 38+4*14,  5+5*20, WHITE);
	lcd_clearA(38-1,     5-1,  38-1,  5+5*20, WHITE);
	lcd_clearA(38+4*14,  5-1, 38+4*14,  5+5*20, WHITE);
}
/********************************************************************************/
/*		hako_main																*/
/********************************************************************************/
const struct menu hako_menu[]={
	{0, 60,  1, "箱入り娘表\示"},
	{0, 80,  2, "箱入り娘プレイ"},
	{0,0,0,0}
};

void hako_main()
{
	int i, j, sw, cmd;
	
	lcd_clear(GRAY);
	for(;;){
		cmd = menu_select(hako_menu);
		if(cmd==1)
			break;
		else if(cmd==2){
			hako_play();
			return;
		}
	}
	lcd_clear(BLACK);
	
	srand(TickCount);
	memcpy(hako_ban, hako_ban_dat, sizeof(hako_ban));
	hako_disp_ban();								// 表示
	sw_updown();
	for(i=0; ; i++){
		if(hako_kif[i][0]==0)
			break;
		memcpy(hako_ban, hako_kif[i], 7*6);
		hako_disp_ban();
		for(j=0; j<5; j++){
			sw = sw_sense();
//			if(sw)
//				sw_downup();
            if(sw & B_SW)
                return;
			wait_ms(100);
		}
	}
	sw_updown();
}




const char hako_kif[][7*6]={
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x35,0x35,0x3d,0x9c,0x9c,0x29,0x4a,0x54,0x3d,0x9c,0x9c,0x5e,0x00,0x00,0x68,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x35,0x35,0x3d,0x9c,0x9c,0x29,0x00,0x54,0x3d,0x9c,0x9c,0x5e,0x4a,0x00,0x68,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x35,0x35,0x3d,0x9c,0x9c,0x29,0x00,0x54,0x3d,0x9c,0x9c,0x5e,0x4a,0x68,0x00,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x35,0x35,0x00,0x9c,0x9c,0x29,0x00,0x54,0x3d,0x9c,0x9c,0x5e,0x4a,0x68,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x00,0x35,0x35,0x9c,0x9c,0x29,0x00,0x54,0x3d,0x9c,0x9c,0x5e,0x4a,0x68,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x00,0x29,0x35,0x35,0x9c,0x9c,0x00,0x29,0x54,0x3d,0x9c,0x9c,0x5e,0x4a,0x68,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x00,0x29,0x35,0x35,0x9c,0x9c,0x5e,0x29,0x54,0x3d,0x9c,0x9c,0x00,0x4a,0x68,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x00,0x29,0x35,0x35,0x9c,0x9c,0x5e,0x29,0x54,0x3d,0x9c,0x9c,0x4a,0x00,0x68,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x00,0x00,0x35,0x35,0x9c,0x9c,0x5e,0x29,0x54,0x3d,0x9c,0x9c,0x4a,0x29,0x68,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x00,0x35,0x35,0x00,0x9c,0x9c,0x5e,0x29,0x54,0x3d,0x9c,0x9c,0x4a,0x29,0x68,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x35,0x35,0x00,0x00,0x9c,0x9c,0x5e,0x29,0x54,0x3d,0x9c,0x9c,0x4a,0x29,0x68,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x35,0x35,0x54,0x00,0x9c,0x9c,0x5e,0x29,0x00,0x3d,0x9c,0x9c,0x4a,0x29,0x68,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x35,0x35,0x00,0x54,0x9c,0x9c,0x5e,0x29,0x00,0x3d,0x9c,0x9c,0x4a,0x29,0x68,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x35,0x35,0x00,0x54,0x9c,0x9c,0x5e,0x29,0x68,0x3d,0x9c,0x9c,0x4a,0x29,0x00,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x35,0x35,0x68,0x54,0x9c,0x9c,0x5e,0x29,0x00,0x3d,0x9c,0x9c,0x4a,0x29,0x00,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x35,0x35,0x68,0x54,0x9c,0x9c,0x5e,0x00,0x29,0x3d,0x9c,0x9c,0x4a,0x00,0x29,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x35,0x35,0x68,0x54,0x9c,0x9c,0x00,0x5e,0x29,0x3d,0x9c,0x9c,0x4a,0x00,0x29,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x35,0x35,0x68,0x54,0x9c,0x9c,0x00,0x00,0x29,0x3d,0x9c,0x9c,0x4a,0x5e,0x29,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x00,0x00,0x68,0x54,0x9c,0x9c,0x35,0x35,0x29,0x3d,0x9c,0x9c,0x4a,0x5e,0x29,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x00,0x68,0x00,0x54,0x9c,0x9c,0x35,0x35,0x29,0x3d,0x9c,0x9c,0x4a,0x5e,0x29,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x68,0x00,0x00,0x54,0x9c,0x9c,0x35,0x35,0x29,0x3d,0x9c,0x9c,0x4a,0x5e,0x29,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x68,0x00,0x54,0x00,0x9c,0x9c,0x35,0x35,0x29,0x3d,0x9c,0x9c,0x4a,0x5e,0x29,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x68,0x54,0x00,0x00,0x9c,0x9c,0x35,0x35,0x29,0x3d,0x9c,0x9c,0x4a,0x5e,0x29,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x68,0x54,0x29,0x00,0x9c,0x9c,0x35,0x35,0x29,0x3d,0x9c,0x9c,0x4a,0x5e,0x00,0x3d,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x68,0x54,0x29,0x3d,0x9c,0x9c,0x35,0x35,0x29,0x3d,0x9c,0x9c,0x4a,0x5e,0x00,0x00,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x68,0x54,0x29,0x3d,0x9c,0x9c,0x35,0x35,0x29,0x3d,0x9c,0x9c,0x4a,0x00,0x5e,0x00,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x68,0x54,0x29,0x3d,0x9c,0x9c,0x35,0x35,0x29,0x3d,0x9c,0x9c,0x4a,0x00,0x00,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x68,0x54,0x29,0x3d,0x9c,0x9c,0x35,0x35,0x29,0x3d,0x9c,0x9c,0x00,0x4a,0x00,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x68,0x54,0x29,0x3d,0x9c,0x9c,0x35,0x35,0x29,0x3d,0x9c,0x9c,0x00,0x00,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x68,0x54,0x29,0x3d,0x9c,0x9c,0x00,0x00,0x29,0x3d,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x68,0x00,0x29,0x3d,0x9c,0x9c,0x00,0x54,0x29,0x3d,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x68,0x00,0x29,0x3d,0x9c,0x9c,0x54,0x00,0x29,0x3d,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x68,0x29,0x00,0x3d,0x9c,0x9c,0x54,0x29,0x00,0x3d,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x68,0x29,0x3d,0x00,0x9c,0x9c,0x54,0x29,0x3d,0x00,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x00,0x9c,0x9c,0x0b,0x16,0x16,0x1f,0x9c,0x9c,0x68,0x29,0x3d,0x1f,0x9c,0x9c,0x54,0x29,0x3d,0x00,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x16,0x16,0x00,0x9c,0x9c,0x0b,0x16,0x16,0x00,0x9c,0x9c,0x68,0x29,0x3d,0x1f,0x9c,0x9c,0x54,0x29,0x3d,0x1f,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x00,0x16,0x16,0x9c,0x9c,0x0b,0x00,0x16,0x16,0x9c,0x9c,0x68,0x29,0x3d,0x1f,0x9c,0x9c,0x54,0x29,0x3d,0x1f,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x00,0x0b,0x16,0x16,0x9c,0x9c,0x00,0x0b,0x16,0x16,0x9c,0x9c,0x68,0x29,0x3d,0x1f,0x9c,0x9c,0x54,0x29,0x3d,0x1f,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x00,0x0b,0x16,0x16,0x9c,0x9c,0x68,0x0b,0x16,0x16,0x9c,0x9c,0x00,0x29,0x3d,0x1f,0x9c,0x9c,0x54,0x29,0x3d,0x1f,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x0b,0x16,0x16,0x9c,0x9c,0x00,0x0b,0x16,0x16,0x9c,0x9c,0x00,0x29,0x3d,0x1f,0x9c,0x9c,0x54,0x29,0x3d,0x1f,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x0b,0x16,0x16,0x9c,0x9c,0x00,0x0b,0x16,0x16,0x9c,0x9c,0x54,0x29,0x3d,0x1f,0x9c,0x9c,0x00,0x29,0x3d,0x1f,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x0b,0x16,0x16,0x9c,0x9c,0x54,0x0b,0x16,0x16,0x9c,0x9c,0x00,0x29,0x3d,0x1f,0x9c,0x9c,0x00,0x29,0x3d,0x1f,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x0b,0x16,0x16,0x9c,0x9c,0x54,0x0b,0x16,0x16,0x9c,0x9c,0x29,0x00,0x3d,0x1f,0x9c,0x9c,0x29,0x00,0x3d,0x1f,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x00,0x16,0x16,0x9c,0x9c,0x54,0x0b,0x16,0x16,0x9c,0x9c,0x29,0x0b,0x3d,0x1f,0x9c,0x9c,0x29,0x00,0x3d,0x1f,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x00,0x16,0x16,0x9c,0x9c,0x54,0x00,0x16,0x16,0x9c,0x9c,0x29,0x0b,0x3d,0x1f,0x9c,0x9c,0x29,0x0b,0x3d,0x1f,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x16,0x16,0x00,0x9c,0x9c,0x54,0x16,0x16,0x00,0x9c,0x9c,0x29,0x0b,0x3d,0x1f,0x9c,0x9c,0x29,0x0b,0x3d,0x1f,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x16,0x16,0x00,0x9c,0x9c,0x54,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x0b,0x3d,0x1f,0x9c,0x9c,0x29,0x0b,0x3d,0x00,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x16,0x16,0x1f,0x9c,0x9c,0x54,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x0b,0x3d,0x00,0x9c,0x9c,0x29,0x0b,0x3d,0x00,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x16,0x16,0x1f,0x9c,0x9c,0x54,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x0b,0x00,0x3d,0x9c,0x9c,0x29,0x0b,0x00,0x3d,0x9c,0x9c,0x35,0x35,0x4a,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x16,0x16,0x1f,0x9c,0x9c,0x54,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x0b,0x00,0x3d,0x9c,0x9c,0x29,0x0b,0x4a,0x3d,0x9c,0x9c,0x35,0x35,0x00,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x16,0x16,0x1f,0x9c,0x9c,0x54,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x0b,0x4a,0x3d,0x9c,0x9c,0x29,0x0b,0x00,0x3d,0x9c,0x9c,0x35,0x35,0x00,0x5e,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x16,0x16,0x1f,0x9c,0x9c,0x54,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x0b,0x4a,0x3d,0x9c,0x9c,0x29,0x0b,0x00,0x3d,0x9c,0x9c,0x35,0x35,0x5e,0x00,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x16,0x16,0x1f,0x9c,0x9c,0x54,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x0b,0x4a,0x3d,0x9c,0x9c,0x29,0x0b,0x5e,0x3d,0x9c,0x9c,0x35,0x35,0x00,0x00,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x16,0x16,0x1f,0x9c,0x9c,0x54,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x0b,0x4a,0x3d,0x9c,0x9c,0x29,0x0b,0x5e,0x3d,0x9c,0x9c,0x00,0x35,0x35,0x00,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x16,0x16,0x1f,0x9c,0x9c,0x54,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x0b,0x4a,0x3d,0x9c,0x9c,0x29,0x0b,0x5e,0x3d,0x9c,0x9c,0x00,0x00,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x16,0x16,0x1f,0x9c,0x9c,0x54,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x00,0x4a,0x3d,0x9c,0x9c,0x29,0x0b,0x5e,0x3d,0x9c,0x9c,0x00,0x0b,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x16,0x16,0x1f,0x9c,0x9c,0x54,0x16,0x16,0x1f,0x9c,0x9c,0x00,0x00,0x4a,0x3d,0x9c,0x9c,0x29,0x0b,0x5e,0x3d,0x9c,0x9c,0x29,0x0b,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x16,0x16,0x1f,0x9c,0x9c,0x54,0x16,0x16,0x1f,0x9c,0x9c,0x00,0x4a,0x00,0x3d,0x9c,0x9c,0x29,0x0b,0x5e,0x3d,0x9c,0x9c,0x29,0x0b,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x16,0x16,0x1f,0x9c,0x9c,0x54,0x16,0x16,0x1f,0x9c,0x9c,0x4a,0x00,0x00,0x3d,0x9c,0x9c,0x29,0x0b,0x5e,0x3d,0x9c,0x9c,0x29,0x0b,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x68,0x00,0x00,0x1f,0x9c,0x9c,0x54,0x16,0x16,0x1f,0x9c,0x9c,0x4a,0x16,0x16,0x3d,0x9c,0x9c,0x29,0x0b,0x5e,0x3d,0x9c,0x9c,0x29,0x0b,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x00,0x68,0x00,0x1f,0x9c,0x9c,0x54,0x16,0x16,0x1f,0x9c,0x9c,0x4a,0x16,0x16,0x3d,0x9c,0x9c,0x29,0x0b,0x5e,0x3d,0x9c,0x9c,0x29,0x0b,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x00,0x00,0x68,0x1f,0x9c,0x9c,0x54,0x16,0x16,0x1f,0x9c,0x9c,0x4a,0x16,0x16,0x3d,0x9c,0x9c,0x29,0x0b,0x5e,0x3d,0x9c,0x9c,0x29,0x0b,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x54,0x00,0x68,0x1f,0x9c,0x9c,0x00,0x16,0x16,0x1f,0x9c,0x9c,0x4a,0x16,0x16,0x3d,0x9c,0x9c,0x29,0x0b,0x5e,0x3d,0x9c,0x9c,0x29,0x0b,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x00,0x54,0x68,0x1f,0x9c,0x9c,0x00,0x16,0x16,0x1f,0x9c,0x9c,0x4a,0x16,0x16,0x3d,0x9c,0x9c,0x29,0x0b,0x5e,0x3d,0x9c,0x9c,0x29,0x0b,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x00,0x54,0x68,0x1f,0x9c,0x9c,0x4a,0x16,0x16,0x1f,0x9c,0x9c,0x00,0x16,0x16,0x3d,0x9c,0x9c,0x29,0x0b,0x5e,0x3d,0x9c,0x9c,0x29,0x0b,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x4a,0x54,0x68,0x1f,0x9c,0x9c,0x00,0x16,0x16,0x1f,0x9c,0x9c,0x00,0x16,0x16,0x3d,0x9c,0x9c,0x29,0x0b,0x5e,0x3d,0x9c,0x9c,0x29,0x0b,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x4a,0x54,0x68,0x1f,0x9c,0x9c,0x00,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x16,0x16,0x3d,0x9c,0x9c,0x29,0x0b,0x5e,0x3d,0x9c,0x9c,0x00,0x0b,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x4a,0x54,0x68,0x1f,0x9c,0x9c,0x29,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x16,0x16,0x3d,0x9c,0x9c,0x00,0x0b,0x5e,0x3d,0x9c,0x9c,0x00,0x0b,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x4a,0x54,0x68,0x1f,0x9c,0x9c,0x29,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x16,0x16,0x3d,0x9c,0x9c,0x0b,0x00,0x5e,0x3d,0x9c,0x9c,0x0b,0x00,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x4a,0x54,0x68,0x1f,0x9c,0x9c,0x29,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x16,0x16,0x3d,0x9c,0x9c,0x0b,0x5e,0x00,0x3d,0x9c,0x9c,0x0b,0x00,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x4a,0x54,0x68,0x1f,0x9c,0x9c,0x29,0x16,0x16,0x1f,0x9c,0x9c,0x29,0x16,0x16,0x3d,0x9c,0x9c,0x0b,0x00,0x00,0x3d,0x9c,0x9c,0x0b,0x5e,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x4a,0x54,0x68,0x1f,0x9c,0x9c,0x29,0x00,0x00,0x1f,0x9c,0x9c,0x29,0x16,0x16,0x3d,0x9c,0x9c,0x0b,0x16,0x16,0x3d,0x9c,0x9c,0x0b,0x5e,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x4a,0x54,0x00,0x1f,0x9c,0x9c,0x29,0x00,0x68,0x1f,0x9c,0x9c,0x29,0x16,0x16,0x3d,0x9c,0x9c,0x0b,0x16,0x16,0x3d,0x9c,0x9c,0x0b,0x5e,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x4a,0x54,0x00,0x1f,0x9c,0x9c,0x29,0x68,0x00,0x1f,0x9c,0x9c,0x29,0x16,0x16,0x3d,0x9c,0x9c,0x0b,0x16,0x16,0x3d,0x9c,0x9c,0x0b,0x5e,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x4a,0x54,0x1f,0x00,0x9c,0x9c,0x29,0x68,0x1f,0x00,0x9c,0x9c,0x29,0x16,0x16,0x3d,0x9c,0x9c,0x0b,0x16,0x16,0x3d,0x9c,0x9c,0x0b,0x5e,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x4a,0x54,0x1f,0x00,0x9c,0x9c,0x29,0x68,0x1f,0x3d,0x9c,0x9c,0x29,0x16,0x16,0x3d,0x9c,0x9c,0x0b,0x16,0x16,0x00,0x9c,0x9c,0x0b,0x5e,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x4a,0x54,0x1f,0x3d,0x9c,0x9c,0x29,0x68,0x1f,0x3d,0x9c,0x9c,0x29,0x16,0x16,0x00,0x9c,0x9c,0x0b,0x16,0x16,0x00,0x9c,0x9c,0x0b,0x5e,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x4a,0x54,0x1f,0x3d,0x9c,0x9c,0x29,0x68,0x1f,0x3d,0x9c,0x9c,0x29,0x00,0x16,0x16,0x9c,0x9c,0x0b,0x00,0x16,0x16,0x9c,0x9c,0x0b,0x5e,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x4a,0x54,0x1f,0x3d,0x9c,0x9c,0x29,0x00,0x1f,0x3d,0x9c,0x9c,0x29,0x68,0x16,0x16,0x9c,0x9c,0x0b,0x00,0x16,0x16,0x9c,0x9c,0x0b,0x5e,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x4a,0x54,0x1f,0x3d,0x9c,0x9c,0x29,0x00,0x1f,0x3d,0x9c,0x9c,0x29,0x00,0x16,0x16,0x9c,0x9c,0x0b,0x68,0x16,0x16,0x9c,0x9c,0x0b,0x5e,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x4a,0x00,0x1f,0x3d,0x9c,0x9c,0x29,0x54,0x1f,0x3d,0x9c,0x9c,0x29,0x00,0x16,0x16,0x9c,0x9c,0x0b,0x68,0x16,0x16,0x9c,0x9c,0x0b,0x5e,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x00,0x4a,0x1f,0x3d,0x9c,0x9c,0x29,0x54,0x1f,0x3d,0x9c,0x9c,0x29,0x00,0x16,0x16,0x9c,0x9c,0x0b,0x68,0x16,0x16,0x9c,0x9c,0x0b,0x5e,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x29,0x4a,0x1f,0x3d,0x9c,0x9c,0x29,0x54,0x1f,0x3d,0x9c,0x9c,0x00,0x00,0x16,0x16,0x9c,0x9c,0x0b,0x68,0x16,0x16,0x9c,0x9c,0x0b,0x5e,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x29,0x4a,0x1f,0x3d,0x9c,0x9c,0x29,0x54,0x1f,0x3d,0x9c,0x9c,0x0b,0x00,0x16,0x16,0x9c,0x9c,0x0b,0x68,0x16,0x16,0x9c,0x9c,0x00,0x5e,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x29,0x4a,0x1f,0x3d,0x9c,0x9c,0x29,0x54,0x1f,0x3d,0x9c,0x9c,0x0b,0x00,0x16,0x16,0x9c,0x9c,0x0b,0x68,0x16,0x16,0x9c,0x9c,0x5e,0x00,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x29,0x4a,0x1f,0x3d,0x9c,0x9c,0x29,0x54,0x1f,0x3d,0x9c,0x9c,0x0b,0x00,0x16,0x16,0x9c,0x9c,0x0b,0x00,0x16,0x16,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x29,0x4a,0x1f,0x3d,0x9c,0x9c,0x29,0x54,0x1f,0x3d,0x9c,0x9c,0x0b,0x16,0x16,0x00,0x9c,0x9c,0x0b,0x16,0x16,0x00,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x29,0x4a,0x1f,0x00,0x9c,0x9c,0x29,0x54,0x1f,0x3d,0x9c,0x9c,0x0b,0x16,0x16,0x3d,0x9c,0x9c,0x0b,0x16,0x16,0x00,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x29,0x4a,0x1f,0x00,0x9c,0x9c,0x29,0x54,0x1f,0x00,0x9c,0x9c,0x0b,0x16,0x16,0x3d,0x9c,0x9c,0x0b,0x16,0x16,0x3d,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x29,0x4a,0x00,0x1f,0x9c,0x9c,0x29,0x54,0x00,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x3d,0x9c,0x9c,0x0b,0x16,0x16,0x3d,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x29,0x00,0x4a,0x1f,0x9c,0x9c,0x29,0x54,0x00,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x3d,0x9c,0x9c,0x0b,0x16,0x16,0x3d,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x29,0x00,0x4a,0x1f,0x9c,0x9c,0x29,0x00,0x54,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x3d,0x9c,0x9c,0x0b,0x16,0x16,0x3d,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x00,0x29,0x4a,0x1f,0x9c,0x9c,0x00,0x29,0x54,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x3d,0x9c,0x9c,0x0b,0x16,0x16,0x3d,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x00,0x29,0x4a,0x1f,0x9c,0x9c,0x0b,0x29,0x54,0x1f,0x9c,0x9c,0x0b,0x16,0x16,0x3d,0x9c,0x9c,0x00,0x16,0x16,0x3d,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x4a,0x1f,0x9c,0x9c,0x0b,0x29,0x54,0x1f,0x9c,0x9c,0x00,0x16,0x16,0x3d,0x9c,0x9c,0x00,0x16,0x16,0x3d,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x4a,0x1f,0x9c,0x9c,0x0b,0x29,0x54,0x1f,0x9c,0x9c,0x16,0x16,0x00,0x3d,0x9c,0x9c,0x16,0x16,0x00,0x3d,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x4a,0x1f,0x9c,0x9c,0x0b,0x29,0x00,0x1f,0x9c,0x9c,0x16,0x16,0x54,0x3d,0x9c,0x9c,0x16,0x16,0x00,0x3d,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x4a,0x1f,0x9c,0x9c,0x0b,0x29,0x00,0x1f,0x9c,0x9c,0x16,0x16,0x00,0x3d,0x9c,0x9c,0x16,0x16,0x54,0x3d,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x00,0x1f,0x9c,0x9c,0x0b,0x29,0x4a,0x1f,0x9c,0x9c,0x16,0x16,0x00,0x3d,0x9c,0x9c,0x16,0x16,0x54,0x3d,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x00,0x1f,0x9c,0x9c,0x0b,0x29,0x00,0x1f,0x9c,0x9c,0x16,0x16,0x4a,0x3d,0x9c,0x9c,0x16,0x16,0x54,0x3d,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x1f,0x00,0x9c,0x9c,0x0b,0x29,0x1f,0x00,0x9c,0x9c,0x16,0x16,0x4a,0x3d,0x9c,0x9c,0x16,0x16,0x54,0x3d,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x1f,0x00,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x16,0x16,0x4a,0x3d,0x9c,0x9c,0x16,0x16,0x54,0x00,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x16,0x16,0x4a,0x00,0x9c,0x9c,0x16,0x16,0x54,0x00,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x16,0x16,0x4a,0x00,0x9c,0x9c,0x16,0x16,0x00,0x54,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x16,0x16,0x4a,0x54,0x9c,0x9c,0x16,0x16,0x00,0x00,0x9c,0x9c,0x5e,0x68,0x35,0x35,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x16,0x16,0x4a,0x54,0x9c,0x9c,0x16,0x16,0x35,0x35,0x9c,0x9c,0x5e,0x68,0x00,0x00,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x16,0x16,0x4a,0x54,0x9c,0x9c,0x16,0x16,0x35,0x35,0x9c,0x9c,0x5e,0x00,0x68,0x00,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x16,0x16,0x4a,0x54,0x9c,0x9c,0x16,0x16,0x35,0x35,0x9c,0x9c,0x5e,0x00,0x00,0x68,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x16,0x16,0x4a,0x54,0x9c,0x9c,0x16,0x16,0x35,0x35,0x9c,0x9c,0x00,0x5e,0x00,0x68,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x16,0x16,0x4a,0x54,0x9c,0x9c,0x16,0x16,0x35,0x35,0x9c,0x9c,0x00,0x00,0x5e,0x68,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x00,0x00,0x4a,0x54,0x9c,0x9c,0x16,0x16,0x35,0x35,0x9c,0x9c,0x16,0x16,0x5e,0x68,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x00,0x4a,0x00,0x54,0x9c,0x9c,0x16,0x16,0x35,0x35,0x9c,0x9c,0x16,0x16,0x5e,0x68,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x4a,0x00,0x00,0x54,0x9c,0x9c,0x16,0x16,0x35,0x35,0x9c,0x9c,0x16,0x16,0x5e,0x68,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x4a,0x00,0x54,0x00,0x9c,0x9c,0x16,0x16,0x35,0x35,0x9c,0x9c,0x16,0x16,0x5e,0x68,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x4a,0x54,0x00,0x00,0x9c,0x9c,0x16,0x16,0x35,0x35,0x9c,0x9c,0x16,0x16,0x5e,0x68,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x4a,0x54,0x35,0x35,0x9c,0x9c,0x16,0x16,0x00,0x00,0x9c,0x9c,0x16,0x16,0x5e,0x68,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x4a,0x54,0x35,0x35,0x9c,0x9c,0x16,0x16,0x5e,0x00,0x9c,0x9c,0x16,0x16,0x00,0x68,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x4a,0x54,0x35,0x35,0x9c,0x9c,0x16,0x16,0x00,0x5e,0x9c,0x9c,0x16,0x16,0x00,0x68,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x0b,0x29,0x1f,0x3d,0x9c,0x9c,0x4a,0x54,0x35,0x35,0x9c,0x9c,0x00,0x16,0x16,0x5e,0x9c,0x9c,0x00,0x16,0x16,0x68,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,0x9c,},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,},
};
