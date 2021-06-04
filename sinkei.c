#include "app.h"


#define X_PIXCEL	14					// カード１枚のピクセル
#define Y_PIXCEL	25
#define X_NUMB		11					// カードを並べた時の横方向の枚数
#define Y_NUMB		5


extern const char heart[Y_PIXCEL][X_PIXCEL];
extern const char diamond[Y_PIXCEL][X_PIXCEL];
extern const char spade[Y_PIXCEL][X_PIXCEL];
extern const char clubs[Y_PIXCEL][X_PIXCEL];
extern const char card_ura[Y_PIXCEL*X_PIXCEL];


static const char *number[13]={
	"A","2","3","4","5","6","7","8","9","10","J","Q","K"
};
static const char *card_pat[4]={(char*)heart,(char*)diamond,(char*)spade,(char*)clubs};
static char desk_card[Y_NUMB][X_NUMB];			// 0x30:card, 0x0f:numb
static int ninzuu;
static int score[3];
static int player;

static void draw_card(int x, int y, int card, int numb);
static void initialize();
//static void draw_test();
static void draw_ura();
static void draw_score();
static void draw_player();
static int select_card();
static int end_game();


/********************************************************************************/
/*		sinkei_main																*/
/********************************************************************************/
static struct menu sinkei_menu[]={
	{0, 10,  1, "一人"},
	{0, 25,  2, "二人"},
	{0, 40,  3, "三人"},
	{0,0,0,0},
};

void sinkei_main()
{
	int cmd;
	
	for(;;){
		lcd_clear(RED_D);
		str_cdisp(60, "　神経衰弱　");
		str_cdisp(85, "K.Fukumoto");
		sw_updown();
		lcd_clear(BLACK);
		for(;;){
			cmd = menu_select(sinkei_menu);
			if(cmd>0)
				break;
		}
		ninzuu = cmd;						// 人数
		lcd_clear(WHITE);
		initialize();
		player = 0;
		for(;;){
			draw_ura();
			draw_score();
			draw_player();
			while(select_card()){
				score[player] += 2;
				draw_score();
			}
			if(++player >= ninzuu){
				player = 0;
			}
			if(end_game()){
				break;
			}
		}
	}
}
/********************************************************************************/
/*		select_card																*/
/*		return 1:合わせる事が出来た												*/
/********************************************************************************/
static void select_card_sub(int *x, int *y, int x1, int y1)
{
	int xx, yy, sw;
	
	*x = *y = 0;
	for(;;){
		xx = *x;
		yy = *y;
		
		//	カーソル表示
		lcd_clearA(xx*X_PIXCEL+5, yy*Y_PIXCEL+10, xx*X_PIXCEL+8, yy*Y_PIXCEL+14, BLACK);	// カーソル表示
		
		// スイッチ入力・移動
		sw = sw_updown();
		buzzer_div = 8;						// 1KHz
		buzzer_ms  = 50;
		if(sw & UP){
			if(--(*y) < 0)
				*y = 0;
		}
		if(sw & DOWN){
			if(++(*y) >= Y_NUMB)
				*y = Y_NUMB-1;
		}
		if(sw & RIGHT){
			if(++(*x) >= X_NUMB)
				*x = X_NUMB-1;
		}
		if(sw & LEFT){
			if(--(*x) < 0)
				*x = 0;
		}
		
		//	カーソル消去
		if(x1== xx && y1== yy){
			draw_card(xx*X_PIXCEL, yy*Y_PIXCEL, desk_card[yy][xx]>>4, desk_card[yy][xx]&0x0f);
		}
		else if(desk_card[yy][xx] >= 0){
			write_square(xx*X_PIXCEL, yy*Y_PIXCEL, X_PIXCEL, Y_PIXCEL, card_ura);
		}
		else
			lcd_clearA(xx*X_PIXCEL+5, yy*Y_PIXCEL+10, xx*X_PIXCEL+8, yy*Y_PIXCEL+14, WHITE);
		
		if(desk_card[*y][*x]>=0 && (sw & (A_SW | B_SW)) && (x1!=*x || y1!=*y)){
			break;
		}
	}
}

