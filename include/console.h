/*
 * console.h -- 屏幕操作的相关函数头文件
 * Version: 1.0  12/15/2015 08:48:31 PM
 * Authors:
 *     Jichao Wu (Atkjest), karma_wjc@yeah.net
 */
#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "types.h"

typedef enum vga_color
{
    vga_black = 0,
	vga_blue = 1,
	vga_green = 2,
	vga_cyan = 3,
	vga_red = 4,
	vga_magenta = 5,
	vga_brown = 6,
	vga_light_grey = 7,
	vga_dark_grey = 8,
	vga_light_blue = 9,
	vga_light_green = 10,
	vga_light_cyan = 11,
	vga_light_red = 12,
	vga_light_magenta = 13,
	vga_light_brown  = 14, 	// yellow
	vga_white = 15
}vga_clolor_t;

//清屏操作
void console_clear();

//屏幕输出一个字符
void console_putc(char c);

//屏幕输出一个字符（带颜色） 背景色，前景色
void console_putc_color(char c,vga_clolor_t back,vga_clolor_t fore);

//屏幕输出字符串 '\0' 结尾
void console_puts(char *cstr);

//屏幕输出字符串 '\0' 结尾 带颜色
void console_puts_color(char *cstr, vga_clolor_t back, vga_clolor_t fore);

#endif

