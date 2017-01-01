/* 
 * @File   Description: 调试相关的函数
 *
 * @Create 		  Date:	2015-12-18 14:41:30
 * @Last Modified time:	2016-01-07 23:28:32
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */
#ifndef _STDIO_H
#define _STDIO_H

//打印换行
int put_line();
//内核级调试打印函数
int printk(const char *fmt,...);

#endif
