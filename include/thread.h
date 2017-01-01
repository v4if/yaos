/* 
 * @File   Description: 线程相关函数
 *
 * @Create 		  Date:	2016-01-11 17:03:33
 * @Last Modified time:	2016-01-12 17:39:04
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */

#ifndef _THREAD_H
#define _THREAD_H 

#include "types.h"
#include "virtual_mm.h"

//进程栈大小
#define STACK_SIZE 8192

//进程id
typedef uint32_t pid_t;
//进程标识符
extern pid_t glb_pid;

//进程状态描述
typedef enum p_state
{
	ready = 0,				//就绪进程
	running = 1,			//正在运行进程
	blocked = 2,			//阻塞进程
}p_state;

//内核进程上下文contex
typedef struct p_context {
	uint32_t esp;
	uint32_t ebp;
	uint32_t ebx;
	uint32_t esi;
	uint32_t edi;
	uint32_t eflags;
}p_contex;

//进程内存地址映射
typedef struct mm_map
{
	p_dir_t *p_dir;	
}mm_map;

//进程控制块pcb
typedef struct pcb_t
{
	pid_t pid;				//进程标识符
	p_state state;			//进程当前状态
	p_contex context;		//进程切换需要的上下文信息
	mm_map *mm;				//进程的内存地址映像
	void *stack;			//进程内核栈地址
	struct pcb_t *next;		//单向循环链表
}pcb_t;

//线程函数指针
typedef int (*thread_handler)(void *args);
//创建内核线程
int create_thread(thread_handler handler,void *args);

#endif
	
