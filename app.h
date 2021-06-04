//#include <xc.h>
//#include <sys/attribs.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <setjmp.h>
#include <ctype.h>
//#include "configuration.h"
#include "definitions.h"
#include "composite32.h"


#define BLACK		0
#define BRUE		1
#define RED			2
#define MAGENTA		3
#define GREEN		4
#define CYAN		5
#define YELLOW		6
#define WHITE		7
#define GRAY		8
#define BRUE_U		9
#define BRUE_D		10
#define RED_U		11
#define RED_D		12
#define MAGENTA_U	13
#define MAGENTA_D	14
#define GREEN_U		15					// Ç±Ç±
#define GREEN_D		16
#define CYAN_U		17
#define CYAN_D		18
#define YELLOW_U	19
#define YELLOW_D	20
#define GRAY_U		21
#define GRAY_D		22

#define MAX_LINE	15		// ÉâÉCÉìêî
#define MAX_CHAR	22		// ÇPçsÇÃï∂éöêî

#define UP			0x0400
#define DOWN		0x0800
#define RIGHT		0x2000
#define LEFT		0x0200
#define A_SW		0x8000
#define B_SW		0x4000

#define printf      ut_printf

struct menu{
	int x;
	int y;
	int cmd;
	const char *msg;
};


extern int text_color;
extern int back_color;
extern int lcd_x;			// 0Å`21 (22ï∂éö)
extern int lcd_y;			// 0Å`14 (15çs)
extern int rev_flag;
extern volatile unsigned int TickCount;
int buzzer_div;              // 8:1KHz, 9:889Hz, 10:800Hz, 11:727Hz, 12;667Hz, 13:615Hz, 14:615Hz, 15:533Hz, 16:500Hz
extern volatile int buzzer_ms;
extern int work_buf[16000/sizeof(int)];


void set_palette(int n, int r, int g, int b);
void pset(int x,int y,int c);
int pget(int x, int y);
void scrn_clear(void);
void lcd_clear(int color);
void lcd_clearA(int x1, int y1, int x2, int y2, int color);
void write_square(int x, int y, int w, int h, const char *c);
void read_square(int x, int y, int w, int h, char *c);
void ut_printf(const char *fmt, ...);
void ut_puts(const char *str);
int ut_putc(const char *c);
void str_cdisp(int y, const char *str);
void str_disp(int x, int y, const char *str);
void sjis_disp(int x, int y, int c);
void sjis_disp_rev(int x, int y, int c);
int sw_sense();
int sw_updown();
void sw_up();
void wait_1ms();
void wait_ms(int ms);
int menu_select(const struct menu *tbl);
void VRAM_check();
void init_genrand(unsigned long s);
unsigned long genrand_int32();
long genrand_int31();
int MCrnd(int i);
unsigned int MCrndA();
