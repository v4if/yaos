/* 
 * @File   Description: 虚拟内存管理
 *
 * @Create 		  Date:	2016-01-04 23:18:32
 * @Last Modified time:	2016-01-08 15:15:46
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */

#ifndef _VIRTUAL_MM_H
#define _VIRTUAL_MM_H 

#include "types.h"

//开启分页
#define VIRTUAL_PAGE_ON 1
// 页掩码，用于 4KB 对齐
#define PAGE_MASK      0xFFFFF000
//内核最大虚拟映射大小 512MB
#define MAX_VIRTUAL_SIZE 0x20000000
//页目录所能容纳的页目录项
#define P_DIR_INDEX_SIZE 1024
//一个页目录项所能映射的最大物理地址数 4MB
#define P_DIR_CAPACITY 0x400000
//虚拟地址映射所需要的页目录项数
#define P_DIR_MAX_SIZE	(MAX_VIRTUAL_SIZE / P_DIR_CAPACITY)
//页表所容纳的页表项
#define P_TABLE_INDEX_SIZE 1024
//一个页表项所能映射的最大物理地址数 4KB
#define P_TABLE_CAPACITY 0x1000
//虚拟地址映射所需要的页表项数
#define P_TABLE_MAX_SIZE (MAX_VIRTUAL_SIZE / P_TABLE_CAPACITY)

// 内核原始地址
#define KERN_OLD			0x100000
// 内核的偏移地址
#define KERN_OFFSET 		0xC0000000

//获取一个地址的页目录项
#define GET_DIR_INDEX(x) 		(((x) >> 22) & 0x3FF)
// 获取一个地址的页表项
#define GET_TABLE_INDEX(x) 		(((x) >> 12) & 0x3FF)
// 获取一个地址的页內偏移
#define GET_PAGE_OFFSET(x) 		((x) & 0xFFF)

//页目录相关
/**
 * P--位0是存在（Present）标志，用于指明表项对地址转换是否有效。P=1表示有效；P=0表示无效。
 * 在页转换过程中，如果说涉及的页目录或页表的表项无效，则会导致一个异常。
 * 如果P=0，那么除表示表项无效外，其余位可供程序自由使用。
 * 例如，操作系统可以使用这些位来保存已存储在磁盘上的页面的序号。
 */
#define PAGE_PRESENT 	0x1
/** 
 * R/W--位1是读/写（Read/Write）标志。如果等于1，表示页面可以被读、写或执行。
 * 如果为0，表示页面只读或可执行。
 * 当处理器运行在超级用户特权级（级别0、1或2）时，则R/W位不起作用。
 * 页目录项中的R/W位对其所映射的所有页面起作用。
 */
#define PAGE_WRITE 	0x2
/**
 * U/S--位2是用户/超级用户（User/Supervisor）标志。
 * 如果为1，那么运行在任何特权级上的程序都可以访问该页面。
 * 如果为0，那么页面只能被运行在超级用户特权级（0、1或2）上的程序访问。
 * 页目录项中的U/S位对其所映射的所有页面起作用。
 */
#define PAGE_USER 	0x4

//页目录
typedef uint32_t p_dir_t;
//页表
typedef uint32_t p_table_t;

//内核页目录
extern p_dir_t p_kern_dir[P_DIR_INDEX_SIZE] __attribute__((aligned(P_TABLE_CAPACITY)));

//初始化虚拟内存管理
void init_virtual_mm();
// 使用 flags 指出的页权限，把物理地址 pa 映射到虚拟地址 va
void map(p_dir_t *p_dir_ptr,uint32_t va,uint32_t pa,uint32_t flags);
// 取消虚拟地址 va 的物理映射
void unmap(p_dir_t *p_dir_ptr, uint32_t va);
//得到虚拟地址的物理映射地址
void get_mapping(p_dir_t *p_dir,uint32_t va,uint32_t *pa);
//虚拟地址映射测试
void map_test();
//打印内核页目录
void print_page_dir();
//打印内核页表
void print_page_table();
//内核虚拟分页测试函数，如果打印Page Fault !则分页映射错误，否则正确
void virtual_page_test();

#endif
