#include "app.h"


extern const char pin_board[150][70];
extern const char r_lever_up[5][5];
extern const char r_lever_down[5][5];
extern const char lever_clear[5][5];

#define abs1(x) ((x)<0 ? -(x) : (x))

#define G	YELLOW
#define H	24

static const char ball_pat[6][4]={
	{G,H,H,G},
	{H,H,H,H},
	{H,H,H,H},
	{H,H,H,H},
	{H,H,H,H},
	{G,H,H,G},
};


struct ball_save{
	int x;
	int y;
	char ball[6][4];
};


static struct ball_save ball_save;
static int xpos1000, ypos1000;					// 1/100 ドット単位の位置
static int xv1000, yv1000;
static int r_lever_status, l_lever_status;
static int score;

static void pin_init();
//static void draw_ball(int x, int y);
static void move_ball();
static void kabe(int x1, int x2, int y1, int y2, int *x, int *y);
static void hansya(int x1, int x2, int y1, int y2, int *x100, int *y100);
static void set_ball();
static int clash(char ban[6][4], int *x1, int *x2, int *y1, int *y2);
static int end_of_ball();
static void pos_count();


/********************************************************************************/
/*		pin_main																*/
/********************************************************************************/
void pin_main()
{
	int i;
	char buf[50];
	
	for(;;){
		lcd_clear(RED_U);
		str_cdisp(60, "　ピンボール　");
		str_cdisp(80, "K.Fukumoto");
		sw_updown();
		
		lcd_clear(WHITE);
		pin_init();
		score = 0;
		for(i=0; i<3; ){
			write_square(0,0, 70,150, (char*)pin_board);
			
			set_ball();
			r_lever_status = 1;
			l_lever_status = 1;
			buzzer_div = 8;						// 1KHz
			buzzer_ms  = 500;
			for(;;){
				move_ball();
				if(end_of_ball()){
					buzzer_div = 16;			// 500Hz
					buzzer_ms  = 500;
					break;
				}
				pos_count();
				str_disp(80, 10, "Score");
				sprintf(buf, "%d", score);
				str_disp(86, 20, buf);
				wait_ms(5);
			}
			if(++i >= 3){
				break;
			}
			else{
				wait_ms(2000);
			}
		}
		str_disp(80, 40, "End of game");
		wait_ms(2000);
		sw_updown();
	}
}
/********************************************************************************/
/*		pos_count																*/
/********************************************************************************/
static void pos_count()
{
	int x, y;
	
	x = xpos1000/1000;
	y = ypos1000/1000;
	if(x>=24 && x<=42 && y==12){
		buzzer_div = 13;
		buzzer_ms  = 100;
		score += 10;
	}
	if(x>=11 && x<=15 && y==84){
		buzzer_div = 13;
		buzzer_ms  = 100;
		score += 10;
	}
	if(x>=51 && x<=55 && y==84){
		buzzer_div = 13;
		buzzer_ms  = 100;
		score += 10;
	}
}
/********************************************************************************/
/*		end_of_ball																*/
/********************************************************************************/
static int end_of_ball()
{
	if(ypos1000/1000 >= 150)
		return 1;							// end
	if(ypos1000/1000>=132 && (xpos1000/1000==10 || xpos1000/1000==56))
		return 1;
	return 0;
}
/********************************************************************************/
/*		set_ball																*/
/********************************************************************************/
static void set_ball()
{
	int rnd;
	
	rnd = MCrnd(3);
	if(rnd==0)
		xpos1000 = 25*1000;
	else if(rnd==1)
		xpos1000 = 33*1000;
	else
		xpos1000 = 41*1000;
	
	ypos1000 = 6*1000;
	xv1000 = yv1000 = 0;
}
/********************************************************************************/
/*		lever																	*/
/********************************************************************************/
static void lever()
{
	int sw, x, y;
	
	sw = sw_sense();
	x = xpos1000/1000;
	y = ypos1000/1000;
	if(sw & RIGHT){
		if(r_lever_status==0){			// r_lever up
			if(x>=34 && x<=43 && y>=121 && y<=137){
				xv1000 = 100 - MCrnd(300);
				yv1000 = -500 - MCrnd(500);
				ypos1000 = 124*1000;
				buzzer_div = 10;			// 800Hz
				buzzer_ms  = 100;
				score += 100;
			}
			write_square(38,133, 5,5, (char*)lever_clear);
			write_square(38,129, 5,5, (char*)r_lever_up);
		}
		r_lever_status = 1;
	}
	else{
		if(r_lever_status){
			write_square(38,129, 5,5, (char*)lever_clear);
			write_square(38,133, 5,5, (char*)r_lever_down);
		}
		r_lever_status = 0;
	}
	
	if(sw & LEFT){
		if(l_lever_status==0){			// l_lever up
			if(x>=23 && x<=32 && y>=121 && y<=137){
				xv1000 = -100 + MCrnd(300);
				yv1000 = -500 - MCrnd(500);
				ypos1000 = 124*1000;
				buzzer_div = 10;			// 800Hz
				buzzer_ms  = 100;
				score += 100;
			}
			write_square(26,133, 5,5, (char*)lever_clear);
			write_square(26,129, 5,5, (char*)r_lever_down);
		}
		l_lever_status = 1;
	}
	else{
		if(l_lever_status){
			write_square(26,129, 5,5, (char*)lever_clear);
			write_square(26,133, 5,5, (char*)r_lever_up);
		}
		l_lever_status = 0;
	}
}
/********************************************************************************/
/*		move_ball																*/
/********************************************************************************/
static void move_ball()
{
	int x, y, ret, x1, x2, y1, y2;
	char buf[6][4];
	
	write_square(ball_save.x, ball_save.y, 4,6, (char*)ball_save.ball);	// 旧ボールの背景を復元
	lever();								// レバー処理、レバー画像
	
	xpos1000 += xv1000;
	ypos1000 += yv1000;
	yv1000 += 1;
	if(yv1000 > 1000)
		yv1000 = 1000;
	if(xv1000 > 0){
		if(MCrnd(100)<50)
			xv1000--;
	}
	else{
		if(MCrnd(100)<50)
			xv1000++;
	}
	
	x = xpos1000/1000;
	y = ypos1000/1000;
	read_square(x,y, 4,6, (char*)buf);		// 背景を読み取り
	
	ret = clash(buf, &x1, &x2, &y1, &y2);
	
	if(ret==0x40){							// 壁に当たった
		kabe(x1, x2, y1, y2, &xpos1000, &ypos1000);
		buzzer_div = 8;					// 1KHz
		buzzer_ms  = 50;
	}
	else if(ret==0x60){						// 反射
		hansya(x1, x2, y1, y2, &xpos1000, &ypos1000);
		buzzer_div = 11;					// 727Hz
		buzzer_ms  = 100;
		score += 100;
	}
	
	x = xpos1000/1000;						// 新ボールの背景を保存
	y = ypos1000/1000;
	ball_save.x = x;
	ball_save.y = y;
	read_square(x,y, 4,6, (char*)ball_save.ball);
	write_square(x,y, 4,6, (char*)ball_pat);	// 新ボールを描く
}
/********************************************************************************/
/*		kabe																	*/
/*		壁衝突の処理															*/
/********************************************************************************/
static void kabe(int x1, int x2, int y1, int y2, int *x1000, int *y1000)
{
	int dir, tmp;
	
	dir = 0;
	if(x2<=1){							// 左側が当たった
		dir |= 1;
		*x1000 += (x2+1)*1000;
	}
	else if(x1>=2){						// 右側が当たった
		dir |= 2;
		*x1000 -= (4-x1)*1000;
	}
	else{								// 上面または下面が当たった
	}
	if(y2<=2){							// 上面が当たった
		dir |= 0x10;
		*y1000 += (y2+1)*1000;
	}
	else if(y1>=3){						// 下面が当たった
		dir |= 0x20;
		*y1000 -= (6-y1)*1000;
	}
	else{								// 左又は右側が当たった
	}
	
	tmp = (abs1(xv1000)+abs1(yv1000))/2;
	if(tmp < 100)
		tmp = 100;
	switch(dir){
		case 1:							// 左
			xv1000 = 0;
			break;
		case 2:							// 右
			xv1000 = 0;
			break;
		case 0x10:						// 上
			yv1000 = 0;
			break;
		case 0x20:						// 下
			yv1000 = 0;
			break;
		case 1+0x10:					// 左上
			if(yv1000 < 0){				// 下から当たった
				yv1000 = -tmp;
				xv1000 = tmp;
			}
			else{						// 右から当たった
				yv1000 = tmp;
				xv1000 = -tmp;
			}
			break;
		case 1+0x20:					// 左下
			if(yv1000 > 0){				// 上から当たった
				yv1000 = tmp;
				xv1000 = tmp;
			}
			else{						// 右から当たった
				yv1000 = -tmp;
				xv1000 = -tmp;
			}
			break;
		case 2+0x10:					// 右上
			if(yv1000 < 0){				// 下から当たった
				yv1000 = -tmp;
				xv1000 = -tmp;
			}
			else{						// 左から当たった
				yv1000 = tmp;
				xv1000 = tmp;
			}
			break;
		case 2+0x20:					// 右下
			if(yv1000 > 0){				// 上から当たった
				yv1000 = tmp;
				xv1000 = -tmp;
			}
			else{						// 左から当たった
				yv1000 = -tmp;
				xv1000 = tmp;
			}
			break;
	}
	
}
/********************************************************************************/
/*		hansha																	*/
/*		反射の処理																*/
/********************************************************************************/
static void hansya(int x1, int x2, int y1, int y2, int *x1000, int *y1000)
{
	int dir;
	
	dir = 0;
	if(x2<=2){							// 左側が当たった
		dir |= 1;
		*x1000 += (x2+1)*1000;
	}
	else if(x1>=3){						// 右側が当たった
		dir |= 2;
		*x1000 -= (4-x1)*1000;
	}
	else{								// 上面または下面が当たった
	}
	if(y2<=3){							// 上面が当たった
		dir |= 0x10;
		*y1000 += (y2+1)*1000;
	}
	else if(y1>=4){						// 下面が当たった
		dir |= 0x20;
		*y1000 -= (6-y1)*1000;
	}
	else{								// 左又は右側が当たった
	}
	
	switch(dir){
		case 1:							// 左
			xv1000 = 500 + MCrnd(500);
			yv1000 = MCrnd(500) - 250;
			break;
		case 2:							// 右
			xv1000 = -500 - MCrnd(500);
			yv1000 = MCrnd(500) - 250;
			break;
		case 0x10:						// 上
			yv1000 = 500 + MCrnd(500);
			xv1000 = MCrnd(500) - 250;
			break;
		case 0x20:						// 下
			yv1000 = -500 - MCrnd(500);
			xv1000 = MCrnd(500) - 250;
			break;
		case 1+0x10:					// 左上
			xv1000 = 500 + MCrnd(500);
			yv1000 = 500 + MCrnd(500);
			break;
		case 1+0x20:					// 左下
			xv1000 = 500 + MCrnd(500);
			yv1000 = -500 - MCrnd(500);
			break;
		case 2+0x10:					// 右上
			xv1000 = -500 - MCrnd(500);
			yv1000 = 500 + MCrnd(500);
			break;
		case 2+0x20:					// 右下
			xv1000 = -500 - MCrnd(500);
			yv1000 = -500 - MCrnd(500);
			break;
	}
}
/********************************************************************************/
/*		clash																	*/
/*		衝突検出																*/
/*		return: 0=nothing, 0x40:壁, 0x60:反射									*/
/********************************************************************************/
static int clash(char ban[6][4], int *x1, int *x2, int *y1, int *y2)
{
	int type, tmp, x, y;
	
	*x1 = 4-1;
	*x2 = 0;
	*y1 = 6-1;
	*y2 = 0;
	type = 0;
	for(y=0; y<6; y++){
		for(x=0; x<4; x++){
			tmp = ban[y][x] & 0xe0;
			if(tmp==0x20 || tmp==0x80)
				tmp = 0;
			if(tmp && (type==0 || tmp==type)){
				type = tmp;
				if(*x1 > x)
					*x1 = x;
				if(*x2 < x)
					*x2 = x;
				if(*y1 > y)
					*y1 = y;
				if(*y2 < y)
					*y2 = y;
			}
		}
	}
	return type;
}
/********************************************************************************/
/*		draw_ball																*/
/********************************************************************************/
#if 0
static void draw_ball(int x, int y)
{
//	drawcount = 0;
//	while(drawcount==0)
//		;
    wait_ms(5);
	
	write_square(ball_save.x, ball_save.y, 4,6, (char*)ball_save.ball);	// 旧ボールの背景を復元
	
	ball_save.x = x;												// 新ボールの背景を保存
	ball_save.y = y;
	read_square(x,y, 4,6, (char*)ball_save.ball);
	
	write_square(x,y, 4,6, (char*)ball_pat);
}
#endif
/********************************************************************************/
/*		pin_init																*/
/********************************************************************************/
static void pin_init()
{
	set_palette(24,   96,0,0);
	set_palette(25,   255,192,128);
	set_palette(26,   0,128,0);
	set_palette(27,   160,255,128);
	set_palette(28,   255,0,0);
	set_palette(29,   255,192,128);
	set_palette(30,   0,64,0);
	set_palette(31,   255,128,0);
	set_palette(0x21, 255,176,64);
	set_palette(0x22, 128,64,0);
	set_palette(0x23, 255,255,128);
	set_palette(0x25, 255,255,255);
	set_palette(0x40, 128,128,255);
	set_palette(0x41, 255,255,255);
	set_palette(0x42, 0,0,255);
	set_palette(0x43, 255,0,0);
	set_palette(0x44, 255,128,0);
	set_palette(0x45, 192,192,192);
	set_palette(0x46, 0,0,128);
	set_palette(0x60, 0,128,0);
	set_palette(0x61, 64,32,0);
	set_palette(0x80, 128,128,128);
	
	ball_save.x = ball_save.y = 0;
	read_square(0,0, 4,6, (char*)ball_save.ball);
}




