#include "app.h"


#define H_WIN 160                      // ウィンドウの幅
#define W_WIN 140                      // ウィンドウの高さ

#define W2_HODAI 6                     // 砲台の横幅の半分
#define H_HODAI 143                    // 砲台の上面のy座標
#define L_HODAI 149                    // 砲台の下面のy座標
#define V_HODAI 5                      // 砲台の移動速度
#define L_E_BEAM 14                    // 防衛軍のビームの長さ
#define V_E_BEAM 10                    // 防衛軍のビームの速度

#define L_I_BEAM 7                     // インベーダー軍のビームの長さ
#define V_I_BEAM 2                     // インベーダー軍のビームの速度
#define P_I_BEAM 200                   // インベーダー軍のビームの初期発射確率

#define N_I_BEAM 5                     // インベーダー軍のビームの画面上の最大数
#define NXIV     7                     // インベーダー軍の列の数
#define NYIV     4                     // インベーダー軍の行の数
#define V_INVADER 1                    // インベーダー軍の速度

#define NOT_DECIDE 0
#define INVADER 1
#define HUMAN 2

#define CHARA_H			8				// キャラクタの高さ
#define CHARA1_W		12				// キャラクタ１の横幅
#define CHARA2_W		11				// キャラクタ２の横幅
#define CHARA3_W		8				// キャラクタ３の横幅
#define CHARA_DH		14				// 高さ方向のキャラクタ間隔
#define CHARA_DW		14				// 横方向のキャラクタ間隔

//---- グローバル変数 -------
static int xc = 100;                               // マウスのx座標
static unsigned char invd_status[NXIV][NYIV];		// 0:死亡、1:生存
static int invd_x[NXIV][NYIV];
static int invd_y[NXIV][NYIV];

static int xc_old;
static unsigned char invd_status_old[NXIV][NYIV];
static int invd_x_old[NXIV][NYIV];
static int invd_y_old[NXIV][NYIV];

static int alive_inv=NXIV*NYIV;                    // 生きているインベーダーの数
static int inv_vx=V_INVADER;                       // インベーダーの横方向の速度

static char e_beam_status;                 // 地球防衛軍のビーム
static int e_beam_x;
static int e_beam_y0;
static int e_beam_y1;
static int e_beam_vy;

static char e_beam_status_old;                 // 地球防衛軍のビーム
static int e_beam_x_old;
static int e_beam_y0_old;
static int e_beam_y1_old;
static int e_beam_vy_old;

static int p_e_beam1;

static char i_beam_status[N_I_BEAM];
static int i_beam_x[N_I_BEAM];
static int i_beam_y0[N_I_BEAM];
static int i_beam_y1[N_I_BEAM];
static int i_beam_vy[N_I_BEAM];

static char i_beam_status_old[N_I_BEAM];
static int i_beam_x_old[N_I_BEAM];
static int i_beam_y0_old[N_I_BEAM];
static int i_beam_y1_old[N_I_BEAM];
static int i_beam_vy_old[N_I_BEAM];

static int winner = NOT_DECIDE;
static int score;


static void initialize();
static void change_state();
static int draw();
static int draw_result();
static void score_disp();
static void draw_hodai();
static void draw_e_beam();
static void draw_i_beam();
static void draw_invader();
static void state_hodai();
static void state_e_beam();
static void state_invader();
static void state_i_beam();
static void shoot();
//static long MCrnd(long i);


//====================================================================
// main関数
//====================================================================
void shooting_main()
{
	int flag;
	
	flag = 1;
	score = 0;
	back_color = BLACK;
	text_color = WHITE;
	for(;;){
		initialize();
		if(flag==1){
			lcd_clear(BRUE);
			str_cdisp(40, "シューティングゲーム");
			str_cdisp(60, "ＵＰ　　　：ビーム発射");
			str_cdisp(75, "Ｒｉｇｈｔ：砲台移動　");
			str_cdisp(90, "Ｌｅｆｔ　：砲台移動　");
			str_cdisp(105,"K.Fukumoto");
			sw_updown();
		}
		
		scrn_clear();
		for(;;){
			change_state();					// 状態変化処理
			if((flag = draw())){			// 画面表示
				wait_ms(2000);
				break;						// ゲーム終了
			}
		}
	}
}

