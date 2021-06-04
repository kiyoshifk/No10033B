#include "app.h"


#define B	0
#define W	0x7
#define C	0x3
#define D	0x2
#define F	0x1
#define G	0x4

//#define C	0x07e0
//#define D	0x03ef
//#define F	0x001f
//#define G	0xf800


static const char mon1[10][10]={					// モンスター１
	{0,0,0,0,C,C,0,0,0,0},
	{0,0,C,C,C,C,C,C,0,0},
	{0,0,C,C,C,C,C,C,0,0},
	{0,C,C,C,C,C,C,C,C,0},
	{0,C,0,0,C,C,0,0,C,0},
	{0,C,0,0,C,C,0,0,C,0},
	{C,C,C,C,C,C,C,C,C,C},
	{C,C,C,C,C,C,C,C,C,C},
	{C,C,0,C,C,C,C,0,C,C},
	{C,0,0,0,C,C,0,0,0,C},
};
static const char mon2[10][10]={					// モンスター２
	{0,0,0,0,D,D,0,0,0,0},
	{0,0,D,D,D,D,D,D,0,0},
	{0,0,D,D,D,D,D,D,0,0},
	{0,D,D,D,D,D,D,D,D,0},
	{0,D,0,0,D,D,0,0,D,0},
	{0,D,0,0,D,D,0,0,D,0},
	{D,D,D,D,D,D,D,D,D,D},
	{D,D,D,D,D,D,D,D,D,D},
	{D,D,0,D,D,D,D,0,D,D},
	{D,0,0,0,D,D,0,0,0,D},
};
static const char mon3[10][10]={					// モンスター３
	{0,0,0,0,F,F,0,0,0,0},
	{0,0,F,F,F,F,F,F,0,0},
	{0,0,F,F,F,F,F,F,0,0},
	{0,F,F,F,F,F,F,F,F,0},
	{0,F,0,0,F,F,0,0,F,0},
	{0,F,0,0,F,F,0,0,F,0},
	{F,F,F,F,F,F,F,F,F,F},
	{F,F,F,F,F,F,F,F,F,F},
	{F,F,0,F,F,F,F,0,F,F},
	{F,0,0,0,F,F,0,0,0,F},
};
static const char mon4[10][10]={					// モンスター４
	{0,0,0,0,G,G,0,0,0,0},
	{0,0,G,G,G,G,G,G,0,0},
	{0,0,G,G,G,G,G,G,0,0},
	{0,G,G,G,G,G,G,G,G,0},
	{0,G,0,0,G,G,0,0,G,0},
	{0,G,0,0,G,G,0,0,G,0},
	{G,G,G,G,G,G,G,G,G,G},
	{G,G,G,G,G,G,G,G,G,G},
	{G,G,0,G,G,G,G,0,G,G},
	{G,0,0,0,G,G,0,0,0,G},
};
static const char pac0[10][10]={
	{0,0,0,W,W,W,W,0,0,0},
	{0,W,W,W,W,W,W,W,W,0},
	{0,W,W,W,W,W,W,W,W,0},
	{W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W},
	{0,W,W,W,W,W,W,W,W,0},
	{0,W,W,W,W,W,W,W,W,0},
	{0,0,0,W,W,W,W,0,0,0},
};
static const char pac1[10][10]={
	{0,0,0,0,0,0,0,0,0,0},
	{0,W,0,0,0,0,0,0,W,0},
	{0,W,W,0,0,0,0,W,W,0},
	{W,W,W,W,0,0,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W},
	{0,W,W,W,W,W,W,W,W,0},
	{0,W,W,W,W,W,W,W,W,0},
	{0,0,0,W,W,W,W,0,0,0},
};
static const char pac2[10][10]={
	{0,0,0,W,W,W,W,0,0,0},
	{0,W,W,W,W,W,W,W,W,0},
	{0,W,W,W,W,W,W,W,0,0},
	{W,W,W,W,W,W,W,0,0,0},
	{W,W,W,W,W,W,0,0,0,0},
	{W,W,W,W,W,W,0,0,0,0},
	{W,W,W,W,W,W,W,0,0,0},
	{0,W,W,W,W,W,W,W,0,0},
	{0,W,W,W,W,W,W,W,W,0},
	{0,0,0,W,W,W,W,0,0,0},
};
static const char pac3[10][10]={
	{0,0,0,W,W,W,W,0,0,0},
	{0,W,W,W,W,W,W,W,W,0},
	{0,W,W,W,W,W,W,W,W,0},
	{W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W,W,W},
	{W,W,W,W,0,0,W,W,W,W},
	{0,W,W,0,0,0,0,W,W,0},
	{0,W,0,0,0,0,0,0,W,0},
	{0,0,0,0,0,0,0,0,0,0},
};
static const char pac4[10][10]={
	{0,0,0,W,W,W,W,0,0,0},
	{0,W,W,W,W,W,W,W,W,0},
	{0,0,W,W,W,W,W,W,W,0},
	{0,0,0,W,W,W,W,W,W,W},
	{0,0,0,0,W,W,W,W,W,W},
	{0,0,0,0,W,W,W,W,W,W},
	{0,0,0,W,W,W,W,W,W,W},
	{0,0,W,W,W,W,W,W,W,0},
	{0,W,W,W,W,W,W,W,W,0},
	{0,0,0,W,W,W,W,0,0,0},
};


