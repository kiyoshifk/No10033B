#include "app.h"


extern const char pattern7[20][20];
extern const char ichigo[20][20];
extern const char kaki[20][20];
extern const char younashi[20][20];
static const char *pattern[]={
	(char*)pattern7, (char*)ichigo, (char*)kaki, (char*)younashi,
};

struct te{
	int type;
	int cnt;
	int mul;
	int p;
};

static struct te te[]={
	{0, 3, 10, 1},
	{1, 3, 6,  2},
	{2, 3, 5,  2},
	{3, 3, 4,  3},
	{1, 2, 2,  5},
	{1, 1, 1,  10},
};

#define NO_OF_TE	(sizeof(te)/sizeof(struct te))

static int slot_set[][3]={
	{0,0,0},		// x10		total 10
	{2,0,0},
	{3,3,0},
	{0,2,0},
	{0,3,3},
	{2,0,3},
	{3,3,2},
	{0,2,0},
	{3,2,2},
	{2,0,3},
	{2,0,0},
//	{3,0,0},
//	{2,3,0},
//	{0,0,3},
//	{2,2,0},
//	{3,2,2},
//	{0,3,2},
//	{0,0,3},
//	{3,2,2},
//	{3,0,0},
//	{2,0,0},
//	{3,3,0},
	
	{1,1,1},		// x6
	{1,1,1},		// x6		total 12
	{0,2,0},
	{3,0,0},
	{2,3,0},
	{0,0,3},
	{2,2,0},
	{3,2,2},
	{0,3,2},
	{0,0,3},
	{3,2,2},
	{3,0,0},
	{2,3,3},
	{0,2,0},
//	{3,0,0},
//	{2,3,0},
//	{0,0,3},
//	{2,2,0},
//	{3,2,2},
//	{0,3,2},
//	{0,0,3},
//	{3,2,2},
//	{3,0,0},
//	{2,0,0},
//	{3,3,0},
//	{0,2,0},
	
	{2,2,2},		// x5
	{2,2,2},		// x5		total 10
	{0,2,2},
	{2,3,0},
	{2,0,3},
	{3,0,0},
	{3,3,2},
	{0,2,0},
	{0,0,3},
	{2,0,3},
	{3,3,2},
	{0,3,2},
//	{0,0,3},
//	{3,2,2},
//	{3,0,0},
//	{2,0,0},
//	{3,3,0},
//	{0,2,0},
//	{0,3,3},
//	{2,0,3},
//	{3,3,2},
//	{0,2,0},
	
	{3,3,3},		// x4
	{3,3,3},		// x4
	{3,3,3},		// x4		total 12
	{0,2,0},
	{3,0,0},
	{2,3,0},
	{0,0,3},
	{2,2,0},
	{3,2,2},
	{0,3,2},
	{0,0,3},
	{3,2,2},
	{3,0,0},
	{2,0,0},
	{3,3,0},
//	{0,2,0},
//	{0,3,3},
//	{2,0,3},
//	{3,3,2},
//	{0,2,0},
//	{3,2,2},
//	{2,0,3},
//	{2,0,0},
//	{2,3,0},
//	{2,0,3},
//	{3,0,0},
	
	
	{1,1,0},		// x2
	{1,2,1},		// x2
	{3,1,1},		// x2
	{1,3,1},		// x2
	{2,1,1},		// x2		total 10
	{0,3,2},
	{0,0,3},
	{3,2,2},
	{3,0,0},
	{2,0,0},
	{3,3,0},
	{0,2,0},
	{0,3,3},
	{2,0,3},
	{3,3,2},
//	{0,2,0},
//	{3,2,2},
//	{2,0,3},
//	{2,0,0},
//	{2,3,0},
//	{2,0,3},
//	{3,0,0},
	


	{1,0,0},		// x1
	{2,1,3},		// x1
	{0,3,1},		// x1
	{0,2,1},		// x1
	{2,1,2},		// x1
	{1,0,0},		// x1
	{2,1,3},		// x1
	{0,3,1},		// x1
	{0,2,1},		// x1
	{2,1,2},		// x1		total 10
	{0,3,2},
	{0,0,3},
	{3,2,2},
	{3,0,0},
	{2,0,0},
	{3,3,0},
	{0,2,0},
	{0,3,3},
	{2,0,3},
	{3,3,2},
//	{0,2,0},
//	{3,2,2},
};

#define NO_OF_SET	(sizeof(slot_set)/(sizeof(int)*3))


static int coin;
static int bet;
static int high;


