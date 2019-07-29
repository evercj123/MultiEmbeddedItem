#include "stm32f10x.h"
#include "misc.h"
#include "interrupt_func.h"




void SysTick_Handler(void)
{
     sys_time_handler();   
}