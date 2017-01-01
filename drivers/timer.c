/* 
 * @File   Description: 定时器相关函数
 *
 * @Create 		  Date:	2015-12-26 14:50:54
 * @Last Modified time:	2016-01-12 17:21:31
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */

#include "timer.h"
#include "stdio.h"
#include "idt.h"
#include "hw_port.h"
#include "console.h"
#include "sched.h"

uint32_t sys_tick = 0;
void timer_callback()
{
	//printk("System Tick %d\n",sys_tick++);
	schedule();
}

//frequency不能小于19
void init_timer(uint32_t frequency)
{
	// 注册时间相关的处理函数
	register_interrupt_handler(IRQ0, timer_callback);

	// Intel 8253/8254 PIT芯片 I/O端口地址范围是40h~43h
	// 输入频率为 1193180，frequency 即每秒中断次数
	uint32_t divisor = 1193180 / frequency;

	//一定要注意分频系数不能超过16位所表示的大小
	if (divisor > 65535)
	{
		console_puts_color((char*)"Alert->the divisor is overflow,please increase the timer frequency",vga_black,ALLERT_COLOR);
		//关掉定时器的中断
		outb(0x21, 0xff);
		return;
	}

	// D7 D6 D5 D4 D3 D2 D1 D0
	// 0  0  1  1  0  1  1  0
	// 即就是 36 H
	// 设置 8253/8254 芯片工作在模式 3 下
	outb(0x43, 0x34);

	// 拆分低字节和高字节
	uint8_t low = (uint8_t)(divisor & 0xFF);
	uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
	
	// 分别写入低字节和高字节
	outb(0x40, low);
	outb(0x40, high);

	//允许时钟中断
	outb(0x21, 0xfe);
}