static void slot_init();
static void draw_all(int p1, int p2, int p3);
static void show(int p1, int p2, int p3);
static void set_p1_p3(int *p1, int *p2, int *p3);
static void draw_te();
static int result(int p1, int p2, int p3);


/********************************************************************************/
/*		slot_main																*/
/********************************************************************************/
void slot_main()
{
	int sw, p1, p2, p3, res;
	
	lcd_clear(BRUE_D);
	str_cdisp(50, "　スロットマシン　");
	str_cdisp(70, "Ｒｉｇｈｔ：掛け金アップ");
	str_cdisp(85, "Ｌｅｆｔ：　掛け金ダウン");
	str_cdisp(100,"Ｄｏｗｎ：　開始　　　　");
	str_cdisp(115, "K.Fukumoto");
	sw_updown();
	
	slot_init();
	p1 = p2 = p3 = 0;
	coin = high = 10;
	bet = 1;
	for(;;){
		draw_all(p1,p2,p3);
		sw = sw_updown();
		if(sw & RIGHT){
			bet += coin/30+1;
			if(bet > coin)
				bet = coin;
		}
		if(sw & LEFT){
			bet -= coin/30+1;
			if(bet < 1)
				bet = 1;
		}
		if(sw & DOWN){
			set_p1_p3(&p1, &p2, &p3);
			show(p1, p2, p3);
			res = result(p1, p2, p3);
			coin += res;
			if(coin > high){
				high = coin;
			}
			if(res>0){
				buzzer_div = 8;				// 1KHz
				buzzer_ms = 500;
			}
			else{
				buzzer_div = 16;				// 500Hz
				buzzer_ms = 500;
			}
			if(coin <= 0){
				draw_all(p1,p2,p3);
				str_disp(0,40, " Game over ");
				wait_ms(2000);
				sw_updown();
				coin = 10;
			}
			if(bet > coin){
				bet = coin;
			}
		}
	}
}
/********************************************************************************/
/*		result																	*/
/********************************************************************************/
static int result(int p1, int p2, int p3)
{
	int i, cnt;
	
	for(i=0; i<NO_OF_TE; i++){
		cnt = 0;
		if(p1==te[i].type) cnt++;
		if(p2==te[i].type) cnt++;
		if(p3==te[i].type) cnt++;
		if(cnt >= te[i].cnt)
			return te[i].mul * bet;
	}
	return -bet;
}
/********************************************************************************/
/*		set_p1_p3																*/
/********************************************************************************/
static void set_p1_p3(int *p1, int *p2, int *p3)
{
	int rnd;
	
	rnd = MCrnd(NO_OF_SET);
	*p1 = slot_set[rnd][0];
	*p2 = slot_set[rnd][1];
	*p3 = slot_set[rnd][2];
}
/********************************************************************************/
/*		show																	*/
/*		スロットマシンが回転して停止するまで表示								*/
/********************************************************************************/
static void show(int p1, int p2, int p3)
{
	int i, t, pp1, pp2, pp3, start, rnd;
	
	start = TickCount;
	pp1 = 0;
	pp2 = 1;
	pp3 = 2;
	for(;;){								// p3 表示 -
		t = TickCount;
		if(t-start > 1500){
			write_square(52, 10, 20, 20, pattern[p3]);
			buzzer_div = 8;					// 1KHz
			buzzer_ms = 50;
			break;
		}

		rnd = MCrnd(5);
		for(i=0; i<rnd; i++){
			drawcount=0;
			while(drawcount==0) ;
		}
		write_square( 8, 10, 20, 20, pattern[pp1]);
		if(++pp1 >= 4)
			pp1 = 0;

		rnd = MCrnd(5);
		for(i=0; i<rnd; i++){
			drawcount=0;
			while(drawcount==0) ;
		}
		write_square(30, 10, 20, 20, pattern[pp2]);
		if(++pp2 >= 4)
			pp2 = 0;

		rnd = MCrnd(5);
		for(i=0; i<rnd; i++){
			drawcount=0;
			while(drawcount==0) ;
		}
		write_square(52, 10, 20, 20, pattern[pp3]);
		if(++pp3 >= 4)
			pp3 = 0;
	}
	for(;;){								// p2 表示 -
		t = TickCount;
		if(t-start > 3000){
			write_square(30, 10, 20, 20, pattern[p2]);
			buzzer_div = 8;					// 1KHz
			buzzer_ms = 50;
			break;
		}
		rnd = MCrnd(8);
		for(i=0; i<rnd; i++){
			drawcount=0;
			while(drawcount==0) ;
		}
		write_square( 8, 10, 20, 20, pattern[pp1]);
		if(++pp1 >= 4)
			pp1 = 0;

		rnd = MCrnd(8);
		for(i=0; i<rnd; i++){
			drawcount=0;
			while(drawcount==0) ;
		}
		write_square(30, 10, 20, 20, pattern[pp2]);
		if(++pp2 >= 4)
			pp2 = 0;
	}
	for(;;){								// p1 表示 -
		t = TickCount;
		if(t-start > 4500){
			write_square( 8, 10, 20, 20, pattern[p1]);
			buzzer_div = 8;					// 1KHz
			buzzer_ms = 50;
			wait_ms(50);
			break;
		}
		rnd = MCrnd(12);
		for(i=0; i<rnd; i++){
			drawcount=0;
			while(drawcount==0) ;
		}
		write_square( 8, 10, 20, 20, pattern[pp1]);
		if(++pp1 >= 4)
			pp1 = 0;
	}
}
/********************************************************************************/
/*		draw_all																*/
/********************************************************************************/
static void draw_pat(int p1, int p2, int p3)
{
	write_square( 8, 10, 20, 20, pattern[p1]);
	write_square(30, 10, 20, 20, pattern[p2]);
	write_square(52, 10, 20, 20, pattern[p3]);
}

