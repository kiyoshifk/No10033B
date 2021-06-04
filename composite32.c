// コンポジットカラー信号出力プログラム PIC32MX120F032B用　by K.Tanaka Rev.2
// 出力 PORTB
// RGB各色のVRAM形式（16bit単位で左端が上位）
//　カラー信号は120度ごとに出力
// カラーパレット対応（Rev.2）

// 出力に74HC04等のインバーターをバッファ代わりに接続する場合は、
// 以下の"#define INVERTER"を有効にする

//#define INVERTER

//#include "xc.h"
//#include <sys/attribs.h>
//#include "definitions.h"                // SYS function prototypes
//#include "composite32.h"
//#include <plib.h>

#include "app.h"


//カラー信号出力データ
//
#ifndef INVERTER

// 5bit DA、ストレート出力の場合の信号定数データ
#define C_SYN	0
#define C_BLK	6
#define C_BST1	6
#define C_BST2	3
#define C_BST3	8
//　ストレート出力用データここまで

#else
// 5bit DA、インバーター出力の場合こちらを有効にする
#define C_SYN	31
#define C_BLK	25
#define C_BST1	25
#define C_BST2	28
#define C_BST3	23
//　インバーター出力用データここまで
#endif

// パルス幅定数
#define V_NTSC		262					// 262本/画面
#define V_SYNC		10					// 垂直同期本数
#define V_PREEQ		53 //AAAAA	26					// ブランキング区間上側（V_SYNC＋V_PREEQは偶数とすること）
#define V_LINE		Y_RES // AAAAA	(Y_RES*2)			// 画像描画区間、縦解像度の2倍の本数
#define H_NTSC		3405				// 約63.5μsec
#define H_SYNC		252					// 水平同期幅、約4.7μsec
#define H_CBST		285					// カラーバースト開始位置（水平同期立下りから）
#define H_BACK		363					// 左スペース（水平同期立ち上がりから）

// グローバル変数定義
unsigned char VRAM[X_RES*Y_RES] __attribute__ ((aligned (4)));
unsigned char *VRAMp; //VRAMと処理中VRAMアドレス
volatile unsigned short LineCount;	// 処理中の行
volatile unsigned short drawcount;	//　1画面表示終了ごとに1足す。アプリ側で0にする。
							// 最低1回は画面表示したことのチェックと、アプリの処理が何画面期間必要かの確認に利用。
volatile char drawing;		//　映像区間処理中は-1、その他は0
volatile unsigned int TickCount;
int t2_div;
int buzzer_div;              // 8:1KHz, 9:889Hz, 10:800Hz, 11:727Hz, 12;667Hz, 13:615Hz, 14:615Hz, 15:533Hz, 16:500Hz
volatile int buzzer_ms;

//カラー信号テーブル
//各色32bitを最下位から8bitずつ順に出力
//Rev.2からRAMに配置しカラーパレット化
unsigned int ClTable[256];

//int data1,data2,data3,data4;//AAAAA

/**********************
*  Timer2 割り込み処理関数
*   63.5usec周期
***********************/
void __ISR(_TIMER_2_VECTOR, IPL5AUTO) T2Handler(void)
{
	asm volatile("#":::"a0");				// 破壊レジスタの宣言
	asm volatile("#":::"v0");

	//TMR2の値でタイミングのずれを補正
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
	asm volatile("sb	$a0,0($v0)");// 同期信号立ち下がり。ここを基準に全ての信号出力のタイミングを調整する

	if(LineCount<V_SYNC){
		// 垂直同期期間
		OC3R = H_NTSC-H_SYNC-3;	// 切り込みパルス幅設定
		OC3CON = 0x8001;
	}
	else{
		OC1R = H_SYNC-3;		// 同期パルス幅4.7usec
		OC1CON = 0x8001;		// タイマ2選択ワンショット
		if(LineCount>=V_SYNC+V_PREEQ && LineCount<V_SYNC+V_PREEQ+V_LINE){
			// 画像描画区間
			OC2R = H_SYNC+H_BACK-3-31;// 画像信号開始のタイミング
			OC2CON = 0x8001;		// タイマ2選択ワンショット
		}
	}
    ++t2_div;
    if((t2_div & 0xf)==0){          // 約 1ms (1.016ms)
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
	IFS0bits.T2IF = 0;			// T2割り込みフラグクリア
}

/*********************
*  OC3割り込み処理関数 垂直同期切り込みパルス
*********************/
void __ISR(_OUTPUT_COMPARE_3_VECTOR, IPL5AUTO) OC3Handler(void)
{
	asm volatile("#":::"v0");
	asm volatile("#":::"v1");
	asm volatile("#":::"a0");

	//TMR2の値でタイミングのずれを補正
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

	// 同期信号のリセット
	//	LATB=C_BLK;
asm volatile("label4_2:");
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BLK));
	asm volatile("la	$v0,%0"::"i"(&LATB));
	asm volatile("sb	$v1,0($v0)");	// 同期信号リセット。同期信号立ち下がりから3153サイクル

	IFS0bits.OC3IF = 0;			// OC3割り込みフラグクリア
}

