/* 
 * @File   Description: 内核初始化相关函数
 *
 * @Create 		  Date:	2016-01-04 23:53:44
 * @Last Modified time:	2016-01-12 18:31:07
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */

#include "console.h"
#include "stdio.h"
#include "gdt.h"
#include "idt.h"
#include "physical_mm.h"
#include "virtual_mm.h"
#include "heap_mm.h"
#include "sched.h"
#include "timer.h"

int flags = 0;

int thread_test(void *args)
{
	while(1)
	{
		if (flags == 1)
		{
			printk("B");
			flags = 0;
		}
	}
	return 1;
}

int kern_init()
{
	_irq_disable();

	init_gdt();
	init_idt();

	init_timer(19);
	// asm volatile ("int $0x20");

	//终端界面清屏
	console_clear();
	char *msg = (char *)0x0;
	console_puts_color(msg, vga_black, vga_cyan);
	put_line();

	//打印内核起始位置
	print_kern_map();
	put_line();
	//打印bios内存布局
	print_bios_map();
	put_line();

	//初始化物理内存管理
	init_physical_mm();
	//打印物理内存页数
	print_page_count();
	//初始化虚拟内存管理
	init_virtual_mm();
	//初始化堆内存管理
	init_heap_mm();

	heap_test();

	//初始化线程调度器
	init_sched();
	create_thread(thread_test, 0);
	
	// 开启中断
	_irq_enable();
	
	while(1)
	{
		//do something
		if (flags == 0)
		{
			printk("A");
			flags = 1;
		}
	}

	return 0;
}
