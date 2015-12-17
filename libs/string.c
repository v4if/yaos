/* 
 * @File   Description: 字符串函数操作
 *
 * @Create 		  Date:	2015-12-17 19:38:54
 * @Last Modified time:	2015-12-17 20:04:57
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */

#include "string.h"

//字符串拷贝函数
char *_strcpy(char *dest,char *src)
{
    char *ret = dest;

    while((*src != '\0')&&(*dest++ = *src++));
    *dest = '\0';

    return ret;
}

//计算字符串长度函数
int _strlen(char *cstr)
{
    int no = 0;

    while(*cstr++)
        no++;

    return no;
}

//追加字符串函数
char *_strcat(char *dest,char *src)
{
    int len = _strlen(dest);

    while(*src)
    {
        dest[len++] = *src++;
    }

    dest[len] = '\0';

    return dest;
}

//将字符串中的小写转为大写函数
char *_strupr(char *cstr)
{
    char *ret = cstr;

    while(*cstr)
    {
        //大于'a' && 小于'z'，只转换26个字母
        if((*cstr >= 0x61) && (*cstr <= 0x7A))
        {
            //小写转大写
            *cstr = *cstr - 0x20;
        }
        cstr++;
    }

    return ret;
}

//字符串比较函数，按照字典序
int _strcmp(char *str1,char *str2)
{
    int ret;
    while(*str1 && *str2 && (*str1 == *str2))
    {
        str1++;
        str2++;
    }

    //遇到不等的字符或者有一个字符串结束了，便会跳出循环
    //字符串结束符为'\0'，ASCII码为0x00，ACSII最小，直接相减就可以了
    ret = *str1 - *str2;
    if(ret > 0)
        return 1;
    else if(ret < 0)
        return -1;
    else
        return 0;
}