#define W	YELLOW

#define A	0x21	// 盤面 薄茶 RGB(255,176,64)
#define B	0x22	// 盤面 茶   RGB(128,64,0)
#define C	0x23	// 盤面 薄黄 RGB(255,255,128)
//#define D	0x24	// 盤面 橙   RGB(255,128,0)
#define E	0x25	// 盤面 白   RGB(255,255,255)

#define I	0x40	// 壁 薄青   RGB(64,64,255)
#define J	0x41	// 壁 白     RGB(255,255,255)
#define K	0x42	// 壁 青     RGB(0,0,255)
#define L	0x43	// 壁 赤     RGB(255,0,0)
#define M	0x44	// 壁 深緑   RGB(0,128,0)
#define N	0x45	// 壁 灰     RGB(192,192,192)
#define O	0x46	// 壁 濃青   RGB(0,0,128)

#define Q	0x60	// 反射 深緑 RGB(0,128,0)
#define R	0x61	// 反射 濃茶 RGB(64,32,0)

#define c 25		//  RGB(255,192,128)
#define d 26		//  RGB(0,128,0)
#define e 27		//  RGB(160,255,128)
#define f 28		//  RGB(255,0,0)
#define g 29		//  RGB(255,192,128)
#define h 30		//  RGB(0,64,0)
#define i 31		//  RGB(255,128,0)