static void draw_all(int p1, int p2, int p3)
{
	char buf[50];
	
//	drawcount = 0;
//	while(drawcount==0)
//		;
	lcd_clear(WHITE);
	lcd_clearA( 6,  8,   73,  8+1, RED_D);
	lcd_clearA( 6, 30,   73, 30+1, RED_D);
	lcd_clearA( 6,  8,  6+1,   31, RED_D);
	lcd_clearA(28,  8, 28+1,   31, RED_D);
	lcd_clearA(50,  8, 50+1,   31, RED_D);
	lcd_clearA(72,  8, 72+1,   31, RED_D);
	
	draw_pat(p1, p2, p3);
	drawcount = 0;
	sprintf(buf, "持金 %d", coin);
	str_disp(80, 5, buf);
	sprintf(buf, "掛金 %d", bet);
	str_disp(80, 20, buf);
	sprintf(buf, "最大 %d", high);
	str_disp(80, 35, buf);
	draw_te();
}
/********************************************************************************/
/*		draw_te																	*/
/********************************************************************************/
static void draw_te()
{
	int i, j;
	char buf[20];
	
	for(i=0; i<4; i++){
		for(j=3-te[i].cnt; j<3; j++){
			write_square(20*j, 20*i+60, 20, 20, pattern[te[i].type]);
		}
		sprintf(buf, "x%d", te[i].mul);
		str_disp(60, 20*i+60+6, buf);
	}
	for(i=4; i<NO_OF_TE; i++){
		for(j=3-te[i].cnt; j<3; j++){
			write_square(20*j+80, 20*(i-4)+60, 20, 20, pattern[te[i].type]);
		}
		sprintf(buf, "x%d", te[i].mul);
		str_disp(60+80, 20*(i-4)+60+6, buf);
	}
}
/********************************************************************************/
/*		slot_init																*/
/********************************************************************************/
static void slot_init()
{
	set_palette(64, 0,0,255);
	set_palette(65, 255,192,0);
	set_palette(66, 255,255,255);
	set_palette(67, 255,192,128);
	set_palette(68, 0,128,0);
	set_palette(69, 160,255,128);
	set_palette(70, 255,0,0);
	set_palette(71, 255,192,128);
	set_palette(72, 0,64,0);
	set_palette(73, 255,128,0);
	set_palette(74, 128,64,64);
	set_palette(75, 128,160,64);
}



#define A 64		// RGB(0,0,255)
#define B 65		// RGB(255,192,0)
#define W 66		// RGB(255,255,255)

const char pattern7[20][20]={
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,B,B,B,B,B,B,B,B,B,B,B,B,B,B,W,W,W},
	{W,W,W,B,A,A,A,A,A,A,A,A,A,A,A,A,B,W,W,W},
	{W,W,W,B,A,A,A,A,A,A,A,A,A,A,A,A,B,W,W,W},
	{W,W,W,B,B,B,B,B,B,B,B,B,B,A,A,A,B,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,W,B,A,A,A,B,B,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,B,A,A,A,B,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,B,A,A,A,B,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,B,A,A,A,B,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,B,A,A,A,B,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,B,A,A,A,B,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,B,A,A,A,B,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,B,A,A,A,B,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,B,A,A,A,B,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,B,A,A,A,B,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,B,A,A,A,B,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,B,B,B,B,B,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
};