static const char patN[10][10]={
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
};
static const char patE[10][10]={
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,W,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
	{B,B,B,B,B,B,B,B,B,B},
};

#define SCRN_TOP		0xa0000000

#define N				0	// 何もなし
#define K				1	// 壁
#define E				2	// 餌

#define TICK			100		// msec

#define X_SIZE			22
#define Y_SIZE			22

static const char init_ban[Y_SIZE][X_SIZE]={
	{K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,},
	{K,E,E,E,E,E,E,E,E,E,K,E,E,E,E,E,E,E,E,E,E,K,},
	{K,E,K,K,K,E,K,K,K,E,K,E,K,K,K,E,K,E,K,K,E,K,},
	{K,E,K,K,K,E,K,K,K,E,K,E,K,K,K,E,K,E,K,K,E,K,},
	{K,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,K,},
	{K,K,K,K,K,E,K,E,K,K,K,K,K,E,K,E,K,K,K,K,E,K,},
	{K,K,K,K,K,E,K,E,K,K,K,K,K,E,K,E,K,K,K,K,E,K,},
	{K,E,E,E,E,E,K,E,E,E,K,E,E,E,K,E,E,E,E,E,E,K,},
	{K,E,K,K,K,E,K,K,K,E,K,E,K,K,K,E,K,K,E,K,K,K,},
	{K,E,K,N,K,E,K,E,E,E,E,E,E,E,K,E,K,E,E,E,E,K,},
	{K,E,K,K,K,E,K,E,K,K,E,K,K,E,K,E,K,E,K,K,E,K,},
	{K,E,E,E,E,E,E,E,K,E,E,E,K,E,K,E,K,E,K,K,E,K,},
	{K,E,K,K,K,K,K,E,K,K,K,K,K,E,E,E,E,E,E,E,E,K,},
	{K,E,K,K,K,K,K,E,E,E,E,E,E,E,K,K,K,K,K,E,K,K,},
	{K,E,E,E,K,E,E,E,K,K,K,K,K,E,K,K,K,K,K,E,K,K,},
	{K,K,K,E,K,E,K,E,K,K,K,K,K,E,E,E,E,E,E,E,E,K,},
	{K,E,E,E,E,E,K,E,E,E,K,K,E,E,E,K,E,K,K,K,K,K,},
	{K,E,K,K,K,E,K,K,K,E,E,E,E,K,E,K,E,E,E,E,E,K,},
	{K,E,K,N,K,E,K,K,K,E,K,K,E,K,E,K,E,K,K,K,E,K,},
	{K,E,K,K,K,E,E,E,E,E,K,K,E,K,E,K,E,K,K,K,E,K,},
	{K,E,E,E,E,E,K,K,K,E,E,E,E,K,E,E,E,E,E,E,E,K,},
	{K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,},
};
#define PAC_INIT_POS_X	21
#define PAC_INIT_POS_Y	21
#define MON_INIT_POS_X	11
#define MON_INIT_POS_Y	12

static char xpos, ypos;							// 1～X_SIZE/Y_SIZE
static char mon_pos[4][2];
static char mon_dir[4][2]={
	{1,1},
	{1,1},
	{1,1},
	{1,1}
};
static int score;
static char ban[Y_SIZE][X_SIZE];
static int level;