//#define U	0x80	// レバー 灰 RGB(128,128,128)

const char pin_board[150][70]={
//   0         5        10        15        20        25        30        35        40        45        50        55        60        65
	{A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A},	//0
	{A,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B},
	{A,B,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,J,J,J,J,J,J,I,I,I,I,I,I,I,I,I,I,I,I,J,J,J,J,J,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,A,B},
	{A,B,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,J,I,I,I,I,I,I,I,I,I,I,I,I,I,I,I,I,I,I,I,I,I,I,I,J,J,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,A,B},
	{A,B,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,J,J,I,I,I,I,I,I,I,C,C,C,C,C,C,C,C,C,C,C,C,I,I,I,I,I,I,I,J,J,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,A,B},
	{A,B,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,J,J,I,I,I,I,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,I,I,I,I,J,J,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,A,B},	//5
	{A,B,K,K,O,O,K,K,O,O,K,K,O,O,K,K,J,J,I,I,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,I,I,J,J,K,K,O,O,K,K,O,O,K,K,O,O,K,K,A,B},
	{A,B,K,K,O,O,K,K,O,O,K,K,O,O,K,J,I,I,I,I,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,I,I,I,I,J,K,O,O,K,K,O,O,K,K,O,O,K,K,A,B},
	{A,B,O,O,K,K,O,O,K,K,O,O,K,K,J,I,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,I,J,K,K,O,O,K,K,O,O,K,K,O,O,A,B},
	{A,B,O,O,K,K,O,O,K,K,O,O,K,J,I,I,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,I,I,J,K,O,O,K,K,O,O,K,K,O,O,A,B},
	{A,B,K,K,O,O,K,K,O,O,K,K,J,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,J,K,K,O,O,K,K,O,O,K,K,A,B},	//10
	{A,B,K,K,O,O,K,K,O,O,K,J,I,I,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,I,I,J,K,O,O,K,K,O,O,K,K,A,B},
	{A,B,O,O,K,K,O,O,K,K,J,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,J,K,K,O,O,K,K,O,O,A,B},
	{A,B,O,O,K,K,O,O,K,J,I,I,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,I,I,J,K,O,O,K,K,O,O,A,B},
	{A,B,K,K,O,O,K,K,J,I,I,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,I,I,J,K,K,O,O,K,K,A,B},
	{A,B,K,K,O,O,K,J,I,I,I,E,C,C,C,E,C,C,C,E,L,L,L,L,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,L,L,L,L,C,E,C,C,C,E,C,C,C,I,I,J,K,K,O,O,K,K,A,B},	//15
	{A,B,O,O,K,K,O,J,I,I,C,C,C,C,C,C,C,C,C,L,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,L,C,C,C,C,C,C,C,C,C,I,I,J,O,K,K,O,O,A,B},
	{A,B,O,O,K,K,O,J,I,I,C,C,C,E,C,C,C,E,L,L,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,L,L,C,E,C,C,C,E,C,C,I,I,J,O,K,K,O,O,A,B},
	{A,B,K,K,O,O,J,I,I,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,I,I,J,O,O,K,K,A,B},
	{A,B,K,K,O,O,J,I,I,C,C,E,C,C,C,E,L,L,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,L,L,C,E,C,C,C,E,C,I,I,J,O,O,K,K,A,B},
	{A,B,O,O,K,J,I,I,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,I,I,J,K,O,O,A,B},	//20
	{A,B,O,O,K,J,I,I,C,E,C,C,C,E,C,L,L,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,L,L,C,C,E,C,C,C,E,I,I,J,K,O,O,A,B},
	{A,B,K,K,O,J,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,J,O,K,K,A,B},
	{A,B,K,K,J,I,I,E,C,C,C,E,C,C,L,L,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,L,L,C,C,C,E,C,C,C,I,I,J,K,K,A,B},
	{A,B,O,O,J,I,I,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,I,I,J,O,O,A,B},
	{A,B,O,O,J,I,I,C,C,E,C,C,C,L,L,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,L,L,E,C,C,C,E,C,I,I,J,O,O,A,B},	//25
	{A,B,K,K,J,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,J,K,K,A,B},
	{A,B,K,J,J,I,I,E,C,C,C,E,C,L,L,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,L,L,C,C,E,C,C,C,I,I,J,K,K,A,B},
	{A,B,O,J,I,I,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,I,I,J,O,A,B},
	{A,B,O,J,I,I,C,C,C,E,C,C,L,L,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,L,L,C,C,C,E,C,C,I,I,J,O,A,B},
	{A,B,K,J,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,J,K,A,B},	//30
	{A,B,K,J,I,I,C,E,C,C,C,E,L,L,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,L,L,C,E,C,C,C,E,I,I,J,K,A,B},
	{A,B,J,I,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,J,O,A,B},
	{A,B,J,I,I,E,C,C,C,E,C,L,L,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,L,L,C,C,E,C,C,C,I,I,J,A,B},
	{A,B,J,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,J,A,B},
	{A,B,J,I,I,C,C,E,C,C,C,L,L,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,R,R,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,L,L,E,C,C,C,E,C,I,I,J,A,B},	//35
	{A,B,J,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,R,R,R,R,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,J,A,B},
	{A,B,J,I,I,E,C,C,C,E,C,L,L,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,R,R,R,R,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,L,L,C,C,E,C,C,C,I,I,J,A,B},
	{A,B,J,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,R,R,R,R,R,R,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,J,A,B},
	{A,B,J,I,I,C,C,E,C,C,C,L,L,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,R,R,R,R,R,R,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,L,L,E,C,C,C,E,C,I,I,J,A,B},
	{A,B,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,R,R,R,R,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,A,B},	//40
	{A,B,I,I,C,E,C,C,C,E,L,L,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,R,R,R,R,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,L,L,C,E,C,C,C,E,I,I,A,B},
	{A,B,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,R,R,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,A,B},
	{A,B,I,I,C,C,C,E,C,C,L,L,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,L,L,C,C,C,E,C,C,I,I,A,B},
	{A,B,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,A,B},
	{A,B,I,I,C,E,C,C,C,E,L,L,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,L,L,C,E,C,C,C,E,I,I,A,B},	//45
	{A,B,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,A,B},
	{A,B,I,I,C,C,C,E,C,C,L,L,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,L,L,C,C,C,E,C,C,I,I,A,B},
	{A,B,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,A,B},
	{A,B,I,I,C,E,C,C,C,E,L,L,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,L,L,C,E,C,C,C,E,I,I,A,B},
	{A,B,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,A,B},	//50
	{A,B,I,I,C,C,C,E,C,C,L,L,C,C,C,E,C,C,C,E,C,C,C,R,R,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,R,R,E,C,C,C,E,C,C,C,E,C,C,L,L,C,C,C,E,C,C,I,I,A,B},
	{A,B,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,R,R,R,R,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,R,R,R,R,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,A,B},
	{A,B,I,I,C,E,C,C,C,E,L,L,C,E,C,C,C,E,C,C,C,E,R,R,R,R,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,R,R,R,R,C,E,C,C,C,E,C,C,C,E,L,L,C,E,C,C,C,E,I,I,A,B},
	{A,B,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,R,R,R,R,R,R,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,R,R,R,R,R,R,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,A,B},
	{A,B,I,I,C,C,C,E,C,C,L,L,C,C,C,E,C,C,C,E,C,R,R,R,R,R,R,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,R,R,R,R,R,R,C,C,E,C,C,C,E,C,C,L,L,C,C,C,E,C,C,I,I,A,B},	//55
	{A,B,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,R,R,R,R,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,R,R,R,R,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,A,B},
	{A,B,I,I,C,E,C,C,C,E,L,L,C,E,C,C,C,E,C,C,C,C,R,R,R,R,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,R,R,R,R,C,E,C,C,C,E,C,C,C,E,L,L,C,E,C,C,C,E,I,I,A,B},
	{A,B,J,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,R,R,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,R,R,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,J,A,B},
	{A,B,J,I,I,C,C,E,C,C,C,L,L,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,L,L,E,C,C,C,E,C,I,I,J,A,B},
	{A,B,J,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,J,A,B},	//60
	{A,B,J,I,I,E,C,C,C,E,C,L,L,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,L,L,C,C,E,C,C,C,I,I,J,A,B},
	{A,B,J,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,J,A,B},
	{A,B,J,I,I,C,C,E,C,C,C,L,L,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,L,L,E,C,C,C,E,C,I,I,J,A,B},
	{A,B,J,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,J,A,B},
	{A,B,O,J,I,I,C,C,C,E,C,C,L,L,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,L,L,C,C,C,E,C,C,I,I,J,K,A,B},	//65
	{A,B,K,J,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,J,O,A,B},
	{A,B,K,J,I,I,C,E,C,C,C,E,L,L,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,e,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,L,L,C,E,C,C,C,E,I,I,J,O,A,B},
	{A,B,O,J,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,e,e,C,d,e,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,J,K,A,B},
	{A,B,O,J,I,I,C,C,C,E,C,C,L,L,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,f,f,d,d,d,e,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,L,L,C,C,C,E,C,C,I,I,J,K,A,B},
	{A,B,K,K,J,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,c,f,f,d,d,d,d,d,d,e,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,J,O,O,A,B},	//70
	{A,B,K,K,J,I,I,E,C,C,C,E,C,L,L,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,f,f,f,f,f,f,f,d,d,e,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,L,L,C,C,E,C,C,C,I,I,J,O,O,A,B},
	{A,B,O,O,J,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,c,f,f,f,f,f,f,d,d,f,d,e,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,J,K,K,A,B},
	{A,B,O,O,J,I,I,C,C,E,C,C,C,L,L,C,C,E,C,C,C,E,C,C,C,E,C,C,C,c,f,f,f,f,f,f,f,f,f,f,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,L,L,E,C,C,C,E,C,I,I,J,K,K,A,B},
	{A,B,K,K,J,I,I,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,c,f,f,f,f,f,f,f,f,f,c,c,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,I,I,J,O,O,A,B},
	{A,B,K,K,K,J,I,I,C,C,C,E,C,C,L,L,C,C,C,E,C,C,C,E,C,C,C,E,C,c,f,f,f,f,f,f,f,f,f,f,c,C,C,E,C,C,C,E,C,C,C,E,C,C,L,L,C,C,C,E,C,C,I,I,J,K,O,O,A,B},	//75
	{A,B,O,O,K,J,I,I,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,c,f,f,f,f,f,f,f,c,c,f,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,I,I,J,O,K,K,A,B},
	{A,B,O,O,K,J,I,I,C,E,C,C,C,E,C,L,L,E,C,C,C,E,C,C,C,E,C,C,C,c,f,f,f,f,f,f,f,f,f,c,C,E,C,C,C,E,C,C,C,E,C,C,C,L,L,C,C,E,C,C,C,E,I,I,J,O,K,K,A,B},
	{A,B,K,K,O,J,I,I,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,c,f,f,f,f,c,c,c,f,c,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,I,I,J,K,O,O,A,B},
	{A,B,K,K,O,O,J,I,I,C,C,E,C,C,C,E,L,L,C,E,C,C,C,E,C,C,C,E,C,C,f,f,f,f,f,f,f,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,L,L,C,E,C,C,C,E,C,I,I,J,K,K,O,O,A,B},
	{A,B,O,O,K,K,J,I,I,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,f,f,f,f,f,c,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,I,I,J,O,O,K,K,A,B},	//80
	{A,B,O,O,K,K,J,I,I,E,C,C,C,E,C,C,C,L,L,C,C,E,C,C,C,E,C,C,C,E,C,c,c,c,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,L,L,E,C,C,C,E,C,C,C,I,I,J,O,O,K,K,A,B},
	{A,B,K,K,O,O,K,J,I,I,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,I,I,J,O,K,K,O,O,A,B},
	{A,B,K,K,O,O,K,J,I,I,C,E,C,C,C,E,C,C,L,L,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,L,L,C,C,C,E,C,C,C,E,I,I,J,O,K,K,O,O,A,B},
	{A,B,O,O,K,K,O,O,J,I,I,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,I,I,J,K,K,O,O,K,K,A,B},
	{A,B,O,O,K,K,O,O,J,I,I,C,C,E,C,C,C,E,C,L,L,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,L,L,C,C,E,C,C,C,E,C,I,I,J,K,K,O,O,K,K,A,B},	//85
	{A,B,K,K,O,O,K,K,O,J,I,I,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,L,L,C,C,C,C,C,C,C,C,I,I,J,K,O,O,K,K,O,O,A,B},
	{A,B,K,K,O,O,K,K,O,J,I,I,C,C,C,E,C,C,C,E,C,L,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,L,C,C,E,C,C,C,E,C,C,I,I,J,K,O,O,K,K,O,O,A,B},
	{A,B,O,O,K,K,O,O,K,K,J,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,J,O,O,K,K,O,O,K,K,A,B},
	{A,B,O,O,K,K,O,O,K,K,O,J,I,I,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,I,I,J,K,O,O,K,K,O,O,K,K,A,B},
	{A,B,K,K,O,O,K,K,O,O,K,J,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,J,O,K,K,O,O,K,K,O,O,A,B},	//90
	{A,B,K,K,O,O,K,K,O,O,K,K,J,I,I,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,I,I,J,O,O,K,K,O,O,K,K,O,O,A,B},
	{A,B,O,O,K,K,O,O,K,K,O,O,K,J,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,J,O,K,K,O,O,K,K,O,O,K,K,A,B},
	{A,B,O,O,K,K,O,O,K,K,O,O,K,J,I,I,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,I,I,J,O,K,K,O,O,K,K,O,O,K,K,A,B},
	{A,B,K,K,O,O,K,K,O,O,K,K,O,J,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,J,K,O,O,K,K,O,O,K,K,O,O,A,B},
	{A,B,K,K,O,O,K,K,O,O,K,K,O,J,I,I,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,g,g,g,g,g,g,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,I,I,J,K,O,O,K,K,O,O,K,K,O,O,A,B},	//95
	{A,B,O,O,K,K,O,O,K,K,O,O,K,J,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,i,i,i,h,h,h,h,h,h,g,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,J,O,K,K,O,O,K,K,O,O,K,K,A,B},
	{A,B,O,O,K,K,O,O,K,K,O,O,K,J,I,I,C,E,C,C,C,E,C,C,C,E,C,C,C,i,i,i,i,h,h,g,g,h,h,g,i,E,C,C,C,E,C,C,C,E,C,C,C,E,I,I,J,O,K,K,O,O,K,K,O,O,K,K,A,B},
	{A,B,K,K,O,O,K,K,O,O,K,K,O,J,I,I,C,C,C,C,C,C,C,C,C,C,C,C,i,i,i,i,h,h,h,h,h,h,h,g,i,i,C,C,C,C,C,C,C,C,C,C,C,C,I,I,J,K,O,O,K,K,O,O,K,K,O,O,A,B},
	{A,B,K,K,O,O,K,K,O,O,K,K,O,J,I,I,C,C,C,E,C,C,C,E,C,C,C,E,i,i,i,i,i,h,h,g,g,h,h,h,i,i,g,E,C,C,C,E,C,C,C,E,C,C,I,I,J,K,O,O,K,K,O,O,K,K,O,O,A,B},
	{A,B,O,O,K,K,O,O,K,K,O,O,K,J,I,I,C,C,C,C,C,C,C,C,C,C,C,g,i,i,i,i,i,i,i,i,i,i,g,g,W,g,g,C,C,C,C,C,C,C,C,C,C,C,I,I,J,O,K,K,O,O,K,K,O,O,K,K,A,B},	//100
	{A,B,O,O,K,K,O,O,K,K,J,J,J,J,I,I,C,E,C,C,C,E,C,C,C,E,C,g,i,i,i,i,i,i,i,i,i,i,i,G,G,i,i,C,C,E,C,C,C,E,C,C,C,E,I,I,J,J,J,J,O,O,K,K,O,O,K,K,A,B},
	{A,B,K,K,O,O,K,K,O,J,I,I,I,I,I,I,C,C,C,C,C,C,C,C,C,C,C,g,i,i,i,i,i,i,i,i,i,i,i,i,i,i,g,C,C,C,C,C,C,C,C,C,C,C,I,I,I,I,I,I,J,K,O,O,K,K,O,O,A,B},
	{A,B,K,K,O,O,K,K,J,I,I,I,I,I,I,I,C,C,C,E,C,C,C,E,C,C,C,E,i,i,i,i,i,i,i,i,i,i,i,i,i,i,g,E,C,C,C,E,C,C,C,E,C,C,I,I,I,I,I,I,I,J,O,O,K,K,O,O,A,B},
	{A,B,O,O,K,K,O,J,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,g,i,i,i,i,i,i,i,i,i,i,i,i,i,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,J,K,O,O,K,K,A,B},
	{A,B,O,O,K,K,J,I,I,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,g,i,i,i,i,i,i,i,i,i,i,g,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,I,I,J,O,O,K,K,A,B},	//105
	{A,B,K,K,O,O,J,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,g,i,i,i,i,i,i,i,g,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,J,K,K,O,O,A,B},
	{A,B,K,K,O,O,J,I,I,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,g,i,i,g,g,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,I,I,J,K,K,O,O,A,B},
	{A,B,O,O,K,K,J,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,J,O,O,K,K,A,B},
	{A,B,O,O,K,K,J,I,I,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,I,I,J,O,O,K,K,A,B},
	{A,B,K,K,O,O,J,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,J,K,K,O,O,A,B},	//110
	{A,B,K,K,O,O,J,I,I,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,I,I,J,K,K,O,O,A,B},
	{A,B,O,O,K,K,J,I,I,C,C,C,C,C,I,I,C,C,C,C,C,Q,Q,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,Q,Q,C,C,C,C,C,I,I,C,C,C,C,C,I,I,J,O,O,K,K,A,B},
	{A,B,O,O,K,K,J,I,I,E,C,C,C,E,I,I,C,E,C,C,C,Q,Q,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,Q,Q,E,C,C,C,E,I,I,C,E,C,C,C,I,I,J,O,O,K,K,A,B},
	{A,B,K,K,O,O,J,I,I,C,C,C,C,C,I,I,C,C,C,C,C,Q,Q,Q,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,Q,Q,Q,C,C,C,C,C,I,I,C,C,C,C,C,I,I,J,K,K,O,O,A,B},
	{A,B,K,K,O,O,J,I,I,C,C,E,C,C,I,I,C,C,C,E,C,Q,Q,Q,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,Q,Q,Q,C,C,E,C,C,I,I,C,C,C,E,C,I,I,J,K,K,O,O,A,B},	//115
	{A,B,O,O,K,K,J,I,I,C,C,C,C,C,I,I,C,C,C,C,C,M,Q,Q,Q,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,Q,Q,Q,M,C,C,C,C,C,I,I,C,C,C,C,C,I,I,J,O,O,K,K,A,B},
	{A,B,O,O,K,K,J,I,I,E,C,C,C,E,I,I,C,E,C,C,C,C,M,Q,Q,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,Q,Q,M,C,E,C,C,C,E,I,I,C,E,C,C,C,I,I,J,O,O,K,K,A,B},
	{A,B,K,K,O,O,J,I,I,C,C,C,C,C,I,I,C,C,C,C,C,C,C,M,Q,Q,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,Q,Q,M,C,C,C,C,C,C,C,I,I,C,C,C,C,C,I,I,J,K,K,O,O,A,B},
	{A,B,K,K,O,O,J,I,I,C,C,E,C,C,I,I,C,C,C,E,C,C,C,E,M,Q,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,Q,M,C,E,C,C,C,E,C,C,I,I,C,C,C,E,C,I,I,J,K,K,O,O,A,B},
	{A,B,O,O,K,K,J,I,I,C,C,C,C,C,I,I,C,C,C,C,C,C,C,C,C,M,Q,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,Q,M,C,C,C,C,C,C,C,C,C,I,I,C,C,C,C,C,I,I,J,O,O,K,K,A,B},	//120
	{A,B,O,O,K,K,J,I,I,E,C,C,C,E,I,I,C,E,C,C,C,E,C,C,C,E,M,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,M,C,E,C,C,C,E,C,C,C,E,I,I,C,E,C,C,C,I,I,J,O,O,K,K,A,B},
	{A,B,K,K,O,O,J,I,I,C,C,C,C,C,I,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,I,C,C,C,C,C,I,I,J,K,K,O,O,A,B},
	{A,B,K,K,O,O,J,I,I,C,C,E,C,C,C,I,I,I,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,I,I,I,E,C,C,C,E,C,I,I,J,K,K,O,O,A,B},
	{A,B,O,O,K,K,J,I,I,C,C,C,C,C,C,C,I,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,I,C,C,C,C,C,C,C,I,I,J,O,O,K,K,A,B},
	{A,B,O,O,K,K,J,I,I,E,C,C,C,E,C,C,C,I,I,I,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,I,I,I,E,C,C,C,E,C,C,C,I,I,J,O,O,K,K,A,B},	//125
	{A,B,K,K,O,O,J,I,I,C,C,C,C,C,C,C,C,C,I,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,I,C,C,C,C,C,C,C,C,C,I,I,J,K,K,O,O,A,B},
	{A,B,K,K,O,O,J,I,I,C,C,E,C,C,C,E,C,C,C,I,I,I,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,I,I,I,E,C,C,C,E,C,C,C,E,C,I,I,J,K,K,O,O,A,B},
	{A,B,O,O,K,K,J,I,I,C,C,C,C,C,C,C,C,C,C,C,I,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,I,C,C,C,C,C,C,C,C,C,C,C,I,I,J,O,O,K,K,A,B},
	{A,B,O,O,K,K,J,I,I,E,C,C,C,E,C,C,C,E,C,C,C,I,I,I,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,I,I,I,E,C,C,C,E,C,C,C,E,C,C,C,I,I,J,O,O,K,K,A,B},
	{A,B,K,K,O,O,J,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,J,K,K,O,O,A,B},	//130
	{A,B,K,K,O,O,J,I,I,C,C,E,C,C,I,E,C,C,C,E,C,C,C,I,I,I,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,I,I,I,E,C,C,C,E,C,C,C,I,C,C,C,E,C,I,I,J,K,K,O,O,A,B},
	{A,B,O,O,K,K,J,I,I,C,C,C,C,C,I,I,C,C,C,C,C,C,C,C,I,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,I,C,C,C,C,C,C,C,C,I,I,C,C,C,C,C,I,I,J,O,O,K,K,A,B},
	{A,B,O,O,K,K,J,I,I,E,C,C,C,E,I,I,I,E,C,C,C,E,C,C,C,I,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,I,E,C,C,C,E,C,C,C,I,I,I,C,E,C,C,C,I,I,J,O,O,K,K,A,B},
	{A,B,K,K,O,O,J,I,I,C,C,C,C,C,I,I,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,I,I,C,C,C,C,C,I,I,J,K,K,O,O,A,B},
	{A,B,K,K,O,O,J,I,I,C,C,E,C,C,I,I,J,I,I,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,I,I,J,I,I,C,C,C,E,C,I,I,J,K,K,O,O,A,B},	//135
	{A,B,O,O,K,K,J,I,I,C,C,C,C,C,I,I,J,J,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,J,J,I,I,C,C,C,C,C,I,I,J,O,O,K,K,A,B},
	{A,B,O,O,K,K,J,I,I,E,C,C,C,E,I,I,J,K,J,I,I,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,I,I,J,O,J,I,I,C,E,C,C,C,I,I,J,O,O,K,K,A,B},
	{A,B,K,K,O,O,K,J,I,I,C,C,C,I,I,J,O,O,K,J,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,J,O,K,J,I,I,I,C,C,C,I,I,J,O,K,K,O,O,A,B},
	{A,B,K,K,O,O,K,K,J,I,I,I,I,I,J,K,O,O,K,K,J,I,I,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,I,I,J,O,O,K,K,J,I,I,I,I,I,I,J,O,O,K,K,O,O,A,B},
	{A,B,O,O,K,K,O,O,K,J,I,I,I,J,O,O,K,K,O,O,K,J,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,J,O,K,K,O,O,K,J,I,I,I,I,J,O,K,K,O,O,K,K,A,B},	//140
	{A,B,O,O,K,K,O,O,K,K,J,J,J,K,O,O,K,K,O,O,K,K,J,I,I,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,I,I,J,O,O,K,K,O,O,K,K,J,J,J,J,O,O,K,K,O,O,K,K,A,B},
	{A,B,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,J,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,J,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,A,B},
	{A,B,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,J,I,I,E,C,C,C,E,C,C,C,E,C,C,C,E,C,C,C,I,I,J,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,A,B},
	{A,B,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,J,I,I,C,C,C,C,C,C,C,C,C,C,C,C,C,C,I,I,J,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,A,B},
	{A,B,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,J,I,I,E,C,C,C,E,C,C,C,E,C,C,C,I,I,J,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,A,B},	//145
	{A,B,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,J,I,I,C,C,C,C,C,C,C,C,C,C,I,I,J,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,A,B},
	{A,B,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,J,I,I,E,C,C,C,E,C,C,C,I,I,J,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,K,K,O,O,A,B},
	{A,B,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,A,B},
	{A,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B},
};

