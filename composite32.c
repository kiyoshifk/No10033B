// �R���|�W�b�g�J���[�M���o�̓v���O���� PIC32MX120F032B�p�@by K.Tanaka Rev.2
// �o�� PORTB
// RGB�e�F��VRAM�`���i16bit�P�ʂō��[����ʁj
//�@�J���[�M����120�x���Ƃɏo��
// �J���[�p���b�g�Ή��iRev.2�j

// �o�͂�74HC04���̃C���o�[�^�[���o�b�t�@����ɐڑ�����ꍇ�́A
// �ȉ���"#define INVERTER"��L���ɂ���

//#define INVERTER

//#include "xc.h"
//#include <sys/attribs.h>
//#include "definitions.h"                // SYS function prototypes
//#include "composite32.h"
//#include <plib.h>

#include "app.h"


//�J���[�M���o�̓f�[�^
//
#ifndef INVERTER

// 5bit DA�A�X�g���[�g�o�͂̏ꍇ�̐M���萔�f�[�^
#define C_SYN	0
#define C_BLK	6
#define C_BST1	6
#define C_BST2	3
#define C_BST3	8
//�@�X�g���[�g�o�͗p�f�[�^�����܂�

#else
// 5bit DA�A�C���o�[�^�[�o�͂̏ꍇ�������L���ɂ���
#define C_SYN	31
#define C_BLK	25
#define C_BST1	25
#define C_BST2	28
#define C_BST3	23
//�@�C���o�[�^�[�o�͗p�f�[�^�����܂�
#endif

// �p���X���萔
#define V_NTSC		262					// 262�{/���
#define V_SYNC		10					// ���������{��
#define V_PREEQ		53 //AAAAA	26					// �u�����L���O��ԏ㑤�iV_SYNC�{V_PREEQ�͋����Ƃ��邱�Ɓj
#define V_LINE		Y_RES // AAAAA	(Y_RES*2)			// �摜�`���ԁA�c�𑜓x��2�{�̖{��
#define H_NTSC		3405				// ��63.5��sec
#define H_SYNC		252					// �����������A��4.7��sec
#define H_CBST		285					// �J���[�o�[�X�g�J�n�ʒu�i�������������肩��j
#define H_BACK		363					// ���X�y�[�X�i�������������オ�肩��j

// �O���[�o���ϐ���`
unsigned char VRAM[X_RES*Y_RES] __attribute__ ((aligned (4)));
unsigned char *VRAMp; //VRAM�Ə�����VRAM�A�h���X
volatile unsigned short LineCount;	// �������̍s
volatile unsigned short drawcount;	//�@1��ʕ\���I�����Ƃ�1�����B�A�v������0�ɂ���B
							// �Œ�1��͉�ʕ\���������Ƃ̃`�F�b�N�ƁA�A�v���̏���������ʊ��ԕK�v���̊m�F�ɗ��p�B
volatile char drawing;		//�@�f����ԏ�������-1�A���̑���0
volatile unsigned int TickCount;
int t2_div;
int buzzer_div;              // 8:1KHz, 9:889Hz, 10:800Hz, 11:727Hz, 12;667Hz, 13:615Hz, 14:615Hz, 15:533Hz, 16:500Hz
volatile int buzzer_ms;

//�J���[�M���e�[�u��
//�e�F32bit���ŉ��ʂ���8bit�����ɏo��
//Rev.2����RAM�ɔz�u���J���[�p���b�g��
unsigned int ClTable[256];

//int data1,data2,data3,data4;//AAAAA

