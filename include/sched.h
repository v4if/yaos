/* 
 * @File   Description: 线程调度相关函数
 *
 * @Create 		  Date:	2016-01-09 01:18:50
 * @Last Modified time:	2016-01-12 17:03:57
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */

#ifndef _SCHED_H_
#define _SCHED_H_ 

#include "thread.h"

extern pcb_t *ready_list;			//就绪队列
extern pcb_t *blocked_list;		//阻塞队列
extern pcb_t *running_list;

//初始化线程调度器
void init_sched();
//线程调度函数，没有用复杂调度算法
void schedule();

#endif
