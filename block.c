#include "app.h"


#define DOT		9
#define SIZE	(144/DOT)		// 16
int     RACKET_W;				// ラケットの幅 20, 15, 10
#define RACKET_V	5			// ラケットの移動速度
#define TICK		17*2		// 刻み時間
//#define SCRN_TOP	0xa0000000	// video ram top address
#define BALL_SIZE	4			// ボールの大きさ
#define BALL_YV		2			// ボールの縦方向速度

#define N		0				// Nashi
#define B		1				// Block
#define K		2				// Kabe

#define abs(x) ((x)>0 ? (x) : -(x))

static char ball_data[BALL_SIZE*BALL_SIZE];
static int ban[SIZE][SIZE];
static int racket;
static int ball_x, ball_y;				// ボール位置
static int ball_xv, ball_yv;			// ボール速度
static int score;

static const int ban_init[SIZE][SIZE]={
	{K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,},
	{K,N,N,N,N,N,N,N,N,N,N,N,N,N,N,K,},
	{K,N,B,B,B,B,B,B,B,B,B,B,B,B,N,K,},
	{K,N,B,B,B,B,B,B,B,B,B,B,B,B,N,K,},
	{K,N,B,B,B,B,B,B,B,B,B,B,B,B,N,K,},
	{K,N,B,B,B,B,B,B,B,B,B,B,B,B,N,K,},
	{K,N,B,B,B,B,B,B,B,B,B,B,B,B,N,K,},
	{K,N,N,N,N,N,N,N,N,N,N,N,N,N,N,K,},
	{K,N,N,N,N,N,N,N,N,N,N,N,N,N,N,K,},
	{K,N,N,N,N,N,N,N,N,N,N,N,N,N,N,K,},
	{K,N,N,N,N,N,N,N,N,N,N,N,N,N,N,K,},
	{K,N,N,N,N,N,N,N,N,N,N,N,N,N,N,K,},
	{K,N,N,N,N,N,N,N,N,N,N,N,N,N,N,K,},
	{K,N,N,N,N,N,N,N,N,N,N,N,N,N,N,K,},
	{K,N,N,N,N,N,N,N,N,N,N,N,N,N,N,K,},
	{K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,},
};

#define H	7
#define I	1
#define J	2
#define L	6
#define M	3
#define O	5

static const char koma_K[DOT][DOT]={			// 白壁
	{H,H,H,H,H,H,H,H,H},
	{H,H,H,H,H,H,H,H,H},
	{H,H,H,H,H,H,H,H,H},
	{H,H,H,H,H,H,H,H,H},
	{H,H,H,H,H,H,H,H,H},
	{H,H,H,H,H,H,H,H,H},
	{H,H,H,H,H,H,H,H,H},
	{H,H,H,H,H,H,H,H,H},
	{H,H,H,H,H,H,H,H,H},
};
static char koma_N[DOT][DOT];			// 黒地

static const char koma_B[5][DOT][DOT]={
	{
		{0,0,0,0,0,0,0,0,0},
		{0,I,I,I,I,I,I,I,0},
		{0,I,I,I,I,I,I,I,0},
		{0,I,I,I,I,I,I,I,0},
		{0,I,I,I,I,I,I,I,0},
		{0,I,I,I,I,I,I,I,0},
		{0,I,I,I,I,I,I,I,0},
		{0,I,I,I,I,I,I,I,0},
		{0,0,0,0,0,0,0,0,0},
	},
	
	{
		{0,0,0,0,0,0,0,0,0},
		{0,J,J,J,J,J,J,J,0},
		{0,J,J,J,J,J,J,J,0},
		{0,J,J,J,J,J,J,J,0},
		{0,J,J,J,J,J,J,J,0},
		{0,J,J,J,J,J,J,J,0},
		{0,J,J,J,J,J,J,J,0},
		{0,J,J,J,J,J,J,J,0},
		{0,0,0,0,0,0,0,0,0},
	},
	
	{
		{0,0,0,0,0,0,0,0,0},
		{0,L,L,L,L,L,L,L,0},
		{0,L,L,L,L,L,L,L,0},
		{0,L,L,L,L,L,L,L,0},
		{0,L,L,L,L,L,L,L,0},
		{0,L,L,L,L,L,L,L,0},
		{0,L,L,L,L,L,L,L,0},
		{0,L,L,L,L,L,L,L,0},
		{0,0,0,0,0,0,0,0,0},
	},
	
	{
		{0,0,0,0,0,0,0,0,0},
		{0,M,M,M,M,M,M,M,0},
		{0,M,M,M,M,M,M,M,0},
		{0,M,M,M,M,M,M,M,0},
		{0,M,M,M,M,M,M,M,0},
		{0,M,M,M,M,M,M,M,0},
		{0,M,M,M,M,M,M,M,0},
		{0,M,M,M,M,M,M,M,0},
		{0,0,0,0,0,0,0,0,0},
	},
	
	{
		{0,0,0,0,0,0,0,0,0},
		{0,O,O,O,O,O,O,O,0},
		{0,O,O,O,O,O,O,O,0},
		{0,O,O,O,O,O,O,O,0},
		{0,O,O,O,O,O,O,O,0},
		{0,O,O,O,O,O,O,O,0},
		{0,O,O,O,O,O,O,O,0},
		{0,O,O,O,O,O,O,O,0},
		{0,0,0,0,0,0,0,0,0},
	},
};