/**********************
*  Timer2 ���荞�ݏ����֐�
*   63.5usec����
***********************/
void __ISR(_TIMER_2_VECTOR, IPL5AUTO) T2Handler(void)
{
	asm volatile("#":::"a0");				// �j�󃌃W�X�^�̐錾
	asm volatile("#":::"v0");

	//TMR2�̒l�Ń^�C�~���O�̂����␳
	asm volatile("la	$v0,%0"::"i"(&TMR2));
	asm volatile("lhu	$a0,0($v0)");
	asm volatile("addiu	$a0,$a0,-22-9");

//asm volatile("la	$v0,%0"::"i"(&data1));
//asm volatile("sw	$a0,0($v0)");

	asm volatile("bltz	$a0,label1_2");
	asm volatile("addiu	$v0,$a0,-10");
	asm volatile("bgtz	$v0,label1_2");
	asm volatile("sll	$a0,$a0,2");
	asm volatile("la	$v0,label1");
	asm volatile("addu	$a0,$v0");
	asm volatile("jr	$a0");
asm volatile("label1:");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");

asm volatile("label1_2:");
	//LATB=C_SYN;
	asm volatile("addiu	$a0,$zero,%0"::"n"(C_SYN));
	asm volatile("la	$v0,%0"::"i"(&LATB));
	asm volatile("sb	$a0,0($v0)");// �����M������������B��������ɑS�Ă̐M���o�͂̃^�C�~���O�𒲐�����

	if(LineCount<V_SYNC){
		// ������������
		OC3R = H_NTSC-H_SYNC-3;	// �؂荞�݃p���X���ݒ�
		OC3CON = 0x8001;
	}
	else{
		OC1R = H_SYNC-3;		// �����p���X��4.7usec
		OC1CON = 0x8001;		// �^�C�}2�I�������V���b�g
		if(LineCount>=V_SYNC+V_PREEQ && LineCount<V_SYNC+V_PREEQ+V_LINE){
			// �摜�`����
			OC2R = H_SYNC+H_BACK-3-31;// �摜�M���J�n�̃^�C�~���O
			OC2CON = 0x8001;		// �^�C�}2�I�������V���b�g
		}
	}
    ++t2_div;
    if((t2_div & 0xf)==0){          // �� 1ms (1.016ms)
        TickCount++;
        if(buzzer_ms){
            buzzer_ms--;
        }
    }
    if(buzzer_ms){
        if((t2_div % buzzer_div)==0)
            LATBINV = 0x0100;
    }
    
	LineCount++;
	if(LineCount>=V_NTSC) LineCount=0;
	IFS0bits.T2IF = 0;			// T2���荞�݃t���O�N���A
}

/*********************
*  OC3���荞�ݏ����֐� ���������؂荞�݃p���X
*********************/
void __ISR(_OUTPUT_COMPARE_3_VECTOR, IPL5AUTO) OC3Handler(void)
{
	asm volatile("#":::"v0");
	asm volatile("#":::"v1");
	asm volatile("#":::"a0");

	//TMR2�̒l�Ń^�C�~���O�̂����␳
	asm volatile("la	$v0,%0"::"i"(&TMR2));
	asm volatile("lhu	$a0,0($v0)");
//	asm volatile("addiu	$a0,$a0,-3153-22");		// original line
	asm volatile("addiu	$a0,$a0,-3153-22-3");

//asm volatile("la	$v0,%0"::"i"(&data2));
//asm volatile("sw	$a0,0($v0)");

	asm volatile("bltz	$a0,label4_2");
	asm volatile("addiu	$v0,$a0,-10");
	asm volatile("bgtz	$v0,label4_2");
	asm volatile("sll	$a0,$a0,2");
	asm volatile("la	$v0,label4");
	asm volatile("addu	$a0,$v0");
	asm volatile("jr	$a0");

asm volatile("label4:");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");

	// �����M���̃��Z�b�g
	//	LATB=C_BLK;
asm volatile("label4_2:");
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BLK));
	asm volatile("la	$v0,%0"::"i"(&LATB));
	asm volatile("sb	$v1,0($v0)");	// �����M�����Z�b�g�B�����M�����������肩��3153�T�C�N��

	IFS0bits.OC3IF = 0;			// OC3���荞�݃t���O�N���A
}

/*********************
*  OC1���荞�ݏ����֐� �������������オ��`�J���[�o�[�X�g
*********************/
void __ISR(_OUTPUT_COMPARE_1_VECTOR, IPL5AUTO) OC1Handler(void)
{
	asm volatile("#":::"v0");
	asm volatile("#":::"v1");
	asm volatile("#":::"a0");

	//TMR2�̒l�Ń^�C�~���O�̂����␳
	asm volatile("la	$v0,%0"::"i"(&TMR2));
	asm volatile("lhu	$a0,0($v0)");
//	asm volatile("addiu	$a0,$a0,-252-22");		// original line
	asm volatile("addiu	$a0,$a0,-252-22-3");

//asm volatile("la	$v0,%0"::"i"(&data3));
//asm volatile("sw	$a0,0($v0)");

	asm volatile("bltz	$a0,label2_2");
	asm volatile("addiu	$v0,$a0,-10");
	asm volatile("bgtz	$v0,label2_2");
	asm volatile("sll	$a0,$a0,2");
	asm volatile("la	$v0,label2");
	asm volatile("addu	$a0,$v0");
	asm volatile("jr	$a0");

asm volatile("label2:");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");
asm volatile("label2_2:");

	// �����M���̃��Z�b�g
	//	LATB=C_BLK;
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BLK));
	asm volatile("la	$v0,%0"::"i"(&LATB));
	asm volatile("sb	$v1,0($v0)");	// �����M�����Z�b�g�B�����������������肩��252�T�C�N��

	// 28�N���b�N�E�F�C�g
	asm volatile("addiu	$a0,$zero,9");