#define C 67		//  RGB(255,192,128)
#define D 68		//  RGB(0,128,0)
#define E 69		//  RGB(160,255,128)
#define F 70		//  RGB(255,0,0)

const char ichigo[20][20]={
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,W,E,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,E,E,W,D,E,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,F,F,D,D,D,E,W,W,W,W,W,W,W},
	{W,W,W,W,W,C,F,F,D,D,D,D,D,D,E,W,W,W,W,W},
	{W,W,W,W,W,F,F,F,F,F,F,F,D,D,E,W,W,W,W,W},
	{W,W,W,W,C,F,F,F,F,F,F,D,D,F,D,E,W,W,W,W},
	{W,W,W,W,C,F,F,F,F,F,F,F,F,F,F,W,W,W,W,W},
	{W,W,W,W,C,F,F,F,F,F,F,F,F,F,C,C,W,W,W,W},
	{W,W,W,W,C,F,F,F,F,F,F,F,F,F,F,C,W,W,W,W},
	{W,W,W,W,C,F,F,F,F,F,F,F,C,C,F,W,W,W,W,W},
	{W,W,W,W,C,F,F,F,F,F,F,F,F,F,C,W,W,W,W,W},
	{W,W,W,W,C,F,F,F,F,C,C,C,F,C,W,W,W,W,W,W},
	{W,W,W,W,W,F,F,F,F,C,F,F,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,F,F,F,F,F,C,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,C,C,C,W,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
};

#define G 71		//  RGB(255,192,128)
#define H 72		//  RGB(0,64,0)
#define I 73		//  RGB(255,128,0)

const char kaki[20][20]={
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,G,G,G,G,G,G,W,W,W,W,W,W,W},
	{W,W,W,W,W,I,I,I,H,H,H,H,H,H,G,W,W,W,W,W},
	{W,W,W,W,I,I,I,I,H,H,G,G,H,H,G,I,W,W,W,W},
	{W,W,W,I,I,I,I,H,H,H,H,H,H,H,G,I,I,W,W,W},
	{W,W,W,I,I,I,I,I,H,H,G,G,H,H,H,I,I,G,W,W},
	{W,W,G,I,I,I,I,I,I,I,I,I,I,G,G,W,G,G,W,W},
	{W,W,G,I,I,I,I,I,I,I,I,I,I,I,G,G,I,I,W,W},
	{W,W,G,I,I,I,I,I,I,I,I,I,I,I,I,I,I,G,W,W},
	{W,W,W,I,I,I,I,I,I,I,I,I,I,I,I,I,I,G,W,W},
	{W,W,W,G,I,I,I,I,I,I,I,I,I,I,I,I,I,W,W,W},
	{W,W,W,W,W,G,I,I,I,I,I,I,I,I,I,I,G,W,W,W},
	{W,W,W,W,W,W,W,G,I,I,I,I,I,I,I,G,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,G,I,I,G,G,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
};

#define J 74		//  RGB(128,64,64)
#define K 75		//  RGB(128,160,64)

const char younashi[20][20]={
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,J,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,J,J,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,K,J,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,K,K,J,K,K,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,K,K,K,K,W,W,K,W,W,W,W,W,W,W},
	{W,W,W,W,W,K,K,K,K,K,W,K,K,K,W,W,W,W,W,W},
	{W,W,W,W,W,K,K,K,K,K,K,K,K,K,K,W,W,W,W,W},
	{W,W,W,W,K,K,K,K,K,K,K,K,K,K,J,W,W,W,W,W},
	{W,W,W,W,K,K,K,K,K,K,K,K,K,K,K,K,W,W,W,W},
	{W,W,W,K,K,K,K,K,K,K,K,K,K,K,K,K,W,W,W,W},
	{W,W,W,K,K,K,K,K,K,J,K,K,K,K,J,K,W,W,W,W},
	{W,W,W,K,K,K,K,K,K,K,K,J,K,K,K,K,W,W,W,W},
	{W,W,W,K,K,K,K,K,K,J,K,K,K,K,J,K,W,W,W,W},
	{W,W,W,W,K,K,K,K,K,K,K,K,K,K,K,W,W,W,W,W},
	{W,W,W,W,W,K,K,K,K,K,J,K,K,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
};
