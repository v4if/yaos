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

#define WRITE_HEX_COLOR vga_light_magenta
#define ALLERT_COLOR vga_light_red

//清屏操作
void console_clear();

//##返回值为向屏幕输出的字符数##

//屏幕输出一个字符
int console_putc(char c);

//屏幕输出一个字符（带颜色） 背景色，前景色
int console_putc_color(char c,vga_clolor_t back,vga_clolor_t fore);

//屏幕输出字符串 '\0' 结尾
int console_puts(char *cstr);

//屏幕输出字符串 '\0' 结尾 带颜色
int console_puts_color(char *cstr, vga_clolor_t back, vga_clolor_t fore);

//屏幕输出十进制整形数 即以ACSII码文本形式输出
int console_write_dec(int n);

//屏幕输出十六进制数 即以binary二进制形式输出
int console_write_hex(int n);

//屏幕输出float实数数据
int console_write_float(float n);

#endif