static int select_card()
{
	int x1, y1, x2, y2;
	
	select_card_sub(&x1, &y1, -1, -1);
	draw_card(x1*X_PIXCEL, y1*Y_PIXCEL, desk_card[y1][x1]>>4, desk_card[y1][x1]&0x0f);
	select_card_sub(&x2, &y2, x1, y1);
	draw_card(x2*X_PIXCEL, y2*Y_PIXCEL, desk_card[y2][x2]>>4, desk_card[y2][x2]&0x0f);
	wait_ms(3000);
	
	if((desk_card[y1][x1]&0xf)==(desk_card[y2][x2]&0xf)){	// 合った
		desk_card[y1][x1] = -1;
		desk_card[y2][x2] = -1;
		lcd_clearA(x1*X_PIXCEL, y1*Y_PIXCEL, (x1+1)*X_PIXCEL-1, (y1+1)*Y_PIXCEL-1, WHITE);
		lcd_clearA(x2*X_PIXCEL, y2*Y_PIXCEL, (x2+1)*X_PIXCEL-1, (y2+1)*Y_PIXCEL-1, WHITE);
		return 1;							// 合った
	}
	else{
		write_square(x1*X_PIXCEL, y1*Y_PIXCEL, X_PIXCEL, Y_PIXCEL, card_ura);
		write_square(x2*X_PIXCEL, y2*Y_PIXCEL, X_PIXCEL, Y_PIXCEL, card_ura);
		return 0;							// 合わなかった
	}
}
/********************************************************************************/
/*		end_game																*/
/********************************************************************************/
static int end_game()
{
	int i;
	
	for(i=0; i<52; i++){
		if(((char*)desk_card)[i] >= 0)
			return 0;						// まだカードが有る
	}
	
	str_disp(0, 135, "ゲーム終了　　　　");
	return 1;								// 終了
}
/********************************************************************************/
/*		draw_player																*/
/********************************************************************************/
static void draw_player()
{
	char buf[50];
	
	sprintf(buf, "No%d の番です", player+1);
	str_disp(0, 135, buf);
}
/********************************************************************************/
/*		draw_score																*/
/********************************************************************************/
static void draw_score()
{
	int i;
	char buf[50];
	
	buf[0] = '\0';
	for(i=0; i<ninzuu; i++){
		sprintf(&buf[strlen(buf)], "No%d:%d ", i+1, score[i]);
	}
	back_color = WHITE;
	text_color = BLACK;
	str_disp(0, 125, buf);
}
/********************************************************************************/
/*		draw_ura																*/
/********************************************************************************/
static void draw_ura()
{
	int i, x, y;
	
	for(i=0; i<52; i++){
		x = i%X_NUMB;
		y = i/X_NUMB;
		if(desk_card[y][x] >= 0){
			write_square(x*X_PIXCEL, y*Y_PIXCEL, X_PIXCEL, Y_PIXCEL, card_ura);
		}
	}
}
/********************************************************************************/
/*		draw_test																*/
/********************************************************************************/
#if 0
static void draw_test()
{
	int x, y;
	
	for(y=0; y<Y_NUMB; y++){
		for(x=0; x<X_NUMB; x++){
			if(desk_card[y][x] >= 0){
				draw_card(x*X_PIXCEL, y*Y_PIXCEL, desk_card[y][x]>>4, desk_card[y][x]&0x0f);
			}
			else{
				lcd_clearA(x*X_PIXCEL, y*Y_PIXCEL, (x+1)*X_PIXCEL-1, (y+1)*Y_PIXCEL-1, WHITE);
			}
		}
	}
}
#endif
/********************************************************************************/
/*		initialize																*/
/********************************************************************************/
static void initialize()
{
	int i, j, cnt, x, y, xx, yy, rnd, tmp;
	
	memset(desk_card, -1, sizeof(desk_card));
    cnt = 0;
	for(i=0; i<4; i++){
		for(j=0; j<13; j++){
			((char*)desk_card)[cnt++] = i*0x10 + j;
		}
	}
	for(i=0; i<10; i++){
		for(j=0; j<52; j++){
			x = j%11;
			y = j/11;
			rnd = MCrnd(52);
			xx = rnd%11;
			yy = rnd/11;
			tmp = desk_card[y][x];
			desk_card[y][x] = desk_card[yy][xx];
			desk_card[yy][xx] = tmp;
		}
	}
}
/********************************************************************************/
/*		draw_card																*/
/*		card 0:ハート、1:ダイヤ、2:スペード、3:クラブ							*/
/*		numb  : 0～(13-1)														*/
/********************************************************************************/
static void draw_card(int x, int y, int card, int numb)
{
	write_square(x,y, X_PIXCEL,Y_PIXCEL, card_pat[card]);
	back_color = WHITE;
	if(card < 2){
		text_color = RED;
	}
	else{
		text_color = BLACK;
	}
	if(strlen(number[numb])==1){
		str_disp(x+3, y+1, number[numb]);
	}
	else{
		str_disp(x+0, y+1, number[numb]);
	}
}








