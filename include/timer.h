/* 
 * @File   Description: 
 *
 * @Create 		  Date:	2015-12-26 14:51:33
 * @Last Modified time:	2015-12-26 15:30:33
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */

#ifndef _TIMER_H
#define _TIMER_H

#include "types.h"

extern uint32_t sys_tick;

//frequency不能小于19
void init_timer(uint32_t frequency);

#endif