//====================================================================
// 初期化
//====================================================================
static void initialize()
{
  int i, j;

	init_genrand(TickCount);

    e_beam_status=0;
    e_beam_y0=H_HODAI-L_E_BEAM;
    e_beam_y1=H_HODAI;
    e_beam_vy=0;

  e_beam_status=1;                       // 砲台にのせる
	p_e_beam1 = 0;

  for(i=0; i<N_I_BEAM; i++){
    i_beam_status[i] = 0;
    i_beam_y0[i] = 0;
    i_beam_y1[i] = 0;
    i_beam_vy[i] = V_I_BEAM;
  }

  for(i=0; i<NXIV; i++){
    for(j=0; j<NYIV; j++){
      invd_status[i][j]=1;
      invd_x[i][j] = CHARA_DW*(i+1);
      invd_y[i][j] = (NYIV-1-j)*CHARA_DH+CHARA_H/2 + 10;	// 10 はメッセージエリア
    }
  }
  winner = NOT_DECIDE;
//  score = 0;
}


//====================================================================
// 図を描く
//====================================================================
static int draw()
{
	if(winner != NOT_DECIDE){
		return draw_result();			// 終了
	}
	
	score_disp();
	draw_hodai();         // 砲台を描く関数呼び出し
	draw_e_beam();        // 地球防衛軍のビームを描く関数の呼び出し
	draw_i_beam();        // インベーダー軍のビームを描く関数の呼び出し
	draw_invader();       // インベーダーを描く関数の呼び出し
	return 0;								// normal
}


/////////////////////////////////
//	score_disp
/////////////////////////////////
static void score_disp()
{
	char buf[100];
	
	sprintf(buf, "Score %d", score);
	str_cdisp(0, buf);
}
//====================================================================
// 勝者の表示
//====================================================================
static int draw_result()
{
	if(winner==HUMAN)
		return 2;
	else{
//		str_cdisp(0, "    You are lose.    ");
		score = 0;
		sw_updown();
		return 1;
	}
}


//====================================================================
// 地球防衛軍の砲台の描画
//====================================================================
static void draw_hodai()
{
	int x, y;
	
	for(y=H_HODAI; y<=L_HODAI; y++){
		for(x=xc_old-W2_HODAI; x<=xc_old+W2_HODAI; x++){
			pset(x, y, BLACK);
		}
	}
	for(y=H_HODAI; y<=L_HODAI; y++){
		for(x=xc-W2_HODAI; x<=xc+W2_HODAI; x++){
			pset(x, y, WHITE);
		}
	}
}


//====================================================================
// 地球防衛軍のビーム砲の描画
//====================================================================
static void draw_e_beam()
{
	int  y;
	
	if(e_beam_status != 0){
		for(y=e_beam_y0_old; y<=e_beam_y1_old; y++){
			pset(e_beam_x_old, y, BLACK);
			pset(e_beam_x_old+1, y, BLACK);
		}
		for(y=e_beam_y0; y<=e_beam_y1; y++){
			pset(e_beam_x, y, GREEN);
			pset(e_beam_x+1, y, GREEN);
		}
	}
}


//====================================================================
// インベーダー軍のビームの描画
//====================================================================
static void draw_i_beam()
{
	int i, y;
	
	for(i=0; i<N_I_BEAM; i++){
		for(y=i_beam_y0_old[i]; y<=i_beam_y1_old[i]; y++){
			pset(i_beam_x_old[i], y, BLACK);
			pset(i_beam_x_old[i]+1, y, BLACK);
		}
		if(i_beam_status[i] == 2){
			for(y=i_beam_y0[i]; y<=i_beam_y1[i]; y++){
				pset(i_beam_x[i], y, RED);
				pset(i_beam_x[i]+1, y, RED);
			}
		}
	}
}


//====================================================================
// インベーダー軍の描画
//====================================================================

#define B 0
#define Y 0x6
#define C 0x3
#define M 0x5

static const char chara1[2][CHARA_H][CHARA1_W]={
	{
		{0,0,0,0,Y,Y,Y,Y,0,0,0,0},
		{0,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,0},
		{Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y},
		{Y,Y,Y,0,0,Y,Y,0,0,Y,Y,Y},
		{Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y},
		{0,0,0,Y,Y,0,0,Y,Y,0,0,0},
		{0,0,Y,Y,0,Y,Y,0,Y,Y,0,0},
		{Y,Y,0,0,0,0,0,0,0,0,Y,Y},
	},
	{
		{0,0,0,0,Y,Y,Y,Y,0,0,0,0},
		{0,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,0},
		{Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y},
		{Y,Y,Y,0,0,Y,Y,0,0,Y,Y,Y},
		{Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y},
		{0,0,Y,Y,Y,0,0,Y,Y,Y,0,0},
		{0,Y,Y,0,0,Y,Y,0,0,Y,Y,0},
		{0,0,Y,Y,0,0,0,0,Y,Y,0,0},
	},
};

