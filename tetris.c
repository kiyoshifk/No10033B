#include "app.h"


#define W	WHITE
#define R	RED
#define G	GREEN
#define B	BRUE
#define M	MAGENTA
#define C	CYAN
#define Y	YELLOW


static const char piece[7][4][4]={
	{
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
		{W,W,W,W},
	},
	{
		{0,0,0,0},
		{0,0,0,0},
		{R,R,0,0},
		{R,R,0,0},
	},
	{
		{0,0,0,0},
		{0,0,0,0},
		{0,G,G,0},
		{G,G,0,0},
	},
	{
		{0,0,0,0},
		{0,0,0,0},
		{B,B,0,0},
		{0,B,B,0},
	},
	{
		{0,0,0,0},
		{0,0,0,0},
		{M,0,0,0},
		{M,M,M,0},
	},
	{
		{0,0,0,0},
		{0,0,0,0},
		{0,0,C,0},
		{C,C,C,0},
	},
	{
		{0,0,0,0},
		{0,0,0,0},
		{0,Y,0,0},
		{Y,Y,Y,0},
	},
};

#define X_BAN		12			// 盤の横サイズ, 1～10 がフィールド内
#define Y_BAN		22			// 盤の縦サイズ, 1～20 がフィールド内
#define X_KOMA		5			// 正方形の横ドット数
#define Y_KOMA		7			// 正方形の縦ドット数
#define TICK		500

static char koma[4][4];
static int x_koma;
static int y_koma;
static int score;

static char ban[Y_BAN][X_BAN];
static const char ban_init[Y_BAN][X_BAN]={
	{W,W,W,0,0,0,0,0,0,W,W,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,0,0,0,0,0,0,0,0,0,0,W},
	{W,W,W,W,W,W,W,W,W,W,W,W},
};


static void draw_game_over();
static void draw_score();
static int result();
static void draw_ban();
static int test1();
static void rotate();
static int put_koma(char ban[Y_BAN][X_BAN]);
static void remove_koma();


