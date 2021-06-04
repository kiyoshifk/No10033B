#define X_RES	160 // 横方向解像度
#define Y_RES	150 // AAAAA	100 // 縦方向解像度
//#define H_WORD	(X_RES/8) // 1行当りのワード数(16bit単位)
//#define PLANESIZE	(H_WORD*Y_RES) // VRAM1枚当りのワード数(16bit単位)

extern volatile char drawing;		//　表示期間中は-1
extern volatile unsigned short drawcount;		//　1画面表示終了ごとに1足す。アプリ側で0にする。
							// 最低1回は画面表示したことのチェックと、アプリの処理が何画面期間必要かの確認に利用。

extern unsigned char VRAM[];
extern void start_composite(void); //カラーコンポジット出力開始
extern void stop_composite(void); //カラーコンポジット出力停止
extern void init_composite(void); //カラーコンポジット出力初期化
extern void scrn_clear(void); //画面クリア
extern void set_palette(int n, int r, int g, int b); //カラーパレット設定


//	t0=カラーデータ
//	t1=VRAMp
//	v1=ClTable
//	v0=&LATB
//	a1=パレットコード