asm volatile("loop2:");
	asm volatile("addiu	$a0,$a0,-1");
	asm volatile("nop");
	asm volatile("bnez	$a0,loop2");

	// �J���[�o�[�X�g�M�� 9�����o��
	asm volatile("addiu	$a0,$zero,9");
	asm volatile("la	$v0,%0"::"i"(&LATB));
asm volatile("loop3:");
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BST1));
	asm volatile("sb	$v1,0($v0)");	// �J���[�o�[�X�g�J�n�B�����������������肩��285�T�C�N��
	asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BST2));
	asm volatile("sb	$v1,0($v0)");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BST3));
	asm volatile("sb	$v1,0($v0)");
	asm volatile("addiu	$a0,$a0,-1");//���[�v�J�E���^
	asm volatile("nop");
	asm volatile("bnez	$a0,loop3");

	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BLK));
	asm volatile("sb	$v1,0($v0)");

	IFS0bits.OC1IF = 0;			// OC1���荞�݃t���O�N���A
}


/***********************
*  OC2���荞�ݏ����֐��@�f���M���o��
***********************/
void __ISR(_OUTPUT_COMPARE_2_VECTOR, IPL5AUTO) OC2Handler(void)
{
// �f���M���o��
	//�C�����C���A�Z���u���ł̔j�󃌃W�X�^��錾�i�X�^�b�N�ޔ������邽�߁j
	asm volatile("#":::"v0");
	asm	volatile("#":::"v1");
	asm volatile("#":::"a0");
	asm volatile("#":::"a1");
	asm volatile("#":::"a2");
	asm volatile("#":::"a3");
	asm volatile("#":::"t0");
	asm volatile("#":::"t1");
	asm volatile("#":::"t2");
	asm volatile("#":::"t3");
	asm volatile("#":::"t4");
	asm volatile("#":::"t5");
	asm volatile("#":::"t6");

	//TMR2�̒l�Ń^�C�~���O�̂����␳
	asm volatile("la	$v0,%0"::"i"(&TMR2));
	asm volatile("lhu	$a0,0($v0)");
//	asm volatile("addiu	$a0,$a0,-615-22+21");		// original line
	asm volatile("addiu	$a0,$a0,-615-22+21-8");

//asm volatile("la	$v0,%0"::"i"(&data4));
//asm volatile("sw	$a0,0($v0)");

	asm volatile("bltz	$a0,label3_2");			// >=0 �ł��鎖
	asm volatile("addiu	$v0,$a0,-10");
	asm volatile("bgtz	$v0,label3_2");			// <0  �ł��鎖
	asm volatile("sll	$a0,$a0,2");
	asm volatile("la	$v0,label3");
	asm volatile("addu	$a0,$v0");
	asm volatile("jr	$a0");

asm volatile("label3:");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");

asm volatile("label3_2:");
	//	drawing=-1;
	asm volatile("addiu	$t1,$zero,-1");
	asm volatile("la	$v0,%0"::"i"(&drawing));
	asm volatile("sb	$t1,0($v0)");
	//	t1=VRAMp;
	asm volatile("la	$v0,%0"::"i"(&VRAMp));
	asm volatile("lw	$t1,0($v0)");
	//	v1=ClTable;
	asm volatile("la	$v1,%0"::"i"(ClTable));
	//	v0=&LATB;
	asm volatile("la	$v0,%0"::"i"(&LATB));
	//	t2=0;
	asm volatile("addiu	$t2,$zero,0");
	//	t6=���[�v�J�E���^
	asm volatile("addiu	$t6,$zero,160"); //���[�v�J�E���^


	asm volatile("lbu	$a1,0($t1)");	// VRAM �f�[�^
	asm volatile("sll	$a1,$a1,2");	// x4
	asm volatile("addiu	$t1,$t1,1"); //VRAMp++;
	asm volatile("addu	$a0,$a1,$v1");
	asm volatile("lw	$t0,0($a0)");						// *****
	asm volatile("nop");//�Ȃ��Ă�1�N���b�N�}�������

//    asm volatile("nop");
//    asm volatile("nop");
//    asm volatile("nop");
//    asm volatile("nop");
//    asm volatile("nop");
asm volatile("loop1:");
	asm volatile("sb	$t0,0($v0)");	// �p���b�g�f�[�^���� 8bit �o��
	asm volatile("srl	$t0,$t0,8");

	asm volatile("lbu	$a1,0($t1)");	// VRAM �f�[�^
	asm volatile("sll	$a1,$a1,2");	// x4  �p���b�g�I�t�Z�b�g
	asm volatile("addiu	$t1,$t1,1");	// VRAM �A�h���X�C���N�������g

	asm volatile("sb	$t0,0($v0)");	// �p���b�g�f�[�^���� 8bit �o��
	asm volatile("srl	$t0,$t0,8");

	asm volatile("addu	$a0,$a1,$v1");	// �p���b�g�f�[�^�A�h���X
	asm volatile("nop");
	asm volatile("addiu	$t6,$t6,-1");//���[�v�J�E���^

	asm volatile("sb	$t0,0($v0)");	// �p���b�g�f�[�^��� 8bit �o��

	asm volatile("lw	$t0,0($a0)");	// �p���b�g�f�[�^�ǂݎ��

	asm volatile("bnez	$t6,loop1");
	asm volatile("nop");

	//	LATB=C_BLK;
	asm volatile("addiu	$t0,$zero,%0"::"n"(C_BLK));
	asm volatile("sb	$t0,0($v0)");

	VRAMp += X_RES;
	if(LineCount==V_SYNC+V_PREEQ+V_LINE){ // 1��ʍŌ�̕`��I��
			drawing=0;
			drawcount++;
			VRAMp=VRAM;
	}
	IFS0bits.OC2IF = 0;			// OC2���荞�݃t���O�N���A
}

