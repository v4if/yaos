/* 
 * @File   Description: 打印调试函数printk的相关实现
 *
 * @Create 		  Date:	2015-12-18 14:40:28
 * @Last Modified time:	2015-12-19 15:01:21
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */
#include <../include/stdarg.h>
#include <../include/console.h>
#include <../include/string.h>

static int vsprintf(const char *fmt,va_list args);

int printk(const char *fmt,...)
{
	//返回打印参数个数
	int ret_no;
	//打印可变参数变量的起始地址
	va_list arg_ptr;

	//fmt为形式化参数列表
	va_start(arg_ptr,fmt);
	ret_no = vsprintf(fmt,arg_ptr);
	va_end(arg_ptr);

#if 0
	//for debug
	console_puts((char *)fmt);
	/* 这里发现一个奇怪的现象，printk形式化参数列表传递的时候会自动忽略转义字符'\'
	 * 如'printk("This is a demo\n")'得到的解析形式化参数列表是This is a demo
	 * 订正：发现错误，是由于控制台屏幕打印函数的打印问题，控制台打印函数只支持打印ASCII码打印字符
	 * 对ASCII控制字符进行了忽略
	 */
#endif

	return ret_no;
}

//形式化参数列表解析，回车、换行等控制字符直接交给console_putc()函数处理就可以了，只解析参数数据
static int vsprintf(const char *fmt,va_list args)
{
	char ch;

	//解析变参类型数据模板
	int byte_len = 0;			//向屏幕输出字符数
	char ch_template;			//char
	char *str_template;			//string
	int int_template;			//int
	float float_template;		//float
	
	//解析形式化参数列表  解析参数buff[0] = *(char *)args;
	while(*fmt)
	{
		ch = *fmt++;
		if(ch == '%')
		{
			//解析下一个字符
			ch = *fmt++;
			switch (ch){
				//char
				case 'c' :  // *str++ = *(char *)args;
							// args = args + 1;
							ch_template = (unsigned char)va_arg(args,int);
							byte_len += console_putc(ch_template);
							break;
				//int
				case 'd' :	int_template = va_arg(args,int);
							byte_len += console_write_dec(int_template);
							break;
				//string
				case 's' :	str_template = va_arg(args,char *);
							// len = _strlen(str_template);
							// for (i = 0; i < len; ++i)
							// {
							// 	*str++ = *str_template++;
							// }
							byte_len += console_puts(str_template);
							break;
				//float
				case 'f' :
							float_template = va_arg(args,double);
							byte_len += console_write_float(float_template);
							break;
				//format error
				default  :	//错误的参数类型
							console_puts_color("The format of printk() is error\n",vga_black,ALLERT_COLOR);
							return -1;
			}
		}
		else
		{
			//非格式化参数直接输出
			byte_len += console_putc(ch);
		}
	}

	return byte_len;
}
