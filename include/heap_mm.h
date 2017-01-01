/* 
 * @File   Description: 堆内存动态分配管理函数
 *
 * @Create 		  Date:	2016-01-06 16:23:58
 * @Last Modified time:	2016-01-08 13:47:28
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */

#ifndef _HEAP_MM_H
#define _HEAP_MM_H 

#include "types.h"

//堆内存块基地址
#define HEAP_BASE 0xE0000000

//堆内存块管理头
typedef struct heap_header_t
{
	struct heap_header_t *prev;
	struct heap_header_t *next;
	uint32_t allocated : 1;				//是否被分配
	uint32_t length	: 31;				//内存块长度，包括内存管理块的长度
}heap_header_t;

//初始化堆内存管理
void init_heap_mm();
//申请堆内存空间
void *mallock(uint32_t len); 
//释放堆内存空间
bool_t freek(void *va);
//内核的内存堆管理测试
void heap_test();

#endif
