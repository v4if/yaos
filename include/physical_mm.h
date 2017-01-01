/* 
 * @File   Description: 页内存管理相关函数
 *
 * @Create 		  Date:	2016-01-03 14:22:49
 * @Last Modified time:	2016-01-12 15:17:34
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */

#ifndef _PAGE_MM_H
#define _PAGE_MM_H

#include "types.h"

//内核栈
#define KERN_STACK_SIZE 8192
//物理叶框大小 4KB
#define MM_PAGE_SIZE 0x1000
//支持的最大物理内存 512MB
#define MM_MAX_SIZE 0x20000000
// 最多支持的物理页面个数
#define PAGE_MAX_SIZE (MM_MAX_SIZE/MM_PAGE_SIZE)

// 开启分页机制之后的内核栈
extern char kern_stack[KERN_STACK_SIZE];

// 内核文件在内存中的起始和结束位置
// 在链接器脚本中要求链接器定义
extern uint8_t kern_start[];
extern uint8_t kern_end[];

//打印bios提供的物理内存布局
void print_bios_map();
//打印内核在内存中的使用情况
void print_kern_map();
//初始化物理内存分页
void init_physical_mm();
//分配内存物理页
uint32_t mm_alloc_page();
//释放内存物理页
void mm_free_page(uint32_t p_addr);
//打印物理页内存栈
void print_stack_map();
//打印内核物理页面数
void print_page_count();

#endif // _PAGE_MM_H
