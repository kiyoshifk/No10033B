#include "app.h"

/********************************************************************************/
/*		MCrnd																	*/
/********************************************************************************/
unsigned int MCrndA()
{
	static int first=1;
	static unsigned long k;
	
	if(first){
		first = 0;
		srand(TickCount);
		init_genrand(TickCount);
	}
	if(k==0){
		k = genrand_int32() + 225555777;
	}
	k = ((long long)k * k)>>16;
	
	return (genrand_int32()+k+rand());
}

int MCrnd(int i)
{
	return (int)(MCrndA()%i);
}
