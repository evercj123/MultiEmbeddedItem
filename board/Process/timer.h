#ifndef _TIMER_
#include "sys.h"
void timer3_init(u16 time,u16 psc);		//初始化定时器3 time位时间 psc为分频值
void timer3_close(void);					//关闭定时器3
#endif
