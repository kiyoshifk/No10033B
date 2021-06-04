#include "app.h"


/********************************************************************************/
/*		menu_select																*/
/*		è„ñÓàÛ/â∫ñÓàÛ: select													*/
/*		A_sw:          execute													*/
/*		B_sw:          abort(exit)												*/
/********************************************************************************/
int menu_select(const struct menu *tbl)
{
	int i, sw;
	
//	scrn_clear();
	back_color = WHITE;
	text_color = BRUE;
	for(i=0; tbl[i].msg ; ++i){
		str_disp(tbl[i].x, tbl[i].y, tbl[i].msg);
	}
	i = 0;
	for(;;){
		rev_flag = 1;
		str_disp(tbl[i].x, tbl[i].y, tbl[i].msg);	// îΩì]ï\é¶
		rev_flag = 0;
		
		sw = sw_updown();
		if(sw & DOWN){						// touch
			str_disp(tbl[i].x, tbl[i].y, tbl[i].msg);	// îÒîΩì]ï\é¶
			if(tbl[++i].msg==0)
				i = 0;
		}
		else if(sw & UP){
			str_disp(tbl[i].x, tbl[i].y, tbl[i].msg);	// îÒîΩì]ï\é¶
			if(--i < 0){
				for(i=0; tbl[i].msg; i++)
					;
				--i;
			}
		}
		else if(sw & A_SW){					// sw1: execute
			return tbl[i].cmd;
		}
		else if(sw & B_SW){					// sw2: abort(exit)
			return -1;
		}
	}
}
/********************************************************************************/
/*		VRAM_check																*/
/********************************************************************************/
void VRAM_check()
{
	int k;
	
	for(k=0; k<X_RES*Y_RES; k++){
		if(VRAM[k]>=8){
			ut_printf("\nVRAM[k]=%d\n", VRAM[k]);
			for(;;);
		}
	}
}