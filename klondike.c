#include "app.h"


#define X_PIXCEL	14					// �J�[�h�P���̃s�N�Z��
#define Y_PIXCEL	25
#define X_NUMB		11					// �J�[�h����ׂ����̉������̖���
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
static char bafuda1[7][6+1];					// 0x30:�J�[�h��ށA0x0f:�J�[�h�ԍ�(0-12)
static char bafuda2[7][13+1];
static char yamafuda1[52];
static char yamafuda2[52];
static char kumifuda[4][13+1];

static void initialize();
static void draw_card(int x, int y, int c);
static void draw_bafuda();
static void draw_kumifuda();
static void draw_yamafuda();
static void draw();
static void cursor_on(int basyo, int j, int k);
static void select1(int *basyo, int *j, int *k);
static void select1_next(int *basyo, int *j, int *k);
static void next_sub(int *basyo, int *j, int *k);
static void select2(int basyo, int j, int k, int *bx, int *jx, int *kx);
static void select2_next(int basyo, int j, int k, int *bx, int *jx, int *kx);
static int can_move(int basyo, int j, int k, int bx, int jx, int kx);
static void yamafuda_process();
static void draw_message1(const char *msg);
static void draw_message2(const char *msg);
static void draw_message3(const char *msg);
static void move(int basyo, int j, int k, int bx, int jx, int kx);
static void bafuda_mekuri();
static void test();
static void draw_s(char *c, int n);
static int end_process();


/********************************************************************************/
/*		klon_main																*/
/********************************************************************************/
void klon_main()
{
	int basyo, j, k,  bx, jx, kx;
	
	for(;;){
		lcd_clear(MAGENTA_U);
		str_cdisp(50, "�@�N�����_�C�N�@");
		str_cdisp(70, "Down: �I��");
		str_cdisp(85, "A-SW: ����");
		str_cdisp(100,"K.Fukumoto");
		sw_updown();
		lcd_clear(WHITE);
		initialize();
		for(;;){
			draw();
			draw_message1("From    ");
			select1(&basyo, &j, &k);
			if(basyo==1 && j==0){
				yamafuda_process();
				continue;
			}
			draw_message1("To      ");
			select2(basyo, j, k, &bx, &jx, &kx);
			move(basyo, j, k, bx, jx, kx);
			bafuda_mekuri();
			if(end_process()){
				break;
			}
		}
	}
}
/********************************************************************************/
/*		end_process																*/
/*		�R�D�������Ȃ��D���S���\�����ɂȂ����玩�����s����					*/
/********************************************************************************/
static int end_search(int c, int *j, int *k)
{
	for(*j=0; *j<7; (*j)++){
		if(bafuda2[*j][0]>=0){
			for(*k=12; ; (*k)--){
				if(bafuda2[*j][*k]>=0){
					break;
				}
			}
			if(c==bafuda2[*j][*k]){
				return 1;					// �}�b�`����
			}
		}
	}
	return 0;								// �}�b�`����D����
}

static int end_process()
{
	int i, j, k, m, c;
	
	if(yamafuda1[0]>=0 || yamafuda2[0]>=0){
		return 0;								// �R�D���L��
	}
	for(i=0; i<7; i++){
		if(bafuda1[i][0]>=0){
			return 0;							// �\�����̏�D���L��
		}
	}
	//	�������s
	for(;;){
		for(i=0; i<4; i++){
			if(kumifuda[i][0]<0){				// ��̏ꏊ���L��
				for(m=0; m<4; m++){				// �S��ނ� A �𒲂ׂ�
					if(end_search(m<<4, &j, &k)){
						kumifuda[i][0] = m<<4;	// ��������
						bafuda2[j][k] = -1;
						draw();
						wait_ms(200);
						goto next1;
					}
				}
			}
			for(m=12; ; m--){
				if(kumifuda[i][m] >= 0){
					break;
				}
			}
			c = kumifuda[i][m] + 1;
			if(end_search(c, &j, &k)){
				kumifuda[i][m+1] = c;
				bafuda2[j][k] = -1;
				draw();
				wait_ms(200);
			}
next1:;
		}
		for(i=0; i<4; i++){
			if(kumifuda[i][13-1] < 0){
				break;
			}
		}
		if(i==4){							// �I������
			str_cdisp(70,"���߂łƂ�������܂�");
			sw_updown();
			return 1;						// �I��
		}
	}
}
/********************************************************************************/
/*		test																	*/
/********************************************************************************/
static void test_c(char c)
{
	int card = c>>4;
	int numb = c&0x0f;
	char msg[20];
	
	if(c== -1){
		return;
	}
	if(card<0 || card>=4 || numb>=13){
		draw_message2("card error");
		sprintf(msg, "card=%02x", c);
		draw_message3(msg);
		for(;;);
	}
}