/********************************************************************************/
/*		tetris_main																*/
/********************************************************************************/
void tetris_main()
{
	int sw, move_ok_flag, time;
	
	for(;;){
		lcd_clear(RED);
		str_cdisp(40, "テトリス");
		str_cdisp(60, "Up: Rotate  ");
		str_cdisp(70, "Right: Right");
		str_cdisp(80, "Left: Left  ");
		str_cdisp(90, "Down: Down  ");
		str_cdisp(105,"K.Fukumoto  ");
		sw_updown();
		lcd_clear(BLACK);
		memcpy(ban, ban_init, sizeof(ban));
		score = 0;
		init_genrand(TickCount);
		time = TickCount;
		for(;;){
			//	ピース決定
			memcpy(koma, piece[MCrnd(7)], sizeof(koma));
			x_koma = 4;
			y_koma = -3;
			for(;;){
				wait_ms(100);
				move_ok_flag = 1;
				sw = sw_sense();
				if(sw & UP){					// 回転
					remove_koma();
					rotate();
					put_koma(ban);
				}
				else if(sw & DOWN){
					remove_koma();
					y_koma++;
					if(test1()==0)				// 移動出来ない
						y_koma--;
					put_koma(ban);
				}
				else if(sw & RIGHT){
					remove_koma();
					x_koma++;
					if(test1()==0)				// 移動出来ない
						x_koma--;
					put_koma(ban);
				}
				else if(sw & LEFT){
					remove_koma();
					x_koma--;
					if(test1()==0)				// 移動出来ない
						x_koma++;
					put_koma(ban);
				}
				else{
					if(TickCount-time >= TICK){
						time = TickCount;
						remove_koma();
						y_koma++;
						if(test1()==0){					// 移動出来ない
							y_koma--;
							move_ok_flag = 0;
						}
						put_koma(ban);					// 駒を盤に置く
					}
				}
				drawcount = 0;
				while(drawcount==0)
					;
				draw_ban();						// 盤表示
				draw_score();
				if(move_ok_flag==0){			// 下に移動が出来なかった
					if(result()){				// 結果の判定
						draw_game_over();
						goto next_game;			// game over
					}
					break;
				}
			}
		}
next_game:;
	}
}
/********************************************************************************/
/*		remove_koma																*/
/********************************************************************************/
static void remove_koma()
{
	int x, y, xx, yy;
	
	for(y=0; y<4; y++){
		for(x=0; x<4; x++){
			if(koma[y][x]==0){
				continue;
			}
			yy = y_koma+y;
			if(yy<0 || yy>=Y_BAN-1){
				continue;
			}
			xx = x_koma+x;
			if(xx<1 || xx>=X_BAN-1){
				continue;
			}
			ban[yy][xx] = 0;
		}
	}
}
/********************************************************************************/
/*		draw_game_over															*/
/********************************************************************************/
static void draw_game_over()
{
	str_disp(13*X_KOMA, 30, "Game over");
	sw_updown();
}
/********************************************************************************/
/*		draw_score																*/
/********************************************************************************/
static void draw_score()
{
	char buf[20];
	
	str_disp(13*X_KOMA, 0, "Score:");
	sprintf(buf, "%d", score);
	str_disp(14*X_KOMA, 10, buf);
}
/********************************************************************************/
/*		result																	*/
/*		return 1:game over														*/
/********************************************************************************/
static int result()
{
	int x, y, xx, yy, cnt;
    
	//	全部そろった行を取り除く
    cnt = 0;
	for(y=1; y<Y_BAN-1; y++){
		for(x=0; x<X_BAN; x++){
			if(ban[y][x]==0){
				break;
			}
		}
		if(x==X_BAN){						// 取り除くべき行発見
            cnt++;
			for(yy=y; yy!=1; yy--){
				for(xx=0; xx<X_BAN; xx++){
					ban[yy][xx] = ban[yy-1][xx];
				}
			}
            for(xx=1; xx<X_BAN-1; xx++){
                ban[1][xx] = 0;
            }
			wait_ms(TICK);
			draw_ban();
		}
	}
    if(cnt==1) score += 10;
    if(cnt==2) score += 30;
    if(cnt==3) score += 100;
    if(cnt==4) score += 300;
	//	終了判定
	for(y=1; y<Y_BAN-1; y++){
		for(x=1; x<X_BAN-1; x++){
			if(ban[y][x])
				break;
		}
		if(x==X_BAN-1){
			return 0;						// 終了ではない
		}
	}
	return 1;								// 終了
}
/********************************************************************************/
/*		draw_ban																*/
/********************************************************************************/
static void draw_ban()
{
	int x, y;
	
	for(y=0; y<Y_BAN; y++){
		for(x=0; x<X_BAN; x++){
			lcd_clearA(x*X_KOMA, y*Y_KOMA, (x+1)*X_KOMA-1, (y+1)*Y_KOMA-1, ban[y][x]);
		}
	}
}
/********************************************************************************/
/*		test1																	*/
/*		return 1:置ける, 0:置けない												*/
/********************************************************************************/
static int test1()
{
	char ban_save[Y_BAN][X_BAN];
	
	memcpy(ban_save, ban, sizeof(ban_save));
	return put_koma(ban_save);				// return 1:置ける
}
/********************************************************************************/
/*		rotate																	*/
/********************************************************************************/
static void rotate_sub(int x, int y, int *xx, int *yy)
{
	int tmp;
	
	tmp = 3-x;
	*xx = y;
	*yy = tmp;
}

static void rotate()
{
	int x, y, xx, yy;
	char koma_save[4][4];
	
	for(y=0; y<4; y++){
		for(x=0; x<4; x++){
			rotate_sub(x, y, &xx, &yy);
			koma_save[yy][xx] = koma[y][x];
		}
	}
	memcpy(koma, koma_save, sizeof(koma));
}
/********************************************************************************/
/*		put_koma																*/
/*		return 1:置ける, 0:置けない												*/
/********************************************************************************/
static int put_koma(char ban[Y_BAN][X_BAN])
{
	char ban_save[Y_BAN][X_BAN];
	int x, y, xx, yy;
	
	memcpy(ban_save, ban, sizeof(ban_save));
	for(y=0; y<4; y++){
		for(x=0; x<4; x++){
			if(koma[y][x]==0){
				continue;
			}
			yy = y_koma+y;
			if(yy<0 || yy>=Y_BAN){
				continue;
			}
			xx = x_koma+x;
			if(xx<0 || xx>=X_BAN){
				continue;
			}
			if(ban_save[yy][xx]){			// 置けない
				return 0;
			}
			ban_save[yy][xx] = koma[y][x];
		}
	}
	memcpy(ban, ban_save, sizeof(ban_save));
	return 1;								// 置ける
}
