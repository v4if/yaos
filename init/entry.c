/*
 * entry.c -- 由grub引导的内核入口函数
 * Version: 1.0  12/14/2015 04:55:31 PM
 * Authors:
 *     Jichao Wu (Atkjest), karma_wjc@yeah.net
 */

#include "console.h"
#include "stdio.h"
#include "gdt.h"

int kern_entry()
{
	init_gdt();

	console_clear();

	//console_puts_color("Hello atkos , That is it",vga_black,vga_light_cyan);

	printk("Hello atkos\n");

	printk("\t%c %d %f %s",'z',25,1.35,"work it");

	return 0;
}

