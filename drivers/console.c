/*
 * console.c -- 屏幕操作相关函数的实现
 * Version: 1.0  12/15/2015 08:56:41 PM
 * Authors:
 *     Jichao Wu (Atkjest), karma_wjc@yeah.net
 */

/*
 * Date : 2015-12-18 17:16:28
 * 修改console_putc_color()函数，添加对ASCII控制字符的处理
 */

/*
 * Date : 2015-12-19 10:57:28
 * 添加对屏幕输出float实数数据的处理，console_write_float(float n);
 */

/*
 * 待解决问题：console_write_dec(025)与console_write_hex(025)输出的数据都等于21
 */

#include <../include/console.h>
#include <../include/hw_port.h>
#include <../include/math.h>

/*
 * 在默认的文本模式(Text-Mode)下，VGA控制器保留了一块内存(0x8b000~0x8bfa0)作为屏幕上字符显示的缓冲区，25*80
 * 从0x8B00这个地址开始，每两个字节代表屏幕上显示的一个字符，前一个字节是显示字符的ASCII，后一个是字符颜色的控制信息
 * 若要改变屏幕上字符的显示，只需要修改这块内存就好了。
 */

#define console_width 80
#define console_hight 25

//定义tab键宽度
#define tab_width 4

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
int console_putc(char c)
{
    //默认黑底白字
    return console_putc_color(c,vga_black,vga_white);
}

//屏幕输出一个字符（带颜色） 背景色，前景色
int console_putc_color(char c,vga_clolor_t back,vga_clolor_t fore)
{
    uint8_t back_color = (uint8_t)back;
    uint8_t fore_color = (uint8_t)fore;
    //颜色属性
    //attribute一定要定义成16位的，否则左移8位之后就全为0了
    uint16_t attribute = (back_color<<4) | fore_color;
    //attribute应该在显存两个字节中的高字节
    attribute = attribute << 8;
    
    //添加对ASCII码控制字符的处理
    //0x08是退格键、0x09是tab键 
    if (c == 0x08 && cursor_x)
    {
        cursor_x--;
    }
    else if (c == 0x09)
    {
        cursor_x = (cursor_x + tab_width) & ~(tab_width - 1);
    }
    else if (c == '\r')         //回车
    {
        cursor_x = 0;
    }
    else if (c == '\n')         //换行
    {
        cursor_x = 0;
        cursor_y++;
    }

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

    return 1;
}

//屏幕输出字符串 '\0' 结尾
int console_puts(char *cstr)
{
    int ret = 0;

    while(*cstr)
    {
        console_putc(*cstr++);
        ret++;
    }

    return ret;
}

//屏幕输出字符串 '\0' 结尾 带颜色
int console_puts_color(char *cstr, vga_clolor_t back, vga_clolor_t fore)
{
    int ret = 0;

    while(*cstr)
    {
        console_putc_color(*cstr++,back,fore);
        ret++;
    }

    return ret;
}

//屏幕输出十进制整形数 即以ACSII码文本形式输出
int console_write_dec(int n)
{   
    //x^32 = 4294967296
    char buff[10];
    char dec;
    int len = 0;

    if (abs(0 - n) <= PRECISION)
    {
        console_putc('0');
        return 1;
    }

/*  这里一定要判断精度是否为0，而不是判断除10取余是否为0，要不然遇到100这个函数就完了！！
    while((dec = n%10) && (dec != 0))
    {
        //输出ACSII码
        buff[len++] = dec + 0x30;
        n = n / 10;
    }
*/
    while(abs(0 - n) > PRECISION)
    {
        dec = n % 10;
        //输出ACSII码
        buff[len++] = dec + 0x30;
        n = n / 10;
    }

    buff[len] = '\0';

    //逆序压栈，需要转置
    int i;
    int temp;
    for ( i = 0; i < len / 2; ++i)
    {
        temp = buff[i];
        buff[i] = buff[len -i -1];
        buff[len -i -1] = temp;
    }
    
    //输出buff
    console_puts(buff);

    return len;
}

//屏幕输出十六进制数 即以binary二进制形式输出
int console_write_hex(int n)
{
    int i;
    int temp;
    int has_num = 0;
    int ret = 0;

    console_puts_color("0x",vga_black,WRITE_HEX_COLOR);

    //高位在左，低位在右
    for (i = 28; i >= 0; i = i-4)
    {
        //4个字节的获取
        temp = (n >> i) & 0xF;

        //如果0前面没有数据，将忽略输出0
        if (temp == 0 && has_num == 0)
        {
           continue;
        }
        has_num = 1;

        //十六进制数据0-9 A-F
        if (temp >= 0x0A)
        {
            //A-F
            console_putc_color(temp - 0xA + 'A',vga_black,vga_light_grey);
            ret++;
        }
        else 
        {
            //0-9
            console_putc_color(temp - 0x0 + '0',vga_black,vga_light_grey);
            ret++;
        }
    }

    return ret;
}

//屏幕输出float实数数据
/*
 * 注意：float的精度为6-7个有效数字
 */
int console_write_float(float n)
{
    int ret = 0;
    int integer;             //整数部分
    int decimal = 0;         //小数部分
    int decimal_fact;        //小数部分连续乘10得到的整数部分
    int fields = 0;          //小数部分有多少个前导0
    int has_num = 0;
    float precision;         //输出精度判断
    int layers = 0;

    //整个实数为0
    if (fabs(0 - n) <= PRECISION)
    {
        console_putc('0');
        return 1;
    }

    integer = (int)n;

    n = n - integer;
    //小数部分为0
    if (fabs(0 - n) <= PRECISION)
    {
        ret = console_write_dec(integer);
        return ret;
    }

    //函数如果执行到这里，小数部分一定不为0
    /* 
     * 结束条件为     (decimal_fact == 0) && (has_num == 1)
     * 所以逆命题为   (decimal_fact != 0) || (has_num == 0)
     *               fabs(precision) > PRECISION
     */
    precision = n;
    do{
        n = n * 10;
        decimal_fact = (int)n;
        n = n - decimal_fact;

        if (decimal_fact == 0 && has_num == 0)
        {
            fields++;
            layers++;
        }

        //前导0已经结束，已经有小数数据
        if (decimal_fact != 0 && has_num == 0)
        {
            has_num = 1;
        }

        //用decimal记录小数部分的数值
        if (decimal_fact != 0 && has_num != 0)
        {
            decimal = decimal * 10 + decimal_fact;

            layers++; 
            precision = precision - (float)decimal_fact / pow(10,layers);   
        }
    }while(fabs(precision) > PRECISION);

    int i;
    ret += console_write_dec(integer);
    ret += console_putc('.');
    for (i = 0; i < fields; ++i)
    {
        ret += console_putc('0');
    }
    ret += console_write_dec(decimal);

    return ret;
}
