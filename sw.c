#include "app.h"


int sw_sense()
{
	return ~PORTB & 0xee00;
}

int sw_updown()
{
	int i, sw, sw_save;
	
	//	50ms off 確認
	for(i=0; i<50; i++){
		sw = sw_sense();
		if(sw)
			i = 0;
		wait_1ms();
	}
	//	10ms 入力確認
	sw_save = 0;
	for(i=0; i<10; i++){
		sw = sw_sense();
		if(sw==0 || sw != sw_save){
			i = 0;
			sw_save = sw;
		}
		wait_1ms();
	}
	
	return sw;
}

void sw_up()
{
    int i;
    
    for(i=0; i<50; i++){
        if(sw_sense())
            i = 0;
        wait_1ms();
    }
}

void wait_1ms()
{
	unsigned int time;
	
	time = TickCount;
	while(time==TickCount){
	}
}

void wait_ms(int ms)
{
	while(ms){
		wait_1ms();
		ms--;
	}
}