static int ball_move();
static void block_clear(int x, int y);
static void block_kuzusi(int x, int y);
static void start_disp();
static void score_disp();
static void losing_disp();
static void win_disp();
static void ball_clear();
static void ball_disp();
static void racket_clear();
static void racket_disp();
static void racket_move();
static void dispban();


/////////////////////////////////
//	main
/////////////////////////////////
void block_main()
{
	uint time1;
	
	init_genrand(TickCount);
	for(;;){
		start_disp();
		lcd_clear(BLACK);
		memcpy(ban, ban_init, sizeof(ban));
		dispban();
		score = 0;
		RACKET_W = 20;
		racket = DOT*SIZE/2 - RACKET_W/2;
		ball_x = DOT*SIZE/2 - BALL_SIZE/2;
		ball_y = DOT*7;
		ball_yv = -BALL_YV;
		ball_xv = 0;
		read_square(ball_x, ball_y, BALL_SIZE, BALL_SIZE, ball_data);
		time1 = TickCount;
		for(;;){
            drawcount = 0;
            while(drawcount==0)
                ;
			racket_clear();
			racket_move();
			racket_disp();
			
			ball_clear();
			if(ball_move()){
				losing_disp();
				break;
			}
			ball_disp();
			
			score_disp();
			win_disp();
			
			while((TickCount-time1) < TICK)
				;
			time1 += TICK;
		}
	}
}
/////////////////////////////////
//	ball_move
/////////////////////////////////
static int ball_move()
{
	int x1, x2, y1, y2, tmp1, tmp2;
	
	//	ボールを移動する
	ball_x += ball_xv;
	ball_y += ball_yv;
	
	//	上壁テスト
	if(ball_y < DOT){						// 上壁に当たっていた
		ball_yv = BALL_YV;					// 下方向の速度にする
//		buzzer(1000,50);
		buzzer_div = 8;						// 1KHz
		buzzer_ms = 50;
	}
	
	//	下壁テスト
	if(ball_y+BALL_SIZE > 15*DOT){			// 下壁に当たっていた
		ball_yv = -BALL_YV;					// 上方向の速度にする
//		buzzer(500,1000);
		buzzer_div = 16;						// 500Hz
		buzzer_ms = 1000;
		return 1;							// 終了
	}
	
	// 右壁テスト
	if(ball_x+BALL_SIZE > 15*DOT){			// 右壁に当たっていた
		ball_xv = -abs(ball_xv);			// 左方向の速度にする
//		buzzer(1000,50);
		buzzer_div = 8;						// 1KHz
		buzzer_ms = 50;
	}
	
	// 左壁テスト
	if(ball_x < DOT){						// 左壁に当たっていた
		ball_xv = abs(ball_xv);				// 右方向の速度にする
//		buzzer(1000,50);
		buzzer_div = 8;						// 1KHz
		buzzer_ms = 50;
	}
	
	// ラケットテスト
	if((ball_y+BALL_SIZE>14*DOT) && (ball_y>14*DOT) &&
			(ball_x+BALL_SIZE>racket) && (ball_x<racket+RACKET_W)){
//		buzzer(1000,50);
		buzzer_div = 8;						// 1KHz
		buzzer_ms = 50;
		tmp1 = ball_x+BALL_SIZE/2;			// ボール位置
		tmp2 = racket+RACKET_W/2;			// ラケット位置
		ball_yv = -BALL_YV;					// 上方向の速度にする
		if(abs(tmp1-tmp2) <= RACKET_W*1/3){	// 中央に当たった
			ball_xv = MCrnd(3) - 1;
		}
		else if(tmp1<tmp2){					// ラケットの左に当たった
			ball_xv = -MCrnd(3) - 2;
		}
		else{								// ラケットの右に当たった
			ball_xv = MCrnd(3) - 0;
		}
	}
	
    // ブロックとの衝突
    y1 = (ball_y-6)/DOT;                  // ボール上面の DOT 数
    y2 = (ball_y+BALL_SIZE-6)/DOT;        // ボール下面の DOT 数
    if((y1>=2 && y1<=6) || (y2>=2 && y2<=6)){
        if(y1==1) y1 = 2;
        if(y2==7) y2 = 6;
        x1 = (ball_x)/DOT;              // ボール左
        x2 = (ball_x+BALL_SIZE)/DOT;    // ボール右
        if((x1>=2 && x1<=SIZE-3) || (x2>=2 && x2<=SIZE-3)){
            if(x1==1) x1 = 2;
            if(x2==SIZE-2) x2 = SIZE-3;
            if(ban[y1][x1]==B){
                block_kuzusi(x1, y1);
                ball_yv = -ball_yv;
            }
            if(ban[y1][x2]==B){
                block_kuzusi(x2, y1);
                ball_yv = -ball_yv;
            }
            if(ban[y2][x1]==B){
                block_kuzusi(x1, y2);
                ball_yv = -ball_yv;
            }
            if(ban[y2][x2]==B){
                block_kuzusi(x2, y2);
                ball_yv = -ball_yv;
            }
        }
    }
    
	return 0;								// normal end
}

