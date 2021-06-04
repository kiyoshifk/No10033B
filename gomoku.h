//
//		gomoku.h
//

//#include <stdio.h>
//#include <memory.h>
//#include <string.h>
//#include <fcntl.h>
//#include <sys/stat.h>
//#include <time.h>
//#include <ctype.h>
//#include <stdlib.h>
//#include <stdarg.h>
//#include <io.h>
//#include <math.h>
//#include <windows.h>
//#include <share.h>
//#include <direct.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;


#define ISHI_N    0x0000		/* ÎƒiƒV					*/
#define ISHI_K    0x0001		/* •Î						*/
#define ISHI_S    0x0002		/* ”’Î						*/
#define ISHI_O    0x0003		/* ”ÕŠO						*/
//#define ISHI_SELF 0x0008
#define ISHI_K2   0x0010		// ‚±‚±‚É•‚ð‘Å‚Â‚Æ‚Q˜A‚É‚È‚é
#define ISHI_K3   0x0020		// ‚±‚±‚É•‚ð‘Å‚Â‚ÆŠˆ‚R‚É‚È‚é
#define ISHI_K4   0x0040		// ‚±‚±‚É•‚ð‘Å‚Â‚ÆŠˆ‚S‚É‚È‚é
#define ISHI_K4X  0x0080		// ‚±‚±‚É•‚ð‘Å‚Â‚Æ–_‚S‚É‚È‚é
#define ISHI_K5   0x0100		// ‚±‚±‚É•‚ð‘Å‚Â‚Æ‚T˜A‚É‚È‚é
#define ISHI_S2   0x0200		// ‚±‚±‚É”’‚ð‘Å‚Â‚Æ‚Q˜A‚É‚È‚é
#define ISHI_S3   0x0400		// ‚±‚±‚É”’‚ð‘Å‚Â‚ÆŠˆ‚R‚É‚È‚é
#define ISHI_S33  0x0800		// ‚±‚±‚É”’‚ð‘Å‚Â‚Æ‚R‚R‚É‚È‚é
#define ISHI_S4   0x1000		// ‚±‚±‚É”’‚ð‘Å‚Â‚ÆŠˆ‚S‚É‚È‚é
#define ISHI_S4X  0x2000		// ‚±‚±‚É”’‚ð‘Å‚Â‚Æ–_‚S‚É‚È‚é
#define ISHI_S5   0x4000		// ‚±‚±‚É”’‚ð‘Å‚Â‚Æ‚T˜A‚É‚È‚é
#define ISHI_KINSYU 0x8000		// ‚±‚±‚É•‚ð‘Å‚Â‚Æ‹ÖŽè


#define macrox(pos) ((pos)%16)	/* pos ‚©‚ç‚wÀ•W‚Ö‚Ì•ÏŠ·	*/
#define macroy(pos) ((pos)/16)	/* pos ‚©‚ç‚xÀ•W‚Ö‚Ì•ÏŠ·	*/
#define macropos(x,y) ((x)+(y)*16)	/* X,Y À•W‚©‚ç pos ‚Ö‚Ì•ÏŠ·	*/
//#define is_stone(k) (((k)&7)==ISHI_K || ((k)&7)==ISHI_S)
//#define ishazi(x,y) ((x)==1 || (x)==19 || (y)==1 || (y)==19)
//#define kk2k(kk) ((kk & IBAN_MASK)==0 ? ISHI_N : ((kk & IBAN_MASK) >= 0x200 ? ISHI_S : ISHI_K))
//#define k2kk(k) ((k & 7)==ISHI_N ? 0 :((k & 7)==ISHI_K ? 1 :((k & 7)==ISHI_S ? 0x201 : IBAN_STOP)))
#define macroinv(koma) ((koma)==ISHI_K ? ISHI_S : ISHI_K)
//#define abs1(x) ((x)>=0?(x):-(x))
#define min1(x,y) ((x)>(y)?(y):(x))
#define max1(x,y) ((x)>(y)?(x):(y))