// ��ʃN���A
void scrn_clear(void)
{
	memset(VRAM, 0, sizeof(VRAM));
}

//void set_palette(int n, int r, int g, int b)
//{
//	// �J���[�p���b�g�ݒ�i5�r�b�gDA�A�d��3.3V�A120�x�P�ʁj
//	// n:�p���b�g�ԍ��Ar,g,b:0�`255
//	int y,s0,s1,s2;
//
//	y=(150*g+29*b+77*r+128)/256;
//	s0=(3525*y+3093*((int)r-y)+1440*256+32768)/65536;
//	s1=(3525*y+1503*((int)b-y)-1547*((int)r-y)+1440*256+32768)/65536;
//	s2=(3525*y-1503*((int)b-y)-1547*((int)r-y)+1440*256+32768)/65536;
//    
//#ifndef INVERTER
//	ClTable[n]=s0+(s1<<8)+(s2<<16);
//#else
//	ClTable[n]=(s0+(s1<<8)+(s2<<16))^0x1f1f1f;
//#endif
//}

void set_palette(int n, int r, int g, int b)
{
	// �J���[�p���b�g�ݒ�i5�r�b�gDA�A�d��3.3V�A120�x�P�ʁj
	// n:�p���b�g�ԍ��Ar,g,b:0�`255
	int y,s0,s1,s2;

//	1:3093

	y=(150*g+29*b+77*r+128)/256;
	s0=(3525*y- 801*((int)b-y)+2988*((int)r-y)+1440*256+32768)/65536;
	s1=(3525*y+2988*((int)b-y)- 801*((int)r-y)+1440*256+32768)/65536;
	s2=(3525*y-2187*((int)b-y)-2187*((int)r-y)+1440*256+32768)/65536;
	
	if(s0>=0x1f) s0 = 0x1f;
	if(s0<0)     s0 = 0;
	if(s1>=0x1f) s1 = 0x1f;
	if(s1<0)     s1 = 0;
	if(s2>=0x1f) s2 = 0x1f;
	if(s2<0)     s2 = 0;
    
#ifndef INVERTER
	ClTable[n]=s0+(s1<<8)+(s2<<16);
#else
	ClTable[n]=(s0+(s1<<8)+(s2<<16))^0x1f1f1f;
#endif
}

