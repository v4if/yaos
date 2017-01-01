/* 
 * @File   Description: 线程相关函数
 *
 * @Create 		  Date:	2016-01-11 17:02:59
 * @Last Modified time:	2016-01-12 17:39:46
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */

#include "thread.h"
#include "sched.h"
#include "heap_mm.h"

/*
 * 将进程控制块信息pcb放置在进程栈的最低段
 */

//进程标识符
pid_t glb_pid = 0;

//创建内核线程
int create_thread(thread_handler handler,void *args)
{
	//线程栈
	pcb_t *thread_new = (pcb_t *)mallock(STACK_SIZE);	

	thread_new->pid = glb_pid++;
	thread_new->state = ready;
	thread_new->stack = thread_new;
	thread_new->mm = NULL;				//使用内核页表

	uint32_t *stack_top = (uint32_t *)((uint32_t)thread_new + STACK_SIZE);

	*(--stack_top) = (uint32_t)args;
	*(--stack_top) = (uint32_t)handler;

	thread_new->context.esp = (uint32_t)thread_new + STACK_SIZE - sizeof(uint32_t) * 2;
	thread_new->context.ebp = (uint32_t)thread_new;

	// 设置新任务的标志寄存器未屏蔽中断，很重要
	thread_new->context.eflags = 0x200;

	//循环链表
	thread_new->next = running_list;

	pcb_t *tail = running_list;
	if (tail == 0)
	{
		return -1;
	}
	while(tail->next != running_list)
	{
		tail = tail->next;
	}
	tail->next = thread_new;

	return thread_new->pid;
}