static void test_s(char *fuda, int n)
{
	int i;
	
	for(i=0; i<n; i++){
		if(fuda[i]<0)
			break;
	}
	for( ; i<n; i++){
		if(fuda[i]>=0){
			draw_message1("test_s error");
			draw_s(fuda, n<=14 ? n : 14);
			for(;;);
		}
	}
}

static void test()
{
	int i, j;
	
	for(i=0; i<7; i++){
		for(j=0; j<6+1; j++){
			test_c(bafuda1[i][j]);
		}
		for(j=0; j<13+1; j++){
			test_c(bafuda2[i][j]);
		}
	}
	for(i=0; i<52; i++){
		test_c(yamafuda1[i]);
		test_c(yamafuda2[i]);
	}
	for(i=0; i<4; i++){
		for(j=0; j<13+1; j++){
			test_c(kumifuda[i][j]);
		}
	}
	
	for(i=0; i<7; i++){
		test_s(bafuda1[i], 6+1);
		test_s(bafuda2[i], 13+1);
	}
	test_s(yamafuda1, 52);
	test_s(yamafuda2, 52);
	for(i=0; i<4; i++){
		test_s(kumifuda[i], 13+1);
	}
}
/********************************************************************************/
/*		bafuda_mekuri															*/
/********************************************************************************/
static void bafuda_mekuri()
{
	int i, j;
	
	for(i=0; i<7; i++){
		if(bafuda2[i][0] < 0){
			for(j=0; j<6+1; j++){
				if(bafuda1[i][j]<0){
					if(j==0){
						break;			// ��D����
					}
					draw();
					bafuda2[i][0] = bafuda1[i][j-1];
					bafuda1[i][j-1] = -1;
					
					wait_ms(500);
					return;
				}
			}
		}
	}
}
/********************************************************************************/
/*		yamafuda_process														*/
/*		�R�D������																*/
/********************************************************************************/
static void yamafuda_process()
{
	int i, c;
	
	c = yamafuda1[0];
	memmove(&yamafuda1[0], &yamafuda1[1], 52-1);
	if(c >= 0){
		for(i=0; i<52; i++){
			if(yamafuda2[i] < 0){
				break;
			}
		}
		yamafuda2[i] = c;
		return;
	}
	memcpy(yamafuda1, yamafuda2, sizeof(yamafuda1));
	memset(yamafuda2, -1, sizeof(yamafuda2));
}
/********************************************************************************/
/*		select2																	*/
/*		basyo  0:kumifuda  j:0�`3												*/
/*		       1:yamafuda  j:0�`1												*/
/*		       2:bafuda    j:0�`6  k:0�`12										*/
/*		to�i�Q��ځj�̑I��														*/
/********************************************************************************/
static void select2(int basyo, int j, int k, int *bx, int *jx, int *kx)
{
	int sw;
	
	*bx = 1;
	*jx = *kx = 0;
	select2_next(basyo, j, k, bx, jx, kx);	// �������鎟�̏ꏊ�Ɉړ�����
	for(;;){
		cursor_on(basyo, j, k);
		cursor_on(*bx, *jx, *kx);
		sw = sw_updown();
		draw();								// �J�[�\�� off
		if(sw & (A_SW | B_SW)){
			return;
		}
		if(sw & DOWN){
			select2_next(basyo, j, k, bx, jx, kx);
		}
	}
}
/********************************************************************************/
/*		select2_next															*/
/*		basyo  0:kumifuda  j:0�`3												*/
/*		       1:yamafuda  j:0�`1												*/
/*		       2:bafuda    j:0�`6  k:0�`12										*/
/*		�������鎟�̏ꏊ�Ɉړ�����												*/
/********************************************************************************/
static void select2_next(int basyo, int j, int k, int *bx, int *jx, int *kx)
{
	for(;;){
		next_sub(bx, jx, kx);				// ���̏ꏊ�Ɉړ��i�������邩�ǂ����͌��Ȃ��j
		if(can_move(basyo, j, k, *bx, *jx, *kx)){
			return;							// ���������Ƃ��o����
		}
	}
}
/********************************************************************************/
/*		select1																	*/
/*		*basyo  0:kumifuda  *j:0�`3												*/
/*		        1:yamafuda  *j:0�`1												*/
/*				2:bafuda    *j:0�`6  *k:0�`12									*/
/*		from�i�P��ځj�̑I��													*/
/********************************************************************************/
static void select1(int *basyo, int *j, int *k)
{
	int sw;
	
	sw_updown();
	*basyo = 1;
	*j = *k = 0;
	for(;;){
		cursor_on(*basyo, *j, *k);
		sw = sw_updown();
		draw();								// �J�[�\�� off
		if(sw & (A_SW | B_SW)){
			return;
		}
		if(sw & DOWN){
			select1_next(basyo, j, k);
		}
	}
}
/********************************************************************************/
/*		select1_next															*/
/*		*basyo  0:kumifuda  *j:0�`3												*/
/*		        1:yamafuda  *j:0�`1												*/
/*				2:bafuda    *j:0�`6  *k:0�`12									*/
/*		�������鎟�̏ꏊ�Ɉړ�����												*/
/********************************************************************************/
static void select1_next(int *basyo, int *j, int *k)
{
	int bx, jx, kx;
	
	bx = jx = kx = 0;
	for(;;){
		next_sub(basyo, j, k);				// ���̏ꏊ�Ɉړ��i�������邩�ǂ����͌��Ȃ��j
		
		for(bx=0; bx<3; bx++){
			if(bx==0){
				for(jx=0; jx<4; jx++){
					if(can_move(*basyo, *j, *k, bx, jx, kx)){
						return;				// ���������Ƃ��o����
					}
				}
			}
			else if(bx==1){
				for(jx=0; jx<2; jx++){
					if(can_move(*basyo, *j, *k, bx, jx, kx)){
						return;				// ���������Ƃ��o����
					}
				}
			}
			else if(bx==2){
				for(jx=0; jx<7; jx++){
					for(kx=0; kx<13; kx++){
						if(can_move(*basyo, *j, *k, bx, jx, kx)){
							return;			// ���������Ƃ��o����
						}
					}
				}
			}
		}
	}
}
/********************************************************************************/
/*		can_move																*/
/*		basyo  0:kumifuda  j:0�`3												*/
/*		       1:yamafuda  j:0�`1												*/
/*		       2:bafuda    j:0�`6  *k:0�`12										*/
/*		bx, jx, kx																*/
/*		return 1:��������														*/
/********************************************************************************/
static int can_move(int basyo, int j, int k, int bx, int jx, int kx)
{
	int i, c, c2;
	
	if(basyo==bx && j==jx && k==kx){
		return 0;							// �����ꏊ�ɂ͓������Ȃ�
	}
	if(basyo==0){							// �g�D 4
		c = -1;
		for(i=0; i<13; i++){
			if(kumifuda[j][i] < 0){
				break;
			}
			c = kumifuda[j][i];
		}
next1:;
		if(bx != 2){						// �ړ���͏�D�̂�
			return 0;						// �������Ȃ�
		}
		if(bafuda2[jx][kx]<0 && kx==0 && (c&0x0f)==(13-1)){
			return 1;						// King �͋�̈ʒu�Ɉړ��ł���
		}
		if(bafuda2[jx][kx]<0 || bafuda2[jx][kx+1]>=0){
			return 0;						// �����ɂ����ړ��ł��Ȃ�
		}
		c2 = bafuda2[jx][kx];
		if(((c2&0x0f)-1)==(c&0x0f)){		// �����ԍ�
			if((c>>4)<2 && (c2>>4)>=2){		// �F�Ⴂ
				return 1;					// �ړ��o����
			}
			if((c>>4)>=2 && (c2>>4)<2){		// �F�Ⴂ
				return 1;					// �ړ��o����
			}
			return 0;						// �ړ��o���Ȃ��i���F�j
		}
		return 0;							// �ړ��o���Ȃ�
	}
	else if(basyo==1){						// �R�D 2
		if(j==0){
			return 1;						// �ړ��o����
		}
		c = -1;
		for(i=0; i<52; i++){
			if(yamafuda2[i]<0){
				break;
			}
			c = yamafuda2[i];
		}
		if(c < 0){
			return 0;						// �D���Ȃ���Έړ��o���Ȃ�
		}
		if(bx==0){							// �g�D�ւ̈ړ�
next2:;
			if((c&0x0f)==(1-1) && kumifuda[jx][0]<0){
				return 1;					// A �͉����Ȃ��ꏊ�Ɉړ��ł���
			}
			c2 = -1;
			for(i=0; i<13; i++){
				if(kumifuda[jx][i]<0){
					break;
				}
				c2 = kumifuda[jx][i];
			}
			if(((c2&0x0f)+1)==(c&0x0f) && ((c2>>4)==(c>>4))){
				return 1;					// �����ԍ��œ���ށ@�ړ��o����
			}
			return 0;						// �ړ��o���Ȃ�
		}
		goto next1;							// ��D�ւ̈ړ�
	}
	else if(basyo==2){						// ��D 7
		if(bafuda2[j][k] < 0){				// �D����
			return 0;						// �ړ��o���Ȃ�
		}
		if(bx==0){							// �g�D�ւ̈ړ�
			if(bafuda2[j][k+1] >= 0){
				return 0;					// ���������͑g�D�ֈړ��o���Ȃ�
			}
			c = bafuda2[j][k];
			goto next2;						// �g�D�ւ̈ړ�
		}
		c = bafuda2[j][k];
		goto next1;							// ��D�ւ̈ړ�
	}
    return 0;                               // �ړ��o���Ȃ�
}
/********************************************************************************/
/*		move																	*/
/*		*basyo  0:kumifuda  *j:0�`3												*/
/*		        1:yamafuda  *j:0�`1												*/
/*				2:bafuda    *j:0�`6  *k:0�`12									*/
/********************************************************************************/
static void move(int basyo, int j, int k, int bx, int jx, int kx)
{
	char c[14];
	int i;


//sprintf(c, "%d %d %d", basyo, j, k);//AAAAA
//draw_message2(c);
//sprintf(c, "%d %d %d", bx, jx, kx);
//draw_message3(c);


	memset(c, -1, sizeof(c));
	if(basyo==0){
		for(i=12; ; i--){
			if(kumifuda[j][i]>=0){
				break;
			}
		}
		c[0] = kumifuda[j][i];
		kumifuda[j][i] = -1;
	}
	else if(basyo==1){
		if(j==0){
			draw_message2("error1");
			for(;;);
		}
		for(i=0; yamafuda2[i]>=0; i++){
		}
		if(i==0){
			draw_message2("error4");		// �������R�D����
			for(;;);
		}
		c[0] = yamafuda2[i-1];
		yamafuda2[i-1] = -1;
	}
	else if(basyo==2){
		for(i=0; bafuda2[j][i+k]>=0; i++){
			c[i] = bafuda2[j][i+k];
			bafuda2[j][i+k] = -1;
		}
	}
	
	if(bx==0){
		if(c[1]>=0){
			draw_message2("error2");
			for(;;);
		}
		for(i=0; kumifuda[jx][i]>=0; i++){
		}
		kumifuda[jx][i] = c[0];
	}
	else if(bx==1){
		if(c[0]>=0){
			draw_message2("error3");
			for(;;);
		}
	}
	else if(bx==2){
		if(bafuda2[jx][kx]>=0){
			for(i=0; c[i]>=0; i++){
				bafuda2[jx][kx+i+1] = c[i];
			}
		}
		else{
			for(i=0; c[i]>=0; i++){
				bafuda2[jx][kx+i] = c[i];
			}
		}
	}
}
/********************************************************************************/
/*		next_sub																*/
/*		*basyo  0:kumifuda  *j:0�`3												*/
/*		        1:yamafuda  *j:0�`1												*/
/*				2:bafuda    *j:0�`6  *k:0�`12									*/
/*		���̏ꏊ�Ɉړ��i�������邩�ǂ����͌��Ȃ��j								*/
/********************************************************************************/
static void next_sub(int *basyo, int *j, int *k)
{
	if(*basyo==0){
		if(*j<3){
			(*j)++;
			return;
		}
		*j = 0;
		*basyo = 1;
		return;
	}
	else if(*basyo==1){
		if(*j==0){
			(*j)++;
			return;
		}
		*j = 0;
		*basyo = 2;
		*k = 0;
		return;
	}
	else if(*basyo==2){
		if(*k==12){
			*k = 0;
			if(*j==6){
				*j = 0;
				*basyo = 0;
				return;
			}
			(*j)++;
			return;
		}
		(*k)++;
		return;
	}
}
/********************************************************************************/
/*		cursor_on																*/
/********************************************************************************/
static void cursor_on(int basyo, int j, int k)
{
	int i;
	
	if(basyo==0){							// �g�D 4
		lcd_clearA(j*X_PIXCEL+(160-14*4), 0, (j+1)*X_PIXCEL-1+(160-14*4), 2, BRUE);
	}
	else if(basyo==1){						// �R�D 2
		lcd_clearA(160+X_PIXCEL*(j-3), 30, X_PIXCEL-1+(160+X_PIXCEL*(j-3)), 32, BRUE);
	}
	else if(basyo==2){						// ��D 7
		for(i=0; i<6; i++){
			if(bafuda1[j][i] < 0)
				break;
		}
		lcd_clearA(j*X_PIXCEL, i*3+k*9, (j+1)*X_PIXCEL-1, i*3+k*9+2, BRUE);
	}
}
/********************************************************************************/
/*		draw																	*/
/********************************************************************************/
static void draw()
{
	draw_bafuda();
	draw_yamafuda();
	draw_kumifuda();
	
	test();
}
/********************************************************************************/
/*		draw_message1															*/
/********************************************************************************/
static void draw_message1(const char *msg)
{
	back_color = WHITE;
	text_color = BLACK;
	str_disp(100,60, msg);
}