void start_composite(void)
{
	// �ϐ������ݒ�
	LineCount=0;				// ���������C���J�E���^�[
	drawing=0;
	VRAMp=VRAM;

	PR2 = H_NTSC -1; 			// ��63.5usec�ɐݒ�
	T2CONSET=0x8000;			// �^�C�}2�X�^�[�g
}
void stop_composite(void)
{
	T2CONCLR = 0x8000;			// �^�C�}2��~
}

// �J���[�R���|�W�b�g�o�͏�����
void init_composite(void)
{
	unsigned int i;
	
	scrn_clear();
	//�J���[�ԍ�0�`7�̃p���b�g������
	for(i=0;i<128;i++){
		set_palette(i, 128, 128, 128);
	}

	set_palette(0, 0, 0, 0);			// BLACK
	set_palette(1, 0, 0, 255);			// BRUE
	set_palette(2, 255, 0, 0);			// RED
	set_palette(3, 255, 0, 255);		// MAGENTA
	set_palette(4, 0, 255, 0);			// GREEN
	set_palette(5, 0, 255, 255);		// CYAN
	set_palette(6, 255, 255, 0);		// YELLOW
	set_palette(7, 255, 255, 255);		// WHITE
	set_palette(8, 128, 128, 128);		// GRAY
	set_palette(9, 128, 128, 255);		// BRUE_U
	set_palette(10, 0, 0, 96);			// BRUE_D
	set_palette(11, 255, 128, 128);		// RED_U
	set_palette(12, 128, 0, 0);			// RED_D
	set_palette(13, 255, 192, 255);		// MAGENTA_U
	set_palette(14, 128, 0, 128);		// MAGENTA_D
	set_palette(15, 192, 255, 192);		// GREEN_U
	set_palette(16, 0, 128, 0);			// GREEN_D
	set_palette(17, 128, 255, 255);		// CYAN_U
	set_palette(18, 0, 128, 128);		// CYAN_D
	set_palette(19, 255, 255, 128);		// YELLOW_U
	set_palette(20, 128, 128, 0);		// YELLOW_D
	set_palette(21, 192, 192, 192);		// GRAY_U
	set_palette(22, 64, 64, 64);		// GRAY_D
    
	// �^�C�}2�̏����ݒ�,�����N���b�N��63.5usec�����A1:1
	T2CON = 0x0000;				// �^�C�}2��~���
	IPC2bits.T2IP = 5;			// ���荞�݃��x��5
	IFS0bits.T2IF = 0;
	IEC0bits.T2IE = 1;			// �^�C�}2���荞�ݗL����
	
//	// �^�C�}3 1ms �莞���荞��
//	T3CON = 0x0000;
//	PR3 = 3579545*15/1000;		// 1ms
//	IPC3bits.T3IP = 6;				// ���荞�݃��x��4
//	IFS0bits.T3IF = 0;
//	IEC0bits.T3IE = 1;				// ���荞�ݗL��
//	T3CONSET = 0x8000;			// �^�C�}�X�^�[�g

	// OC1�̊��荞�ݗL����
	IPC1bits.OC1IP = 5;			// ���荞�݃��x��5
	IFS0bits.OC1IF = 0;
	IEC0bits.OC1IE = 1;			// OC1���荞�ݗL����

	// OC2�̊��荞�ݗL����
	IPC2bits.OC2IP = 5;			// ���荞�݃��x��5
	IFS0bits.OC2IF = 0;
	IEC0bits.OC2IE = 1;			// OC2���荞�ݗL����

	// OC3�̊��荞�ݗL����
	IPC3bits.OC3IP = 5;			// ���荞�݃��x��5
	IFS0bits.OC3IF = 0;
	IEC0bits.OC3IE = 1;			// OC3���荞�ݗL����

	OSCCONCLR=0x10; // WAIT���߂̓A�C�h�����[�h
//	BMXCONCLR=0x40;	// RAM�A�N�Z�X�E�F�C�g0
//    INTEnableSystemMultiVectoredInt();
//	__builtin_enable_interrupts();
    start_composite();
}


void pset(int x, int y, int c)
// (x,y)�̈ʒu�ɃJ���[c�œ_��`��
{
	if(x<0 || x>=X_RES || y<0 || y>=Y_RES)
		return;
	VRAM[X_RES*y + x] = c;
}

int pget(int x, int y)
{
	return VRAM[X_RES*y + x];
}