static const char chara2[2][CHARA_H][CHARA2_W]={
	{
		{0,0,C,0,0,0,0,0,C,0,0},
		{C,0,0,C,0,0,0,C,0,0,C},
		{C,0,C,C,C,C,C,C,C,0,C},
		{C,C,C,0,C,C,C,0,C,C,C},
		{C,C,C,C,C,C,C,C,C,C,C},
		{0,C,C,C,C,C,C,C,C,C,0},
		{0,0,C,0,0,0,0,0,C,0,0},
		{0,C,0,0,0,0,0,0,0,C,0},
	},
	{
		{0,0,C,0,0,0,0,0,C,0,0},
		{0,0,0,C,0,0,0,C,0,0,0},
		{0,0,C,C,C,C,C,C,C,0,0},
		{0,C,C,0,C,C,C,0,C,C,0},
		{C,C,C,C,C,C,C,C,C,C,C},
		{C,0,C,C,C,C,C,C,C,0,C},
		{C,0,C,0,0,0,0,0,C,0,C},
		{0,0,0,C,C,0,C,C,0,0,0},
	},
};

static const char chara3[2][CHARA_H][CHARA3_W]={
	{
		{0,0,0,M,M,0,0,0},
		{0,0,M,M,M,M,0,0},
		{0,M,M,M,M,M,M,0},
		{M,M,0,M,M,0,M,M},
		{M,M,M,M,M,M,M,M},
		{0,M,0,M,M,0,M,0},
		{M,0,0,0,0,0,0,M},
		{0,M,0,0,0,0,M,0},
	},
	{
		{0,0,0,M,M,0,0,0},
		{0,0,M,M,M,M,0,0},
		{0,M,M,M,M,M,M,0},
		{M,M,0,M,M,0,M,M},
		{M,M,M,M,M,M,M,M},
		{0,0,M,0,0,M,0,0},
		{0,M,0,M,M,0,M,0},
		{M,0,M,0,0,M,0,M},
	},
};

static int invd_chara;

static void draw_invader()
{
	int i, j, x, y, xx, yy, tmp;
	
	for(i=0; i<NXIV; i++){
		for(j=0; j<NYIV; j++){
			if(invd_status_old[i][j]==1){	// 生きているインベーダーのみ消す
				xx = invd_x_old[i][j]-CHARA1_W/2;
				yy = invd_y_old[i][j]-CHARA_H/2;
				for(y=yy; y<yy+CHARA_H; y++){
					for(x=xx; x<xx+CHARA1_W; x++){
						pset(x, y, BLACK);
					}
				}
			}
			if(invd_status[i][j]==1){		// 生きているインベーダーのみ描く
				tmp = (invd_chara & 2)>>1;
				if(j<2){					// 下の２行
					xx = invd_x[i][j]-CHARA1_W/2;
					yy = invd_y[i][j]-CHARA_H/2;
					write_square(xx, yy, CHARA1_W, CHARA_H, (char*)chara1[tmp]);
				}
				else if(j==2){				// 下から３行目
					xx = invd_x[i][j]-CHARA2_W/2;
					yy = invd_y[i][j]-CHARA_H/2;
					write_square(xx, yy, CHARA2_W, CHARA_H, (char*)chara2[tmp]);
				}
				else{						// 一番上
					xx = invd_x[i][j]-CHARA3_W/2;
					yy = invd_y[i][j]-CHARA_H/2;
					write_square(xx, yy, CHARA3_W, CHARA_H, (char*)chara3[tmp]);
				}
				wait_ms(2);
			}
		}
	}
	invd_chara++;
	wait_ms(20);
}