static void block_clear(int x, int y)
{
	lcd_clearA(x*DOT, y*DOT+6, x*DOT+DOT-1, y*DOT+DOT-1+6, BLACK);
}

static void block_kuzusi(int x, int y)
{
	int tmp1;
	
//	buzzer(800, 100);
	buzzer_div = 16;							// 500Hz
	buzzer_ms = 100;
	tmp1 = ban[y][x]+ban[y-1][x]+ban[y+1][x]+ban[y][x-1]+ban[y][x+1];
	score += tmp1*10;
	ban[y][x] = N;
	ban[y-1][x] = N;
	ban[y+1][x] = N;
	ban[y][x-1] = N;
	ban[y][x+1] = N;
	block_clear(x, y);
	block_clear(x, y-1);
	block_clear(x, y+1);
	block_clear(x-1, y);
	block_clear(x+1, y);
}
/////////////////////////////////
//	start_disp
/////////////////////////////////
static void start_disp()
{
	lcd_clear(CYAN);						// MAZENTA
	str_cdisp(70, "ブロック崩しゲーム");
	str_cdisp(95, "K.Fukumoto");
	sw_updown();
}
/////////////////////////////////
//	score_disp
/////////////////////////////////
static void score_disp()
{
	char buf[50];
	
	sprintf(buf, "Score:%d", score);
	str_disp(0,0, buf);
}
/////////////////////////////////
//	losing_disp
/////////////////////////////////
static void losing_disp()
{
    str_cdisp(20, "End of game.");
    wait_ms(2000);
    sw_updown();
}
/////////////////////////////////
//	win_disp
/////////////////////////////////
static void win_disp()
{
	int i;
	
	for(i=0; i<SIZE*SIZE; i++){
		if(((int*)ban)[i]==B){				// ブロックがまだ有る
			return;
		}
	}
	lcd_clear(BLACK);
	memcpy(ban, ban_init, sizeof(ban));
	dispban();
	ball_x = DOT*SIZE/2 - BALL_SIZE/2;
	ball_y = DOT*7;
	ball_xv = ball_yv = BALL_YV;
	RACKET_W -= 5;
	if(RACKET_W < 10)
		RACKET_W = 10;
}
/////////////////////////////////
//	ball_clear
/////////////////////////////////
static void ball_clear()
{
	write_square(ball_x, ball_y, BALL_SIZE, BALL_SIZE, ball_data);
}
/////////////////////////////////
//	ball_disp
/////////////////////////////////
static void ball_disp()
{
	read_square(ball_x, ball_y, BALL_SIZE, BALL_SIZE, ball_data);
	lcd_clearA(ball_x, ball_y, ball_x+BALL_SIZE-1, ball_y+BALL_SIZE-1, RED);
}
/////////////////////////////////
//	racket_clear
/////////////////////////////////
static void racket_clear()
{
	lcd_clearA(racket, 14*DOT, racket+RACKET_W-1, 14*DOT+2, BLACK);
}
/////////////////////////////////
//	racket_disp
/////////////////////////////////
static void racket_disp()
{
	lcd_clearA(racket, 14*DOT, racket+RACKET_W-1, 14*DOT+2, WHITE);
}
/////////////////////////////////
//	racket_move
/////////////////////////////////
static void racket_move()
{
	int c;
	
	c = sw_sense();						// key 入力
	if(c==RIGHT){
		racket += RACKET_V;
		if(racket > (SIZE-1)*DOT - RACKET_W)
			racket = (SIZE-1)*DOT - RACKET_W;
	}
	else if(c==LEFT){
		racket -= RACKET_V;
		if(racket < 1*DOT)
			racket = 1*DOT;
	}
}
/////////////////////////////////
//	dispban
/////////////////////////////////
static void dispban()
{
	int x,y;

	for(y=0; y<SIZE; y++){
		for(x=0; x<SIZE; x++){
			if(ban[y][x]==N){
				write_square(x*DOT, y*DOT+6, DOT, DOT, (char*)koma_N);
			}
			else if(ban[y][x]==B){
				write_square(x*DOT, y*DOT+6, DOT, DOT, (char*)koma_B[y-2]);
			}
			else if(ban[y][x]==K){
				write_square(x*DOT, y*DOT+6, DOT, DOT, (char*)koma_K);
			}
		}
	}
}
