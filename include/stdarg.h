/* 
 * @File   Description: 可变参数相关的宏定义
 *
 * @Create 		  Date:	2015-12-18 14:11:14
 * @Last Modified time:	2015-12-18 14:57:11
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */
#ifndef _STDARGS_H
#define _STDARGS_H

//带有__builtin的都是编译器所能识别的函数，类似于关键字之类的
typedef __builtin_va_list va_list;				//参数列表指针，一般是char* 类型

//根据压栈顺序的最后一个固定参数获取可变参数的地址指针
#define va_start(arg_ptr,prev_param)	(__builtin_va_start(arg_ptr,prev_param))
//根据参数类型获取参数数据
#define va_arg(arg_ptr,type)			(__builtin_va_arg(arg_ptr,type))
//结束可变参数的获取
#define va_end(arg_ptr)					(__builtin_va_end(arg_ptr))

#endif