//====================================================================
// PCが暇なときに実行する．これが実行されると状態が変化する
//====================================================================
static void change_state()
{
	xc_old = xc;
	memcpy(invd_status_old, invd_status, sizeof(invd_status_old));
	memcpy(invd_x_old, invd_x, sizeof(invd_x_old));
	memcpy(invd_y_old, invd_y, sizeof(invd_y_old));
	e_beam_status_old = e_beam_status;
	e_beam_x_old      = e_beam_x;
	e_beam_y0_old     = e_beam_y0;
	e_beam_y1_old     = e_beam_y1;
	e_beam_vy_old     = e_beam_vy;
	memcpy(i_beam_status_old, i_beam_status, sizeof(i_beam_status_old));
	memcpy(i_beam_x_old, i_beam_x, sizeof(i_beam_x_old));
	memcpy(i_beam_y0_old, i_beam_y0, sizeof(i_beam_y0_old));
	memcpy(i_beam_y1_old, i_beam_y1, sizeof(i_beam_y1_old));
	memcpy(i_beam_vy_old, i_beam_vy, sizeof(i_beam_vy_old));
	
	
  if(winner == NOT_DECIDE){
	state_hodai();		// 砲台の移動
    state_e_beam();     // 地球防衛軍のビームの処理
    state_invader();    // インベーダー軍の処理
    state_i_beam();     // インベーダー軍のビームの処理
  }
}


///////////////////////////////////
//	state_hodai
///////////////////////////////////
static void state_hodai()
{
	int c;
	
	c = sw_sense();					// キー入力
	if(c & UP){
		shoot();					// ビーム発射
	}
	else if(c & RIGHT){
		xc += V_HODAI;
		if(xc > W_WIN-1-W2_HODAI)
			xc = W_WIN-1-W2_HODAI;
	}
	else if(c & LEFT){
		xc -= V_HODAI;
		if(xc < W2_HODAI)
			xc = W2_HODAI;
	}
}
//====================================================================
// 地球防衛軍のビームの状態の処理
//====================================================================
static void state_e_beam()
{
  int l,m, tmp1, tmp2, tmp3;
  int st0;
  int nshoot;                // 発射済みの地球防衛軍の玉の数

	st0 = 0;
	nshoot = 0;

    switch(e_beam_status){

    //--------  格納庫にあるビームの処理 ------------------------
    case 0:
      st0=0;                    // 次に発射可能なビームを設定
      break;

    //--------  砲台にあるビームの処理 ------------------------
    case 1:
      e_beam_x = xc;         // x方向に移動
      break;

    //--------  すでに発射されたビームの処理 ------------------------
    case 2:
      nshoot++;                         // 発射されているビームをカウント
      e_beam_y0 -= e_beam_vy;     // ビームの移動
      e_beam_y1 -= e_beam_vy;

      // ------ インベーダーにビームが衝突したことを確認して処理 ------
      alive_inv = 0;
      for(l=0; l<NXIV; l++){    
		for(m=0; m<NYIV; m++){
		  if(invd_status[l][m]==1){
			tmp2 = CHARA_H/2;				// キャラクタの高さの 1/2
			if(m<2){						// 下の２行
				tmp1 = CHARA1_W/2;			// キャラクタの幅の 1/2
				tmp3 = 10;					// 点数
			}
			else if(m==2){					// 下から３行目
				tmp1 = CHARA2_W/2;
				tmp3 = 20;
			}
			else{							// 一番上
				tmp1 = CHARA3_W/2;
				tmp3 = 30;
			}
			alive_inv++;
		    if((invd_x[l][m]-tmp1 < e_beam_x) &&
					(e_beam_x < invd_x[l][m]+tmp1) &&
					(invd_y[l][m]-tmp2 < e_beam_y1) &&
					(e_beam_y0 < invd_y[l][m]+tmp2)){
				buzzer_div = 16;				// 500Hz
				buzzer_ms = 200;
		      invd_status[l][m]=0;          // インベーダーの死亡
		      score += tmp3;				// 点数計算
		      
		      e_beam_status=0;             // ビームは格納庫へ
		      e_beam_y0=H_HODAI-L_E_BEAM;  // ビームの初期化
		      e_beam_y1=H_HODAI;
		    }
		  }
		}      
      }
      if(alive_inv==0)winner=HUMAN;


      // ---- 画面から地球防衛軍のビームがはみ出た場合の処理 --------
      if(0 > e_beam_y0){
		e_beam_status = 0;
		e_beam_y0 = H_HODAI-L_E_BEAM;
		e_beam_y1 = H_HODAI;
		e_beam_vy = 0;
      }
      break;
    default:
      printf("e_beam status error!!\n");
      for(;;)
        ;
    }


  // --- 地球防衛軍の新たな発射可能なビームの処理 -----
	if(e_beam_status==0){
		e_beam_status=1;
		p_e_beam1 = st0;
	}
}