/*********************
*  OC1割り込み処理関数 水平同期立ち上がり～カラーバースト
*********************/
void __ISR(_OUTPUT_COMPARE_1_VECTOR, IPL5AUTO) OC1Handler(void)
{
	asm volatile("#":::"v0");
	asm volatile("#":::"v1");
	asm volatile("#":::"a0");

	//TMR2の値でタイミングのずれを補正
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

	// 同期信号のリセット
	//	LATB=C_BLK;
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BLK));
	asm volatile("la	$v0,%0"::"i"(&LATB));
	asm volatile("sb	$v1,0($v0)");	// 同期信号リセット。水平同期立ち下がりから252サイクル

	// 28クロックウェイト
	asm volatile("addiu	$a0,$zero,9");
asm volatile("loop2:");
	asm volatile("addiu	$a0,$a0,-1");
	asm volatile("nop");
	asm volatile("bnez	$a0,loop2");

	// カラーバースト信号 9周期出力
	asm volatile("addiu	$a0,$zero,9");
	asm volatile("la	$v0,%0"::"i"(&LATB));
asm volatile("loop3:");
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BST1));
	asm volatile("sb	$v1,0($v0)");	// カラーバースト開始。水平同期立ち下がりから285サイクル
	asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BST2));
	asm volatile("sb	$v1,0($v0)");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BST3));
	asm volatile("sb	$v1,0($v0)");
	asm volatile("addiu	$a0,$a0,-1");//ループカウンタ
	asm volatile("nop");
	asm volatile("bnez	$a0,loop3");

	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BLK));
	asm volatile("sb	$v1,0($v0)");

	IFS0bits.OC1IF = 0;			// OC1割り込みフラグクリア
}


/***********************
*  OC2割り込み処理関数　映像信号出力
***********************/
void __ISR(_OUTPUT_COMPARE_2_VECTOR, IPL5AUTO) OC2Handler(void)
{
// 映像信号出力
	//インラインアセンブラでの破壊レジスタを宣言（スタック退避させるため）
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

	//TMR2の値でタイミングのずれを補正
	asm volatile("la	$v0,%0"::"i"(&TMR2));
	asm volatile("lhu	$a0,0($v0)");
//	asm volatile("addiu	$a0,$a0,-615-22+21");		// original line
	asm volatile("addiu	$a0,$a0,-615-22+21-8");

//asm volatile("la	$v0,%0"::"i"(&data4));
//asm volatile("sw	$a0,0($v0)");

	asm volatile("bltz	$a0,label3_2");			// >=0 である事
	asm volatile("addiu	$v0,$a0,-10");
	asm volatile("bgtz	$v0,label3_2");			// <0  である事
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
	//	t6=ループカウンタ
	asm volatile("addiu	$t6,$zero,160"); //ループカウンタ


	asm volatile("lbu	$a1,0($t1)");	// VRAM データ
	asm volatile("sll	$a1,$a1,2");	// x4
	asm volatile("addiu	$t1,$t1,1"); //VRAMp++;
	asm volatile("addu	$a0,$a1,$v1");
	asm volatile("lw	$t0,0($a0)");						// *****
	asm volatile("nop");//なくても1クロック挿入される

//    asm volatile("nop");
//    asm volatile("nop");
//    asm volatile("nop");
//    asm volatile("nop");
//    asm volatile("nop");
asm volatile("loop1:");
	asm volatile("sb	$t0,0($v0)");	// パレットデータ下位 8bit 出力
	asm volatile("srl	$t0,$t0,8");

	asm volatile("lbu	$a1,0($t1)");	// VRAM データ
	asm volatile("sll	$a1,$a1,2");	// x4  パレットオフセット
	asm volatile("addiu	$t1,$t1,1");	// VRAM アドレスインクリメント

	asm volatile("sb	$t0,0($v0)");	// パレットデータ中位 8bit 出力
	asm volatile("srl	$t0,$t0,8");

	asm volatile("addu	$a0,$a1,$v1");	// パレットデータアドレス
	asm volatile("nop");
	asm volatile("addiu	$t6,$t6,-1");//ループカウンタ

	asm volatile("sb	$t0,0($v0)");	// パレットデータ上位 8bit 出力

	asm volatile("lw	$t0,0($a0)");	// パレットデータ読み取り

	asm volatile("bnez	$t6,loop1");
	asm volatile("nop");

	//	LATB=C_BLK;
	asm volatile("addiu	$t0,$zero,%0"::"n"(C_BLK));
	asm volatile("sb	$t0,0($v0)");

	VRAMp += X_RES;
	if(LineCount==V_SYNC+V_PREEQ+V_LINE){ // 1画面最後の描画終了
			drawing=0;
			drawcount++;
			VRAMp=VRAM;
	}
	IFS0bits.OC2IF = 0;			// OC2割り込みフラグクリア
}

