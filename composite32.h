#define X_RES	160 // �������𑜓x
#define Y_RES	150 // AAAAA	100 // �c�����𑜓x
//#define H_WORD	(X_RES/8) // 1�s����̃��[�h��(16bit�P��)
//#define PLANESIZE	(H_WORD*Y_RES) // VRAM1������̃��[�h��(16bit�P��)

extern volatile char drawing;		//�@�\�����Ԓ���-1
extern volatile unsigned short drawcount;		//�@1��ʕ\���I�����Ƃ�1�����B�A�v������0�ɂ���B
							// �Œ�1��͉�ʕ\���������Ƃ̃`�F�b�N�ƁA�A�v���̏���������ʊ��ԕK�v���̊m�F�ɗ��p�B

extern unsigned char VRAM[];
extern void start_composite(void); //�J���[�R���|�W�b�g�o�͊J�n
extern void stop_composite(void); //�J���[�R���|�W�b�g�o�͒�~
extern void init_composite(void); //�J���[�R���|�W�b�g�o�͏�����
extern void scrn_clear(void); //��ʃN���A
extern void set_palette(int n, int r, int g, int b); //�J���[�p���b�g�ݒ�


//	t0=�J���[�f�[�^
//	t1=VRAMp
//	v1=ClTable
//	v0=&LATB
//	a1=�p���b�g�R�[�h
