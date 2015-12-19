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
	//一定要先清屏在输出字符，不然会出现问题，黑底白字或者字符根本就显示不出来
	console_clear();

	//console_puts_color("Hello atkos , That is it",vga_black,vga_light_cyan);

	printk("This is a demo\t%s %d %f\n","hello",23,0.35);

	return 0;
}

