/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

//#include <stddef.h>                     // Defines NULL
//#include <stdbool.h>                    // Defines true
//#include <stdlib.h>                     // Defines EXIT_FAILURE
//#include "definitions.h"                // SYS function prototypes

#include "app.h"


void shooting_main();
void esakui_main();
void block_main();
void tetris_main();
void hako_main();
void hako_play();
void rush_main();
void pentomino_main();
void othe_main();
void slot_main();
void pin_main();
void sinkei_main();
void klon_main();
void life_main();
void gomoku_main();
void syogi_main();


extern int data1,data2,data3,data4;


const struct menu main_menu[]={
    {0, 20,  6, "ラッシュアワー"},
    {0, 35,  7, "スロットマシン"},
    {0, 50, 12, "クロンダイク"},
	{0, 65,  1, "Shooting"},
	{0, 75,  2, "Esakui"},
	{0, 85,  3, "Block"},
	{0, 95,  4, "Tetris"},
    {0, 105, 8, "Othello"},
    {0, 115, 9, "Pentomino"},
    {0, 125,10, "Pinball"},
    {0, 135,13, "life game"},

    {90, 20,  5, "箱入り娘"},
    {90, 35, 11, "神経衰弱"},
    {90, 50, 14, "五目並べ"},
    {90, 65, 15, "　将棋　"},
    
	{0,0,0},
};


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
	int cmd;
	
    /* Initialize all modules */
    SYS_Initialize ( NULL );

    LATASET = 0x0010;               // LED on


int i, j;
for(i=0; i<8; i++){
	for(j=0; j<3; j++){
		lcd_clearA(i*20, j*15, i*20+19, j*15+14, j*8+i);
	}
}
for(i=0; i<8; i++){
	for(j=0; j<3; j++){
		lcd_clearA(i*20, j*15+50, i*20+19, j*15+14+50, j*8+i+30);
	}
}
for(i=0; i<8; i++){
	for(j=0; j<3; j++){
		lcd_clearA(i*20, j*15+100, i*20+19, j*15+14+100, j*8+i+60);
	}
}
//sw_updown();


//for(;;){
//	ut_printf("%5d%5d%5d%5d\n", data1,data2,data3,data4);
//}


	for(;;){
		lcd_clear(GRAY);

		back_color = BLACK;
		text_color = WHITE;
		str_cdisp(2, "TVｹﾞｰﾑ V307 K.Fukumoto");
		
		cmd = menu_select(main_menu);
		
		lcd_y = MAX_LINE-1;
		back_color = BLACK;
		text_color = WHITE;
		switch(cmd){
			case 1:							// Shooting
				shooting_main();
				break;
			case 2:							// Esakui
				esakui_main();
				break;
			case 3:							// Block
				block_main();
				break;
			case 4:							// Tetris
				tetris_main();
				break;
			case 5:							// 箱入り娘
				hako_main();
				break;
			case 6:							// ラッシュアワー
				rush_main();
				break;
			case 7:							// スロットマシン
				slot_main();
				break;
			case 8:							// Othello
				othe_main();
				break;
			case 9:							// Pentomino
				pentomino_main();
				break;
			case 10:						// Pinball
				pin_main();
				break;
			case 11:						// 神経衰弱
				sinkei_main();
				break;
			case 12:						// クロンダイク
				klon_main();
				break;
			case 13:						// life game
				life_main();
				break;
			case 14:						// 五目並べ     77%
				gomoku_main();
				break;
			case 15:						// 将棋         83%
				syogi_main();
				break;
		}
	}


//    while ( true )
//    {
//        /* Maintain state machines of all polled MPLAB Harmony modules. */
//        SYS_Tasks ( );
//    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