static void draw_message2(const char *msg)
{
	back_color = WHITE;
	text_color = BLACK;
	str_disp(100,75, msg);
}

static void draw_message3(const char *msg)
{
	back_color = WHITE;
	text_color = BLACK;
	str_disp(100,90, msg);
}
/********************************************************************************/
/*		draw_s																	*/
/********************************************************************************/
static void draw_s(char *c, int n)
{
	int i, j, k;
	char buf[20];
	
	for(k=0; k<n; k++){
		i = k/3;
		j = k%3;
		if(j==0){
			buf[0] = '\0';
		}
		sprintf(&buf[strlen(buf)], "%02X ", c[k]&0xff);
		if(j==2 || k==n-1){
			str_disp(100, 70+10*i, buf);
		}
	}
}
/********************************************************************************/
/*		draw_yamafuda															*/
/********************************************************************************/
static void draw_yamafuda()
{
	int j, c;
	
	if(yamafuda1[0] < 0){
		lcd_clearA((160-14*3), 30, X_PIXCEL-1+(160-14*3), 30+Y_PIXCEL-1, WHITE);
	}
	else{
		write_square((160-14*3), 30, X_PIXCEL, Y_PIXCEL, card_ura);
	}
	
	c = -1;
	for(j=0; j<52; j++){
		if(yamafuda2[j] < 0){
			break;
		}
		c = yamafuda2[j];
	}
	if(c < 0){
		lcd_clearA((160-14*2), 30, X_PIXCEL-1+(160-14*2), 30+Y_PIXCEL-1, WHITE);
	}
	else{
		draw_card((160-14*2), 30, c);
	}
}
/********************************************************************************/
/*		draw_kumifuda															*/
/********************************************************************************/
static void draw_kumifuda()
{
	int i, j, c;
	
	for(i=0; i<4; i++){
		c = -1;
		for(j=0; j<13; j++){
			if(kumifuda[i][j] < 0){
				break;
			}
			c = kumifuda[i][j];
		}
		if(c < 0){
			lcd_clearA(i*X_PIXCEL+(160-14*4), 0, (i+1)*X_PIXCEL-1+(160-14*4), Y_PIXCEL-1, WHITE);
		}
		else{
			draw_card(i*X_PIXCEL+(160-14*4), 0, c);
		}
	}
}
/********************************************************************************/
/*		draw_bafuda																*/
/********************************************************************************/
static void draw_bafuda()
{
	int i, j, k;
	
	for(i=0; i<7; i++){
		lcd_clearA(i*X_PIXCEL, 0, (i+1)*X_PIXCEL-1, 150-1, WHITE);
		for(j=0; j<6; j++){
			if(bafuda1[i][j] < 0){
				break;
			}
			else{
				write_square(i*X_PIXCEL, j*3, X_PIXCEL, Y_PIXCEL, card_ura);
			}
		}
		for(k=0; k<13; k++){
			if(bafuda2[i][k] < 0){
				break;
			}
			else{
				draw_card(i*X_PIXCEL, j*3+k*9, bafuda2[i][k]);
			}
		}
	}
}
/********************************************************************************/
/*		initialize																*/
/********************************************************************************/
static void initialize()
{
	int i, j, cnt, rnd, tmp;
	char buf[52];
	
	memset(buf, -1, sizeof(buf));
    cnt = 0;
	for(i=0; i<4; i++){
		for(j=0; j<13; j++){
			buf[cnt++] = i*0x10 + j;
		}
	}
	for(i=0; i<10; i++){
		for(j=0; j<52; j++){
			rnd = MCrnd(52);
			tmp = buf[j];
			buf[j] = buf[rnd];
			buf[rnd] = tmp;
		}
	}
	cnt = 0;
	//	��D�Z�b�g
	memset(bafuda1, -1, sizeof(bafuda1));
	memset(bafuda2, -1, sizeof(bafuda2));
	for(i=0; i<7; i++){
		for(j=0; j<i; j++){
			bafuda1[i][j] = buf[cnt++];
		}
	}
	for(i=0; i<7; i++){
		bafuda2[i][0] = buf[cnt++];
	}
	//	�g�D�Z�b�g
	memset(kumifuda, -1, sizeof(kumifuda));
	//	�R�D�Z�b�g
	memset(yamafuda1, -1, sizeof(yamafuda1));
	memset(yamafuda2, -1, sizeof(yamafuda2));
	for(i=0; cnt<52; i++){
		yamafuda1[i] = buf[cnt++];
	}
}
/********************************************************************************/
/*		draw_card																*/
/*		card 0:�n�[�g�A1:�_�C���A2:�X�y�[�h�A3:�N���u							*/
/*		numb  : 0�`(13-1)														*/
/********************************************************************************/
static void draw_card(int x, int y, int c)
{
    int card = (c>>4)&0x03;
    int numb = c&0x0f;

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
/********************************************************************************/
/*		write_square_w_to_y														*/
/********************************************************************************/
#if 0
static void write_square_w_to_y(int x, int y, int w, int h, const char *c)
{
	int xx, yy, color;
	
	for(yy=y; yy<y+h; yy++){
		for(xx=x; xx<x+w; xx++){
			color = *c++;
			if(color==WHITE)
				color = YELLOW_D;
			pset(xx, yy, color);
		}
	}
}
#endif
