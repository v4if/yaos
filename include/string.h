/*
 * string.h -- 字符串函数相关操作
 * Version: 1.0  12/17/2015 07:43:02 PM
 * Authors:
 *     Jichao Wu (Atkjest), karma_wjc@yeah.net
 */
#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

//字符串拷贝函数
char *strcpy(char *dest,char *src);
//字符串长度计算函数
int strlen(char *cstr);
//字符串追加函数
char *strcat(char *dest,char *src);
//字符串转大写函数
char *strupr(char *cstr);
//字符串比较函数
int strcmp(char *str1,char *str2);
//内存拷贝函数
void *memcpy(void *dest,void *src,int len);
//内存填充函数
void *memset(void *dest,int ch,int len);

#endif // STRING_H_INCLUDED