/********************************************************************************/
/*		disp_pat																*/
/********************************************************************************/
static void disp_pat(int x, int y, const char *pat)
{
	int xx, yy;
	
	xx = (x-1)*(10/2+1);						// この値に +8 で中心
	yy = (y-1)*(10/2+1)+10;
	write_square(xx, yy, 10, 10, pat);
}
/********************************************************************************/
/*		disp_ban																*/
/********************************************************************************/
static void disp_ban1(int x, int y)
{
	int k, xx, yy;
	
	xx = (x-1)*(10/2+1)+(10/2);
	yy = (y-1)*(10/2+1)+(10/2)+10;
	k = ban[y-1][x-1];
	if(k==E){
		disp_pat(x, y, (char*)patE);
	}
	else if(k==N){
		disp_pat(x, y, (char*)patN);
	}
	else if(k==K){
		disp_pat(x, y, (char*)patN);
		if(ban[y-2][x-1]==K && y!=1){		// 上: 壁
			lcd_clearA(xx, yy-5, xx+1, yy+1, WHITE);
		}
		if(ban[y-1][x-2]==K && x!=1){		// 左: 壁
			lcd_clearA(xx-5, yy, xx+1, yy+1, WHITE);
		}
		if(ban[y][x-1]==K && y!=Y_SIZE){	// 下: 壁
			lcd_clearA(xx, yy, xx+1, yy+6, WHITE);
		}
		if(ban[y-1][x]==K && x!=X_SIZE){	// 右: 壁
			lcd_clearA(xx, yy, xx+6, yy+1, WHITE);
		}
	}
}

static void disp_ban()
{
	int x, y;
	
	for(y=1; y<=Y_SIZE; y++){
		for(x=1; x<=X_SIZE; x++){
			disp_ban1(x, y);
		}
	}
}
/********************************************************************************/
/*		pac_move																*/
/********************************************************************************/
static int pac_move()
{
	int c, x, y;
	
	x = xpos;
	y = ypos;
	c = sw_sense();
	if(c==UP){
		y--;
		if(y<1) y = 1;
	}
	else if(c==RIGHT){
		x++;
		if(x>X_SIZE) x = X_SIZE;
	}
	else if(c==DOWN){
		y++;
		if(y>Y_SIZE) y = Y_SIZE;
	}
	else if(c==LEFT){
		x--;
		if(x<1) x = 1;
	}
	if(ban[y-1][x-1] != K){						// 壁でなければ
		xpos = x;
		ypos = y;
		if(ban[ypos-1][xpos-1]==E){				// 餌を食べたら
			ban[ypos-1][xpos-1] = N;
			score += 10;						// 10点追加
//			buzzer(1000, 20);					// 1KHz 20ms
			buzzer_div = 8;						// 1KHz
			buzzer_ms = 20;
		}
	}
	return c;
}
/********************************************************************************/
/*		pac_disp																*/
/********************************************************************************/
static void pac_disp(int c)
{
	if(c==UP){
		disp_pat(xpos, ypos, (char*)pac1);
	}
	else if(c==RIGHT){
		disp_pat(xpos, ypos, (char*)pac2);
	}
	else if(c==DOWN){
		disp_pat(xpos, ypos, (char*)pac3);
	}
	else if(c==LEFT){
		disp_pat(xpos, ypos, (char*)pac4);
	}
	else{
		disp_pat(xpos, ypos, (char*)pac0);			// 口を閉じる
		return;
	}
}
/********************************************************************************/
/*		score_disp																*/
/********************************************************************************/
static int score_disp()
{
	int i;
	char buf[50];
	
	sprintf(buf, "Score:%d     ", score);
	str_disp(0, 0, buf);
	
	for(i=0; i<X_SIZE*Y_SIZE; i++){
		if(((char*)ban)[i]==E)				// まだ餌が残っている
			return 0;						// normal
	}
	return 1;								// １面クリアした
}
/********************************************************************************/
/*		erase_mon																*/
/********************************************************************************/
static void erase_mon()
{
	int i;
	
	for(i=0; i<4; i++){
		disp_ban1(mon_pos[i][0], mon_pos[i][1]);	// 消す
	}
}
/********************************************************************************/
/*		move_mon																*/
/********************************************************************************/
static void move_mon_sub(int i, int *cnt, int dir[4][2])
{
	int j, x, y;
	
	x = mon_pos[i][0];
	y = mon_pos[i][1];
	*cnt = 0;
	if(ban[y-1][x-1+1] != K){			// 壁でなければ動ける
		dir[*cnt][0] = 1;
		dir[*cnt][1] = 0;
		(*cnt)++;
	}
	if(ban[y-1][x-1-1] != K){			// 壁でなければ動ける
		dir[*cnt][0] = -1;
		dir[*cnt][1] = 0;
		(*cnt)++;
	}
	if(ban[y-1+1][x-1] != K){			// 壁でなければ動ける
		dir[*cnt][0] = 0;
		dir[*cnt][1] = 1;
		(*cnt)++;
	}
	if(ban[y-1-1][x-1] != K){			// 壁でなければ動ける
		dir[*cnt][0] = 0;
		dir[*cnt][1] = -1;
		(*cnt)++;
	}
	if(*cnt >= 2){
		for(j=0; j<*cnt; j++){
			if(mon_dir[i][0]== -dir[j][0] && mon_dir[i][1]== -dir[j][1]){
				memmove(dir[j], dir[j+1], sizeof(int)*2*(3-j));
				(*cnt)--;
				break;
			}
		}
	}
}

