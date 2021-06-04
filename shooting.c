#include "app.h"


#define H_WIN 160                      // �E�B���h�E�̕�
#define W_WIN 140                      // �E�B���h�E�̍���

#define W2_HODAI 6                     // �C��̉����̔���
#define H_HODAI 143                    // �C��̏�ʂ�y���W
#define L_HODAI 149                    // �C��̉��ʂ�y���W
#define V_HODAI 5                      // �C��̈ړ����x
#define L_E_BEAM 14                    // �h�q�R�̃r�[���̒���
#define V_E_BEAM 10                    // �h�q�R�̃r�[���̑��x

#define L_I_BEAM 7                     // �C���x�[�_�[�R�̃r�[���̒���
#define V_I_BEAM 2                     // �C���x�[�_�[�R�̃r�[���̑��x
#define P_I_BEAM 200                   // �C���x�[�_�[�R�̃r�[���̏������ˊm��

#define N_I_BEAM 5                     // �C���x�[�_�[�R�̃r�[���̉�ʏ�̍ő吔
#define NXIV     7                     // �C���x�[�_�[�R�̗�̐�
#define NYIV     4                     // �C���x�[�_�[�R�̍s�̐�
#define V_INVADER 1                    // �C���x�[�_�[�R�̑��x

#define NOT_DECIDE 0
#define INVADER 1
#define HUMAN 2

#define CHARA_H			8				// �L�����N�^�̍���
#define CHARA1_W		12				// �L�����N�^�P�̉���
#define CHARA2_W		11				// �L�����N�^�Q�̉���
#define CHARA3_W		8				// �L�����N�^�R�̉���
#define CHARA_DH		14				// ���������̃L�����N�^�Ԋu
#define CHARA_DW		14				// �������̃L�����N�^�Ԋu

//---- �O���[�o���ϐ� -------
static int xc = 100;                               // �}�E�X��x���W
static unsigned char invd_status[NXIV][NYIV];		// 0:���S�A1:����
static int invd_x[NXIV][NYIV];
static int invd_y[NXIV][NYIV];

static int xc_old;
static unsigned char invd_status_old[NXIV][NYIV];
static int invd_x_old[NXIV][NYIV];
static int invd_y_old[NXIV][NYIV];

static int alive_inv=NXIV*NYIV;                    // �����Ă���C���x�[�_�[�̐�
static int inv_vx=V_INVADER;                       // �C���x�[�_�[�̉������̑��x

static char e_beam_status;                 // �n���h�q�R�̃r�[��
static int e_beam_x;
static int e_beam_y0;
static int e_beam_y1;
static int e_beam_vy;

static char e_beam_status_old;                 // �n���h�q�R�̃r�[��
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
// main�֐�
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
			str_cdisp(40, "�V���[�e�B���O�Q�[��");
			str_cdisp(60, "�t�o�@�@�@�F�r�[������");
			str_cdisp(75, "�q���������F�C��ړ��@");
			str_cdisp(90, "�k�������@�F�C��ړ��@");
			str_cdisp(105,"K.Fukumoto");
			sw_updown();
		}
		
		scrn_clear();
		for(;;){
			change_state();					// ��ԕω�����
			if((flag = draw())){			// ��ʕ\��
				wait_ms(2000);
				break;						// �Q�[���I��
			}
		}
	}
}

//====================================================================
// ������
//====================================================================
static void initialize()
{
  int i, j;

	init_genrand(TickCount);

    e_beam_status=0;
    e_beam_y0=H_HODAI-L_E_BEAM;
    e_beam_y1=H_HODAI;
    e_beam_vy=0;

  e_beam_status=1;                       // �C��ɂ̂���
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
      invd_y[i][j] = (NYIV-1-j)*CHARA_DH+CHARA_H/2 + 10;	// 10 �̓��b�Z�[�W�G���A
    }
  }
  winner = NOT_DECIDE;
//  score = 0;
}