// 画面クリア
void scrn_clear(void)
{
	memset(VRAM, 0, sizeof(VRAM));
}

//void set_palette(int n, int r, int g, int b)
//{
//	// カラーパレット設定（5ビットDA、電源3.3V、120度単位）
//	// n:パレット番号、r,g,b:0～255
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
	// カラーパレット設定（5ビットDA、電源3.3V、120度単位）
	// n:パレット番号、r,g,b:0～255
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
	// 変数初期設定
	LineCount=0;				// 処理中ラインカウンター
	drawing=0;
	VRAMp=VRAM;

	PR2 = H_NTSC -1; 			// 約63.5usecに設定
	T2CONSET=0x8000;			// タイマ2スタート
}
void stop_composite(void)
{
	T2CONCLR = 0x8000;			// タイマ2停止
}

// カラーコンポジット出力初期化
void init_composite(void)
{
	unsigned int i;
	
	scrn_clear();
	//カラー番号0～7のパレット初期化
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
    
	// タイマ2の初期設定,内部クロックで63.5usec周期、1:1
	T2CON = 0x0000;				// タイマ2停止状態
	IPC2bits.T2IP = 5;			// 割り込みレベル5
	IFS0bits.T2IF = 0;
	IEC0bits.T2IE = 1;			// タイマ2割り込み有効化
	
//	// タイマ3 1ms 定時割り込み
//	T3CON = 0x0000;
//	PR3 = 3579545*15/1000;		// 1ms
//	IPC3bits.T3IP = 6;				// 割り込みレベル4
//	IFS0bits.T3IF = 0;
//	IEC0bits.T3IE = 1;				// 割り込み有効
//	T3CONSET = 0x8000;			// タイマスタート

	// OC1の割り込み有効化
	IPC1bits.OC1IP = 5;			// 割り込みレベル5
	IFS0bits.OC1IF = 0;
	IEC0bits.OC1IE = 1;			// OC1割り込み有効化

	// OC2の割り込み有効化
	IPC2bits.OC2IP = 5;			// 割り込みレベル5
	IFS0bits.OC2IF = 0;
	IEC0bits.OC2IE = 1;			// OC2割り込み有効化

	// OC3の割り込み有効化
	IPC3bits.OC3IP = 5;			// 割り込みレベル5
	IFS0bits.OC3IF = 0;
	IEC0bits.OC3IE = 1;			// OC3割り込み有効化

	OSCCONCLR=0x10; // WAIT命令はアイドルモード
//	BMXCONCLR=0x40;	// RAMアクセスウェイト0
//    INTEnableSystemMultiVectoredInt();
//	__builtin_enable_interrupts();
    start_composite();
}


void pset(int x, int y, int c)
// (x,y)の位置にカラーcで点を描画
{
	if(x<0 || x>=X_RES || y<0 || y>=Y_RES)
		return;
	VRAM[X_RES*y + x] = c;
}

int pget(int x, int y)
{
	return VRAM[X_RES*y + x];
}