static void move_mon_select(int i, int cnt, int dir[4][2])
{
	int j, d[4], c, rnd;
	
	c = 0;
	for(j=0; j<cnt; j++){
		if(dir[j][0]>0 && xpos>mon_pos[i][0]){
			d[c++] = j;
		}
		if(dir[j][0]<0 && xpos<mon_pos[i][0]){
			d[c++] = j;
		}
		if(dir[j][1]>0 && ypos>mon_pos[i][1]){
			d[c++] = j;
		}
		if(dir[j][1]<0 && ypos<mon_pos[i][1]){
			d[c++] = j;
		}
	}
	if(c && MCrnd(100)<50){
		rnd = MCrnd(c);
		rnd = d[rnd];
	}
	else{
		rnd = MCrnd(cnt);
	}
	mon_dir[i][0] = dir[rnd][0];
	mon_dir[i][1] = dir[rnd][1];
}

static void move_mon()
{
	int i, x, y, cnt, dir[4][2], rnd;
	
	for(i=0; i<4; i++){
		if(MCrnd(100) < level)
			continue;
		x = mon_pos[i][0] + mon_dir[i][0];
		y = mon_pos[i][1] + mon_dir[i][1];
		if(ban[y-1][x-1] != K){				// 壁でなければ動ける
			mon_pos[i][0] = x;
			mon_pos[i][1] = y;
			move_mon_sub(i, &cnt, dir);
			if(cnt >= 2){
				if(i<=1){
					move_mon_select(i, cnt, dir);
					continue;
				}
				rnd = MCrnd(cnt);
				mon_dir[i][0] = dir[rnd][0];
				mon_dir[i][1] = dir[rnd][1];
				continue;
			}
			continue;
		}
		move_mon_sub(i, &cnt, dir);
		if(i<=1){
			move_mon_select(i, cnt, dir);
			continue;
		}
		rnd = MCrnd(cnt);
		mon_dir[i][0] = dir[rnd][0];
		mon_dir[i][1] = dir[rnd][1];
	}
}
/********************************************************************************/
/*		disp_mon																*/
/********************************************************************************/
static int disp_mon()
{
	int i;
	
	disp_pat(mon_pos[0][0], mon_pos[0][1], (char*)mon1);
	disp_pat(mon_pos[1][0], mon_pos[1][1], (char*)mon2);
	disp_pat(mon_pos[2][0], mon_pos[2][1], (char*)mon3);
	disp_pat(mon_pos[3][0], mon_pos[3][1], (char*)mon4);

	for(i=0; i<4; i++){
		if(xpos==mon_pos[i][0] && ypos==mon_pos[i][1]){		// モンスターに掴まった
//			buzzer(200, 1000);				// 200Hz 1秒
			buzzer_div = 32;					// 250Hz
			buzzer_ms = 1000;
			sw_updown();
			return 1;						// game over
		}
	}
	return 0;								// normal end
}
/********************************************************************************/
/*		main																	*/
/********************************************************************************/
void esakui_main()
{
	int c;
	unsigned int time1, time2;
	
	for(;;){
		lcd_clear(GREEN);			// Green
		str_cdisp(60, "餌食いゲーム");
		str_cdisp(85, "K.Fukumoto");
		sw_updown();
		lcd_clear(BLACK);
	
		init_genrand(TickCount);
		level = 70;
		score = 0;
		for(;;){
			memcpy(ban, init_ban, sizeof(ban));
			disp_ban();							// 空の盤パターンセット
			xpos = PAC_INIT_POS_X;
			ypos = PAC_INIT_POS_Y;
			for(c=0; c<4; c++){
				mon_pos[c][0] = MON_INIT_POS_X;
				mon_pos[c][1] = MON_INIT_POS_Y;
			}
			time1 = TickCount;
			for(;;){
				drawcount = 0;
				while(drawcount==0)
					;
				disp_ban1(xpos, ypos);			// パックマンを消す
				c = pac_move();					// パックマン移動
				pac_disp(c);					// パックマンを表示する
				
				erase_mon();					// モンスターを消す
				move_mon();						// モンスター移動
				if(disp_mon()){					// モンスターを表示する
					wait_ms(2000);
					goto new_game;
				}
				
				if(score_disp())				// １面クリアした
					break;
				while((time2=TickCount)-time1 < TICK)
					;
				time1 = time2;
			}
			level -= 30;
			if(level < 0)
				level = 0;
		}
new_game:;
	}
}
