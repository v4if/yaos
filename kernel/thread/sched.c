/* 
 * @File   Description: 线程调度相关函数
 *
 * @Create 		  Date:	2016-01-09 01:17:24
 * @Last Modified time:	2016-01-12 18:17:26
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */

#include "sched.h"
#include "physical_mm.h"
#include "stdio.h"

//循环链表
pcb_t *ready_list;			//就绪队列
pcb_t *blocked_list;		//阻塞队列
pcb_t *running_list;		//运行队列

//切换当前任务，汇编实现
extern void switch_to(void *running_list,void *switch_to_thread);

//初始化线程调度器
void init_sched()
{
	//为当前执行流创建进程控制块，放在内核的栈底
	ready_list = (pcb_t *)(kern_stack);

	ready_list->pid = glb_pid++;
	ready_list->state = ready;
	ready_list->stack = kern_stack;
	ready_list->mm = NULL;				//使用内核页表

	//循环链表
	ready_list->next = ready_list;

	running_list = ready_list;
}

//线程调度函数
void schedule()
{
	pcb_t *switch_thread,*switch_prev;
	//调度策略
	if (running_list)
	{
		switch_prev = running_list;
		switch_thread = running_list->next;
	}

	if (switch_thread != running_list)
	{
		running_list = switch_thread;
		switch_to(&(switch_prev->context),&(switch_thread->context));
	}
}
