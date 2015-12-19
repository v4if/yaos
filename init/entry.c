/*
 * entry.c -- 由grub引导的内核入口函数
 * Version: 1.0  12/14/2015 04:55:31 PM
 * Authors:
 *     Jichao Wu (Atkjest), karma_wjc@yeah.net
 */

#include <../include/console.h>
#include <../include/debug.h>
#include <../include/math.h>

int kern_entry()
{
	console_clear();

	//console_puts_color("Hello atkos , That is it",vga_black,vga_light_cyan);

	printk("Hello atkos\n");

	printk("\t%c %d %f %s",'z',25,1.35,"work it");

	return 0;
}