//====================================================================
// �}��`��
//====================================================================
static int draw()
{
	if(winner != NOT_DECIDE){
		return draw_result();			// �I��
	}
	
	score_disp();
	draw_hodai();         // �C���`���֐��Ăяo��
	draw_e_beam();        // �n���h�q�R�̃r�[����`���֐��̌Ăяo��
	draw_i_beam();        // �C���x�[�_�[�R�̃r�[����`���֐��̌Ăяo��
	draw_invader();       // �C���x�[�_�[��`���֐��̌Ăяo��
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
// ���҂̕\��
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
// �n���h�q�R�̖C��̕`��
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
// �n���h�q�R�̃r�[���C�̕`��
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
// �C���x�[�_�[�R�̃r�[���̕`��
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
// �C���x�[�_�[�R�̕`��
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
			if(invd_status_old[i][j]==1){	// �����Ă���C���x�[�_�[�̂ݏ���
				xx = invd_x_old[i][j]-CHARA1_W/2;
				yy = invd_y_old[i][j]-CHARA_H/2;
				for(y=yy; y<yy+CHARA_H; y++){
					for(x=xx; x<xx+CHARA1_W; x++){
						pset(x, y, BLACK);
					}
				}
			}
			if(invd_status[i][j]==1){		// �����Ă���C���x�[�_�[�̂ݕ`��
				tmp = (invd_chara & 2)>>1;
				if(j<2){					// ���̂Q�s
					xx = invd_x[i][j]-CHARA1_W/2;
					yy = invd_y[i][j]-CHARA_H/2;
					write_square(xx, yy, CHARA1_W, CHARA_H, (char*)chara1[tmp]);
				}
				else if(j==2){				// ������R�s��
					xx = invd_x[i][j]-CHARA2_W/2;
					yy = invd_y[i][j]-CHARA_H/2;
					write_square(xx, yy, CHARA2_W, CHARA_H, (char*)chara2[tmp]);
				}
				else{						// ��ԏ�
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
// PC���ɂȂƂ��Ɏ��s����D���ꂪ���s�����Ə�Ԃ��ω�����
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
	state_hodai();		// �C��̈ړ�
    state_e_beam();     // �n���h�q�R�̃r�[���̏���
    state_invader();    // �C���x�[�_�[�R�̏���
    state_i_beam();     // �C���x�[�_�[�R�̃r�[���̏���
  }
}


///////////////////////////////////
//	state_hodai
///////////////////////////////////
static void state_hodai()
{
	int c;
	
	c = sw_sense();					// �L�[����
	if(c & UP){
		shoot();					// �r�[������
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
// �n���h�q�R�̃r�[���̏�Ԃ̏���
//====================================================================
static void state_e_beam()
{
  int l,m, tmp1, tmp2, tmp3;
  int st0;
  int nshoot;                // ���ˍς݂̒n���h�q�R�̋ʂ̐�

	st0 = 0;
	nshoot = 0;

    switch(e_beam_status){

    //--------  �i�[�ɂɂ���r�[���̏��� ------------------------
    case 0:
      st0=0;                    // ���ɔ��ˉ\�ȃr�[����ݒ�
      break;

    //--------  �C��ɂ���r�[���̏��� ------------------------
    case 1:
      e_beam_x = xc;         // x�����Ɉړ�
      break;

    //--------  ���łɔ��˂��ꂽ�r�[���̏��� ------------------------
    case 2:
      nshoot++;                         // ���˂���Ă���r�[�����J�E���g
      e_beam_y0 -= e_beam_vy;     // �r�[���̈ړ�
      e_beam_y1 -= e_beam_vy;

      // ------ �C���x�[�_�[�Ƀr�[�����Փ˂������Ƃ��m�F���ď��� ------
      alive_inv = 0;
      for(l=0; l<NXIV; l++){    
		for(m=0; m<NYIV; m++){
		  if(invd_status[l][m]==1){
			tmp2 = CHARA_H/2;				// �L�����N�^�̍����� 1/2
			if(m<2){						// ���̂Q�s
				tmp1 = CHARA1_W/2;			// �L�����N�^�̕��� 1/2
				tmp3 = 10;					// �_��
			}
			else if(m==2){					// ������R�s��
				tmp1 = CHARA2_W/2;
				tmp3 = 20;
			}
			else{							// ��ԏ�
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
		      invd_status[l][m]=0;          // �C���x�[�_�[�̎��S
		      score += tmp3;				// �_���v�Z
		      
		      e_beam_status=0;             // �r�[���͊i�[�ɂ�
		      e_beam_y0=H_HODAI-L_E_BEAM;  // �r�[���̏�����
		      e_beam_y1=H_HODAI;
		    }
		  }
		}      
      }
      if(alive_inv==0)winner=HUMAN;


      // ---- ��ʂ���n���h�q�R�̃r�[�����͂ݏo���ꍇ�̏��� --------
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


  // --- �n���h�q�R�̐V���Ȕ��ˉ\�ȃr�[���̏��� -----
	if(e_beam_status==0){
		e_beam_status=1;
		p_e_beam1 = st0;
	}
}


//====================================================================
// �C���x�[�_�[�R�̏�Ԃ̏���
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
      if(invd_status[i][j]==1){   // �C���x�[�_�[�̐����̃`�F�b�N
		invd_x[i][j] += inv_vx;   // �C���x�[�_�[�̉������ړ�
		// ---- �C���x�[�_�[�R�̃r�[�����˂̏��� ------
		tmp = MCrnd(P_I_BEAM);
		if(can_attack == 1 && tmp == 0){  // ���ˏ���
		  for(k=0; k<N_I_BEAM; k++){
		    if(i_beam_status[k] !=2){      // ���ˉ\�ȃr�[����T��
		      i_beam_status[k] =2;         // �r�[���̔���
		      i_beam_x[k] = invd_x[i][j];
		      i_beam_y0[k] = invd_y[i][j];
		      i_beam_y1[k] = invd_y[i][j]+L_I_BEAM;
		      can_attack=0;
		      break;
		    }
		  }
		}
		// --- �C���x�[�_�[�R�̍��E�㉺�̒[�̍��W -------
		if(invd_x[i][j] < ivmin_x) ivmin_x=invd_x[i][j];   // ���[ 
		if(invd_x[i][j] > ivmax_x) ivmax_x=invd_x[i][j];   // �E�[
		if(invd_y[i][j] < ivmin_y) ivmin_y=invd_y[i][j];   // ���̒[
		if(invd_y[i][j] > ivmax_y) ivmax_y=invd_y[i][j];   // ��̒[
      }
    }
  }
  if(ivmax_y > H_WIN-10)
  	winner=INVADER;            // �n���h�q�R�̕���


  if(ivmin_x < 8) inv_vx = V_INVADER;           // ���[�ɒB�����Ƃ�
  if(ivmax_x > W_WIN-8) inv_vx = -V_INVADER;    // �E�[�ɒB�����Ƃ�
  
  if((ivmin_x < 8) || (ivmax_x > W_WIN-8)){      // ���E�̒[�ɒB���Ƃ�
    for(i=0; i<NXIV; i++){
      for(j=0; j<NYIV; j++){
        invd_y[i][j] += 6;                       // ���ɍ~���
      }
    }
  }
}


//====================================================================
// �C���x�[�_�[�R�̃r�[���̏�Ԃ̏���
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
		winner=INVADER;            // �n���h�q�R�̕���
      }
    }
  }
}


//====================================================================
// �L�[�{�[�h�C�x���g�̏���
// �X�y�[�X�L�[�������ꂽ��n���h�q�R�̃r�[���𔭎�
//====================================================================
static void shoot()
{
  //--- �X�y�[�X�L�[��������āC���ˉ\�ȃr�[��������Ƃ� ----
  if(p_e_beam1 != -1){
	buzzer_div = 8;				// 1KHz
	buzzer_ms = 50;
//	buzzer(1000, 50);
	e_beam_status = 2;       // �r�[���𔭎˂̏�Ԃɂ���
	e_beam_vy = V_E_BEAM;    // �r�[���̑��x��ݒ�
    p_e_beam1 = -1;                 // ���ˉ\�ȃr�[��������
  }
}

/********************************************************************************/
/*		MCrnd																	*/
/********************************************************************************/
//static long MCrnd(long i)
//{
//	return genrand_int31() % i;
//}
