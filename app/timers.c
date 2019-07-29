#include "stm32f10x.h"
#include "timers.h"

int32_t sys_times;
uint32_t sys_now(void)
{
    return sys_times;
}

void sys_time_handler(void)
{
    sys_times = sys_times + 1;
}