#if 0
	{A,B,K,K,K,K,J,I,I, , , , , , , , ,I,I,I, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,I,I,I, , , , , , , , ,I,I,J,K,K,K,K,A,B},	//125
	{A,B,K,K,K,K,J,I,I, , , , , , , , , ,I,I,I, , , , , , , , , , , , , , , , , , , , , , , , , , , , ,I,I,I, , , , , , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,J,I,I, , , , , , , , , , ,I,I,I, , , , , , , , , , , , , , , , , , , , , , , , , , ,I,I,I, , , , , , , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,J,I,I, , , , , , , , , , , ,I,I,I, , , , , , , , , , , , , , , , , , , , , , , , ,I,I,I, , , , , , , , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,J,I,I, , , , , , , , , , , , ,I,I,I, , , , , ,H,H, , , , , , , , ,H,H, , , , , ,I,I,I, , , , , , , , , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,J,I,I, , , , , , , , , , , , , ,I,I,I, , , ,H,H,H, , , , , , , , ,H,H,H, , , ,I,I,I, , , , , , , , , , , , , ,I,I,J,K,K,K,K,A,B},	//130
	{A,B,K,K,K,K,J,I,I, , , , , ,I, , , , , , , , ,I,I,I, ,H,H,H, , , , , , , , , , ,H,H,H, ,I,I,I, , , , , , , , ,I, , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,J,I,I, , , , , ,I,I, , , , , , , , ,I,I,H,H,H, , , , , , , , , , , , ,H,H,H,I,I, , , , , , , , ,I,I, , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,J,I,I, , , , , ,I,I,I, , , , , , , , ,I,H,H, , , , , , , , , , , , , , ,H,H,I, , , , , , , , ,I,I,I, , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,J,I,I, , , , , ,I,I,I,I, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,I,I,I,I, , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,J,I,I, , , , , ,I,I,J,I,I, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,I,I,J,I,I, , , , , ,I,I,J,K,K,K,K,A,B},	//135
	{A,B,K,K,K,K,J,I,I, , , , , ,I,I,J,J,I,I, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,I,I,J,J,I,I, , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,J,I,I, , , , , ,I,I,J,K,J,I,I, , , , , , , , , , , , , , , , , , , , , , , , , , , , ,I,I,J,K,J,I,I, , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,K,J,I,I, , , ,I,I,J,K,K,K,J,I,I, , , , , , , , , , , , , , , , , , , , , , , , , , ,I,I,J,K,K,J,I,I,I, , , ,I,I,J,K,K,K,K,K,A,B},