//====================================================================
// インベーダー軍の状態の処理
//====================================================================
static void state_invader()
{
	int tmp;
  int i, j, k;
  int ivmin_x, ivmax_x;
  int ivmin_y, ivmax_y;
  int can_attack;

	ivmin_x = W_WIN; ivmax_x = 0;
	ivmin_y = H_WIN; ivmax_y = 0;

  for(i=0; i<NXIV; i++){
    can_attack=1;
    for(j=0; j<NYIV; j++){
      if(invd_status[i][j]==1){   // インベーダーの生死のチェック
		invd_x[i][j] += inv_vx;   // インベーダーの横方向移動
		// ---- インベーダー軍のビーム発射の処理 ------
		tmp = MCrnd(P_I_BEAM);
		if(can_attack == 1 && tmp == 0){  // 発射条件
		  for(k=0; k<N_I_BEAM; k++){
		    if(i_beam_status[k] !=2){      // 発射可能なビームを探す
		      i_beam_status[k] =2;         // ビームの発射
		      i_beam_x[k] = invd_x[i][j];
		      i_beam_y0[k] = invd_y[i][j];
		      i_beam_y1[k] = invd_y[i][j]+L_I_BEAM;
		      can_attack=0;
		      break;
		    }
		  }
		}
		// --- インベーダー軍の左右上下の端の座標 -------
		if(invd_x[i][j] < ivmin_x) ivmin_x=invd_x[i][j];   // 左端 
		if(invd_x[i][j] > ivmax_x) ivmax_x=invd_x[i][j];   // 右端
		if(invd_y[i][j] < ivmin_y) ivmin_y=invd_y[i][j];   // 下の端
		if(invd_y[i][j] > ivmax_y) ivmax_y=invd_y[i][j];   // 上の端
      }
    }
  }
  if(ivmax_y > H_WIN-10)
  	winner=INVADER;            // 地球防衛軍の負け


  if(ivmin_x < 8) inv_vx = V_INVADER;           // 左端に達したとき
  if(ivmax_x > W_WIN-8) inv_vx = -V_INVADER;    // 右端に達したとき
  
  if((ivmin_x < 8) || (ivmax_x > W_WIN-8)){      // 左右の端に達しとき
    for(i=0; i<NXIV; i++){
      for(j=0; j<NYIV; j++){
        invd_y[i][j] += 6;                       // 下に降りる
      }
    }
  }
}


//====================================================================
// インベーダー軍のビームの状態の処理
//====================================================================
static void state_i_beam()
{
  int i;

  for(i=0; i<N_I_BEAM; i++){
    if(i_beam_status[i] ==2){
      i_beam_y0[i] += i_beam_vy[i];
      i_beam_y1[i] += i_beam_vy[i];
   
      if(i_beam_y1[i] >= H_WIN) i_beam_status[i]=0;

      if((xc-W2_HODAI < i_beam_x[i]) &&
		 (i_beam_x[i] < xc+W2_HODAI) &&
		 (L_HODAI > i_beam_y0[i]) &&
		 (i_beam_y1[i] > H_HODAI)){
		buzzer_div = 32;				// 250Hz
		buzzer_ms = 1000;
//		buzzer(200, 1000);
		winner=INVADER;            // 地球防衛軍の負け
      }
    }
  }
}


//====================================================================
// キーボードイベントの処理
// スペースキーが押されたら地球防衛軍のビームを発射
//====================================================================
static void shoot()
{
  //--- スペースキーが押されて，発射可能なビームがあるとき ----
  if(p_e_beam1 != -1){
	buzzer_div = 8;				// 1KHz
	buzzer_ms = 50;
//	buzzer(1000, 50);
	e_beam_status = 2;       // ビームを発射の状態にする
	e_beam_vy = V_E_BEAM;    // ビームの速度を設定
    p_e_beam1 = -1;                 // 発射可能なビームが無い
  }
}

/********************************************************************************/
/*		MCrnd																	*/
/********************************************************************************/
//static long MCrnd(long i)
//{
//	return genrand_int31() % i;
//}
