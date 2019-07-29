#include "process.h"
#include "key.h"
#include "ILI93xx.h"
u16 TimeMs_Led;

void TaskProcess(void)
{

	
		TimeMs_Led++;
		if(TimeMs_Led==5)
		{	
				TimeMs_Led=0;
							
				key_scan(1);
		}
	
		
	
	
		
}





	