#define R	RED
#define W	WHITE
#define B	BLACK
#define G	GREEN

const char heart[Y_PIXCEL][X_PIXCEL]={
	//           |
	{R,R,R,R,R,R,R,R,R,R,R,R,R,R},	//0
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},	//5
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},	//10
	{R,W,W,R,R,W,W,W,R,R,W,W,W,R},
	{R,W,R,R,R,R,W,R,R,R,R,W,W,R},
	{R,W,R,R,R,R,R,R,R,R,R,W,W,R},
	{R,W,R,R,R,R,R,R,R,R,R,W,W,R},
	{R,W,R,R,R,R,R,R,R,R,R,W,W,R},	//15
	{R,W,W,R,R,R,R,R,R,R,W,W,W,R},
	{R,W,W,R,R,R,R,R,R,R,W,W,W,R},
	{R,W,W,W,R,R,R,R,R,W,W,W,W,R},
	{R,W,W,W,R,R,R,R,R,W,W,W,W,R},
	{R,W,W,W,W,R,R,R,W,W,W,W,W,R},	//20
	{R,W,W,W,W,R,R,R,W,W,W,W,W,R},
	{R,W,W,W,W,W,R,W,W,W,W,W,W,R},
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},
	{R,R,R,R,R,R,R,R,R,R,R,R,R,R},
};
const char diamond[Y_PIXCEL][X_PIXCEL]={
	//           |
	{R,R,R,R,R,R,R,R,R,R,R,R,R,R},	//0
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},	//5
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},
	{R,W,W,W,W,W,R,W,W,W,W,W,W,R},	//10
	{R,W,W,W,W,W,R,W,W,W,W,W,W,R},
	{R,W,W,W,W,R,R,R,W,W,W,W,W,R},
	{R,W,W,W,W,R,R,R,W,W,W,W,W,R},
	{R,W,W,W,R,R,R,R,R,W,W,W,W,R},
	{R,W,W,W,R,R,R,R,R,W,W,W,W,R},	//15
	{R,W,W,R,R,R,R,R,R,R,W,W,W,R},
	{R,W,W,W,R,R,R,R,R,W,W,W,W,R},
	{R,W,W,W,R,R,R,R,R,W,W,W,W,R},
	{R,W,W,W,W,R,R,R,W,W,W,W,W,R},
	{R,W,W,W,W,R,R,R,W,W,W,W,W,R},	//20
	{R,W,W,W,W,W,R,W,W,W,W,W,W,R},
	{R,W,W,W,W,W,R,W,W,W,W,W,W,R},
	{R,W,W,W,W,W,W,W,W,W,W,W,W,R},
	{R,R,R,R,R,R,R,R,R,R,R,R,R,R},
};
const char spade[Y_PIXCEL][X_PIXCEL]={
	//           |
	{B,B,B,B,B,B,B,B,B,B,B,B,B,B},	//0
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},	//5
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},
	{B,W,W,W,W,W,B,W,W,W,W,W,W,B},	//10
	{B,W,W,W,W,B,B,B,W,W,W,W,W,B},
	{B,W,W,W,B,B,B,B,B,W,W,W,W,B},
	{B,W,W,W,B,B,B,B,B,W,W,W,W,B},
	{B,W,W,B,B,B,B,B,B,B,W,W,W,B},
	{B,W,W,B,B,B,B,B,B,B,W,W,W,B},	//15
	{B,W,B,B,B,B,B,B,B,B,B,W,W,B},
	{B,W,B,B,B,B,B,B,B,B,B,W,W,B},
	{B,W,B,B,B,B,B,B,B,B,B,W,W,B},
	{B,W,B,B,B,B,B,B,B,B,B,W,W,B},
	{B,W,W,B,B,W,B,W,B,B,W,W,W,B},	//20
	{B,W,W,W,W,W,B,W,W,W,W,W,W,B},
	{B,W,W,W,W,B,B,B,W,W,W,W,W,B},
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},
	{B,B,B,B,B,B,B,B,B,B,B,B,B,B},
};
const char clubs[Y_PIXCEL][X_PIXCEL]={
	//           |
	{B,B,B,B,B,B,B,B,B,B,B,B,B,B},	//0
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},	//5
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},	//10
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},
	{B,W,W,W,W,W,B,B,W,W,W,W,W,B},
	{B,W,W,W,W,B,B,B,B,W,W,W,W,B},
	{B,W,W,W,W,B,B,B,B,W,W,W,W,B},
	{B,W,W,W,W,B,B,B,B,W,W,W,W,B},	//15
	{B,W,W,B,B,W,B,B,W,B,B,W,W,B},
	{B,W,B,B,B,B,W,W,B,B,B,B,W,B},
	{B,W,B,B,B,B,B,B,B,B,B,B,W,B},
	{B,W,B,B,B,B,B,B,B,B,B,W,W,B},
	{B,W,W,B,B,W,B,B,W,B,B,W,W,B},	//20
	{B,W,W,W,W,W,B,B,W,W,W,W,W,B},
	{B,W,W,W,W,B,B,B,B,W,W,W,W,B},
	{B,W,W,W,W,W,W,W,W,W,W,W,W,B},
	{B,B,B,B,B,B,B,B,B,B,B,B,B,B},
};
const char card_ura[Y_PIXCEL*X_PIXCEL]={
	//           |
	B,B,B,B,B,B,B,B,B,B,B,B,B,B,	//0
	B,W,W,W,G,G,G,W,W,W,G,G,G,B,
	B,W,W,W,G,G,G,W,W,W,G,G,G,B,
	B,W,W,W,G,G,G,W,W,W,G,G,G,B,
	B,W,W,W,G,G,G,W,W,W,G,G,G,B,
	B,G,G,G,W,W,W,G,G,G,W,W,W,B,	//5
	B,G,G,G,W,W,W,G,G,G,W,W,W,B,
	B,G,G,G,W,W,W,G,G,G,W,W,W,B,
	B,G,G,G,W,W,W,G,G,G,W,W,W,B,
	B,W,W,W,G,G,G,W,W,W,G,G,G,B,
	B,W,W,W,G,G,G,W,W,W,G,G,G,B,	//10
	B,W,W,W,G,G,G,W,W,W,G,G,G,B,
	B,W,W,W,G,G,G,W,W,W,G,G,G,B,
	B,G,G,G,W,W,W,G,G,G,W,W,W,B,
	B,G,G,G,W,W,W,G,G,G,W,W,W,B,
	B,G,G,G,W,W,W,G,G,G,W,W,W,B,	//15
	B,G,G,G,W,W,W,G,G,G,W,W,W,B,
	B,W,W,W,G,G,G,W,W,W,G,G,G,B,
	B,W,W,W,G,G,G,W,W,W,G,G,G,B,
	B,W,W,W,G,G,G,W,W,W,G,G,G,B,
	B,W,W,W,G,G,G,W,W,W,G,G,G,B,	//20
	B,G,G,G,W,W,W,G,G,G,W,W,W,B,
	B,G,G,G,W,W,W,G,G,G,W,W,W,B,
	B,G,G,G,W,W,W,G,G,G,W,W,W,B,
	B,B,B,B,B,B,B,B,B,B,B,B,B,B,
};
