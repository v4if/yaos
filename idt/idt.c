/* 
 * @File   Description: 中断描述符相关函数
 *
 * @Create 		  Date:	2015-12-24 16:28:26
 * @Last Modified time:	2016-01-09 01:39:25
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */

#include "idt.h"
#include "string.h"
#include "stdio.h"
#include "hw_port.h"
#include "console.h"

/*
0~19（0x0~0x13） 		非屏蔽中断和异常 
21~31（0x14~0x1f） 		Intel 保留 
32~127（0x20~0x7f） 		外部中断（IRQ） 
128（0x80） 				系统调用（重点） 
129~238（0x81~0xee） 	外部中断（IRQ） 
239（0xef） 				本地 APIC 时钟中断（重点） 
240~250（0xf0~0xfa） 	由 Linux 留做将来使用 
251~255（0xfb~0xff） 	处理器间中断（必须是SMP机器）
*/

//中断描述符表
idt_entry_t idt_entry[256];

//中断描述符寄存器
idtr_t idtr;

//中断服务处理函数
interrupt_handler_t interrupt_handler[256];

//关中断 cli
inline void _irq_disable()
{
	asm volatile("cli");
}

//开中断 sti
inline void _irq_enable()
{
	asm volatile("sti");
}

#if 0
//irq测试函数
static void irq_test()
{
	printk("\nThis is the irq_test\n");
	return;
}
#endif

//初始化中断控制器
static void init_8259a();

//设置中断描述符
static void idt_set_gate(uint8_t irq_no,uint32_t offset,uint16_t seg_sel,uint8_t flag);

//初始化中断描述符
void init_idt()
{
	init_8259a();

	//清空中断服务处理函数
	memset(interrupt_handler, 0, sizeof(interrupt_handler_t) * 256);

	//idtr中断描述符寄存器
	idtr.limit = sizeof(idt_entry_t) * 256 -1;
	idtr.base = (uint32_t)&idt_entry[0];

	//没有设置中断服务处理函数的中断描述符表项要置0
	memset(idt_entry, 0, sizeof(idt_entry_t) * 256);

	// 0-32:  用于 CPU 的中断处理
	idt_set_gate( 0, (uint32_t)isr0,  0x08, 0x8E);
	idt_set_gate( 1, (uint32_t)isr1,  0x08, 0x8E);
	idt_set_gate( 2, (uint32_t)isr2,  0x08, 0x8E);
	idt_set_gate( 3, (uint32_t)isr3,  0x08, 0x8E);
	idt_set_gate( 4, (uint32_t)isr4,  0x08, 0x8E);
	idt_set_gate( 5, (uint32_t)isr5,  0x08, 0x8E);
	idt_set_gate( 6, (uint32_t)isr6,  0x08, 0x8E);
	idt_set_gate( 7, (uint32_t)isr7,  0x08, 0x8E);
	idt_set_gate( 8, (uint32_t)isr8,  0x08, 0x8E);
	idt_set_gate( 9, (uint32_t)isr9,  0x08, 0x8E);
	idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
	idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
	idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
	idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
	idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
	idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
	idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
	idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
	idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
	idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
	idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
	idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
	idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
	idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
	idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
	idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
	idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
	idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
	idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
	idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
	idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
	idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);

	idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);
	idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
	idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E);
	idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E);
	idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E);
	idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E);
	idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E);
	idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E);
	idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E);
	idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E);
	idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
	idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
	idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
	idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
	idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
	idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);

	// 255 将来用于实现系统调用
	idt_set_gate(128, (uint32_t)isr128, 0x08, 0x8E);

	//将idtr加载到中断描述符寄存器
	idtr_flush((uint32_t)&idtr);
}

// 设置中断描述符
static void idt_set_gate(uint8_t irq_no,uint32_t offset,uint16_t seg_sel,uint8_t flag)
{
	idt_entry[irq_no].offset_low = offset & 0xFFFF;
	idt_entry[irq_no].offset_high = (offset >> 16) & 0xFFFF;

	idt_entry[irq_no].selector = seg_sel;
	idt_entry[irq_no].reserved = 0;

	// 先留下 0x60 这个魔数，以后实现用户态时候
	// 这个与运算可以设置中断门的特权级别为 3
	idt_entry[irq_no].flag = flag;  // | 0x60
}

//调用中断处理函数
void isr_handler(irq_regs_t *irq_regs)
{
	if (interrupt_handler[irq_regs->int_no])
	{
		//调用中断处理服务函数
		interrupt_handler[irq_regs->int_no]();
	}
	else
	{
		printk("Unhandled ISR: %d\n", irq_regs->int_no);
		while(1);
	}
}
//irq处理函数
void irq_handler(irq_regs_t *irq_regs)
{
	// 发送中断结束信号给 PICs
	// 按照我们的设置，从 32 号中断起为用户自定义中断
	// 因为单片的 Intel 8259A 芯片只能处理 8 级中断
	// 故大于等于 40 的中断号是由从片处理的
	if (irq_regs->int_no >= 40) {
		// 发送重设信号给从片
		outb(0xA0, 0x20);
	}
	// 发送重设信号给主片
	outb(0x20, 0x20);

	if (interrupt_handler[irq_regs->int_no])
	{
		//调用中断处理服务函数
		interrupt_handler[irq_regs->int_no]();
	}
	else
	{
		printk("Unhandled IRQ: %d\n", irq_regs->int_no);
		while(1);
	}
}

//注册中断函数
void register_interrupt_handler(uint8_t irq_no,interrupt_handler_t h)
{
	interrupt_handler[irq_no] = h;
}

//初始化8259中断控制器
static void init_8259a()
{
	// 重新映射 IRQ 表
	// 两片级联的 Intel 8259A 芯片
	// 主片端口 0x20 0x21
	// 从片端口 0xA0 0xA1
	
	// 初始化主片、从片
	// 0001 0001
	outb(0x20, 0x11);
	outb(0xA0, 0x11);

	// 设置主片 IRQ 从 0x20(32) 号中断开始
	outb(0x21, 0x20);

	// 设置从片 IRQ 从 0x28(40) 号中断开始
	outb(0xA1, 0x28);
	
	// 设置主片 IR2 引脚连接从片
	outb(0x21, 0x04);

	// 告诉从片输出引脚和主片 IR2 号相连
	outb(0xA1, 0x02);
	
	// 设置主片和从片按照 8086 的方式工作
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	
	//8259上的中断可以控制屏蔽，对应IRQ写1屏蔽中断
	// 设置主从片禁止所有中断
	outb(0x21, 0xff);
	outb(0xA1, 0xff);
}