#endif
#if 0
	{A,B,K,K,K,K,J,I,I, , , , , , , , ,I,I,I, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,I,I,I, , , , , , , , ,I,I,J,K,K,K,K,A,B},	//125
	{A,B,K,K,K,K,J,I,I, , , , , , , , , ,I,I,I, , , , , , , , , , , , , , , , , , , , , , , , , , , , ,I,I,I, , , , , , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,J,I,I, , , , , , , , , , ,I,I,I, , , , , , , , , , , , , , , , , , , , , , , , , , ,I,I,I, , , , , , , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,J,I,I, , , , , , , , , , , ,I,I,I, , , , , , , , , , , , , , , , , , , , , , , , ,I,I,I, , , , , , , , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,J,I,I, , , , , , , , , , , , ,I,I,I, , , , , , , , , , , , , , , , , , , , , , ,I,I,I, , , , , , , , , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,J,I,I, , , , , , , , , , , , , ,I,I,I, , , , , , , , , , , , , , , , , , , , ,I,I,I, , , , , , , , , , , , , ,I,I,J,K,K,K,K,A,B},	//130
	{A,B,K,K,K,K,J,I,I, , , , , ,I, , , , , , , , ,I,I,I, , , , , , , , , , , , , , , , , , ,I,I,I, , , , , , , , ,I, , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,J,I,I, , , , , ,I,I, , , , , , , , ,I,I,I, , , , , , , , , , , , , , , , ,I,I,I, , , , , , , , ,I,I, , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,J,I,I, , , , , ,I,I,I, , , , , , , , ,I,H,H, , , , , , , , , , , , , , ,H,H,I, , , , , , , , ,I,I,I, , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,J,I,I, , , , , ,I,I,I,I, , , , , , , , ,H,H,H, , , , , , , , , , , , ,H,H,H, , , , , , , , ,I,I,I,I, , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,J,I,I, , , , , ,I,I,J,I,I, , , , , , , , ,H,H,H, , , , , , , , , , ,H,H,H, , , , , , , , ,I,I,J,I,I, , , , , ,I,I,J,K,K,K,K,A,B},	//135
	{A,B,K,K,K,K,J,I,I, , , , , ,I,I,J,J,I,I, , , , , , , , ,H,H,H, , , , , , , , ,H,H,H, , , , , , , , ,I,I,J,J,I,I, , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,J,I,I, , , , , ,I,I,J,K,J,I,I, , , , , , , , ,H,H, , , , , , , , ,H,H, , , , , , , , ,I,I,J,K,J,I,I, , , , , ,I,I,J,K,K,K,K,A,B},
	{A,B,K,K,K,K,K,J,I,I, , , ,I,I,J,K,K,K,J,I,I, , , , , , , , , , , , , , , , , , , , , , , , , , ,I,I,J,K,K,J,I,I,I, , , ,I,I,J,K,K,K,K,K,A,B},
#endif

const char r_lever_up[5][5]={
	{N,N,C,C,C},
	{N,N,N,C,C},
	{C,N,N,N,C},
	{C,C,N,N,N},
	{C,C,C,N,N},
};
const char r_lever_down[5][5]={
	{C,C,C,N,N},
	{C,C,N,N,N},
	{C,N,N,N,C},
	{N,N,N,C,C},
	{N,N,C,C,C},
};
const char lever_clear[5][5]={
	{C,C,C,C,C},
	{C,C,C,C,C},
	{C,C,C,C,C},
	{C,C,C,C,C},
	{C,C,C,C,C},
};

