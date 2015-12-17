/*
 * console.c -- 屏幕操作相关函数的实现
 * Version: 1.0  12/15/2015 08:56:41 PM
 * Authors:
 *     Jichao Wu (Atkjest), karma_wjc@yeah.net
 */
#include "console.h"
#include "hw_port.h"

/*
 * 在默认的文本模式(Text-Mode)下，VGA控制器保留了一块内存(0x8b000~0x8bfa0)作为屏幕上字符显示的缓冲区，25*80
 * 从0x8B00这个地址开始，每两个字节代表屏幕上显示的一个字符，前一个字节是显示字符的ASCII，后一个是字符颜色的控制信息
 * 若要改变屏幕上字符的显示，只需要修改这块内存就好了。
 */

#define console_width 80
#define console_hight 25

//将全局变量限制在本文件内部
//vga的显示缓冲起始地址0xB8000
static uint16_t *vga_memory = (uint16_t *)0xB8000;

//屏幕光标的坐标
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

//清屏操作
void console_clear()
{
    uint8_t back_color = (uint8_t)vga_black;
    uint8_t fore_color = (uint8_t)vga_white;
    
    //只要输出黑底白字的空格即可实现清屏
    //attribute一定要定义成16位的，否则左移8位之后就全为0了
    uint16_t attribute = (back_color<<4) | fore_color;
    //attribute应该在显存两个字节中的高字节
    attribute = attribute << 8;
    
    uint16_t blank = attribute | 0x20;
    uint16_t i;
    uint16_t no = console_width * console_hight;
    for (i = 0; i < no; ++i)
    {
        vga_memory[i] = blank;
    } 

    //将屏幕光标复位
    cursor_y = 0;
    cursor_x = 0;
}

//移动屏幕光标
static void move_cursor()
{
    // 屏幕是 80 字节宽
    uint16_t current_local = cursor_y * console_width + cursor_x;
    
    // VGA 内部的寄存器多达300多个，显然无法一一映射到I/O端口的地址空间。
    // 对此 VGA 控制器的解决方案是，将一个端口作为内部寄存器的索引：0x3D4，
    // 再通过 0x3D5 端口来设置相应寄存器的值。
    // 在这里用到的两个内部寄存器的编号为14与15，分别表示光标位置的高8位与低8位。

    outb(0x3D4, 14);                    // 告诉 VGA 我们要设置光标的高字节
    outb(0x3D5, current_local >> 8);    // 发送高 8 位
    outb(0x3D4, 15);                    // 告诉 VGA 我们要设置光标的低字节
    outb(0x3D5, current_local);         // 发送低 8 位
}

//滚动屏幕
static void scroll()
{
    uint16_t i;
    uint16_t no = console_width * (console_hight - 1);

    //构造黑底白字空格
    uint16_t attribute = ((uint8_t)vga_black<<4) | (uint8_t)vga_white;
    //attribute应该在显存两个字节中的高字节
    attribute = attribute << 8;
    uint16_t blank = attribute | 0x20;

    //将显存的第一行抛弃，所有显示的数据复制到上一行,最后一行空出来以黑底白字填充
    //cursor_y先加过1，因此判断console_height
    if (cursor_y >= console_hight)      
    {
        for (i = 0; i < no; ++i)
        {
            vga_memory[i] = vga_memory[i + console_width];
        }

        //最后一行
        for (i = no; i < console_width * console_hight; ++i)
        {
            vga_memory[i] = blank;
        }

        //向上移动了一行，因此需要改变cursor_y
        cursor_y = console_hight - 1;
    }
}

//屏幕输出一个字符,必须为ASCII码打印字符
void console_putc(char c)
{
    //默认黑底白字
    console_putc_color(c,vga_black,vga_white);
}

//屏幕输出一个字符（带颜色） 背景色，前景色
void console_putc_color(char c,vga_clolor_t back,vga_clolor_t fore)
{
    uint8_t back_color = (uint8_t)back;
    uint8_t fore_color = (uint8_t)fore;
    //颜色属性
    //attribute一定要定义成16位的，否则左移8位之后就全为0了
    uint16_t attribute = (back_color<<4) | fore_color;
    //attribute应该在显存两个字节中的高字节
    attribute = attribute << 8;
    
    //换行和tab键

    //是否为ASCII码打印字符
    if(c >= ' ')
    { 
        //小端模式，要将字符ASCII放在低字节
        //注意console_clear()之后cursor_y和cursor_x都已经复位为0
        vga_memory[cursor_y * console_width + cursor_x] = attribute | c;
        cursor_x++;
    }

    if (cursor_x >= console_width)
    {
        cursor_x = 0;
        cursor_y++;
    }

    //需要的话滚动屏幕显示
    scroll();

    //字符输出之后，移动硬件的输入光标
    move_cursor();
}

//屏幕输出字符串 '\0' 结尾
void console_puts(char *cstr)
{
    while(*cstr)
    {
        console_putc(*cstr++);
    }
}

//屏幕输出字符串 '\0' 结尾 带颜色
void console_puts_color(char *cstr, vga_clolor_t back, vga_clolor_t fore)
{
    while(*cstr)
    {
        console_putc_color(*cstr++,back,fore);
    }
}
