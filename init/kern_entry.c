/* 
 * @File   Description: grub引导之后进入的函数，在内核初始化之前完成虚拟地址映射
 *
 * @Create 		  Date:	2016-12-14 04:47:43
 * @Last Modified time:	2016-01-06 22:54:32
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */

#include "multiboot.h"
#include "physical_mm.h"
#include "virtual_mm.h"

extern int kern_init();

extern multiboot_t *glb_mboot_old;

// 开启分页机制之后的 Multiboot 数据指针
multiboot_t *glb_mboot_ptr;

// 开启分页机制之后的内核栈
char kern_stack[KERN_STACK_SIZE];

/*
 * 新旧物理内存都只临时映射一个页目录项，即4MB大小的内存空间
 */
// 我们使用1MB以下的地址来暂时存放临时页表和页目录
// 该地址必须是页对齐的地址，内存 0-640KB 肯定是空闲的
__attribute__((section(".init.data"))) p_dir_t *p_dir_temp  = (p_dir_t *)0x1000;
__attribute__((section(".init.data"))) p_table_t *p_table_old  = (p_table_t *)0x2000;
__attribute__((section(".init.data"))) p_table_t *p_table_new = (p_table_t *)0x3000;

// 内核入口函数
__attribute__((section(".init.text"))) void kern_entry()
{
	//操作系统魔数
	char *os_magic = (char *)0x0;
	*os_magic++ = 'H';
	*os_magic++ = 'e';
	*os_magic++ = 'l';
	*os_magic++ = 'l';
	*os_magic++ = 'o';
	*os_magic++ = ',';
	*os_magic++ = 'a';
	*os_magic++ = 't';
	*os_magic++ = 'k';
	*os_magic++ = 'o';
	*os_magic++ = 's';
	*os_magic++ = '!';
	*os_magic++ = '\n';
	*os_magic++ = '\0';

	p_dir_temp[GET_DIR_INDEX(KERN_OLD)] = (uint32_t)p_table_old | PAGE_PRESENT | PAGE_WRITE;
	//一个页目录最大映射4MB的物理内存大小，所以映射512MB共需要128个页目录项

	//内核偏移页目录项的起始地址
	p_dir_temp[GET_DIR_INDEX(KERN_OFFSET)] = (uint32_t)p_table_new | PAGE_PRESENT | PAGE_WRITE;

	int i;
	// 映射内核虚拟地址 4MB 到物理地址的前 4MB
	// 一个页表项是4个字节，1k个表项正好是4kb大小的占用内存空间，填满上面申请的p_table_t
	// 一个页面的大小是4Kb，1024=1k个页表项正好映射内核的4Mb空间
	for (i = 0; i < 1024; ++i) {
		p_table_old[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
	}

	//映射 0x00000000-0x00400000 的物理地址到虚拟地址 0xC0000000-0xC0400000
	for (i = 0; i < 1024; i++) {
		p_table_new[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
	}
	
	// 设置临时页表
	asm volatile ("mov %0, %%cr3" : : "r" (p_dir_temp));

	uint32_t cr0;

	// 启用分页，将 cr0 寄存器的分页位置为 1 就好
	asm volatile ("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= 0x80000000;
	asm volatile ("mov %0, %%cr0" : : "r" (cr0));
	
	// 在初始化内核函数栈之前一定不要有函数调用！！！
	// 切换内核栈
	uint32_t kern_stack_top = ((uint32_t)kern_stack + KERN_STACK_SIZE) & 0xFFFFFFF0;
	asm volatile ("mov %0, %%esp\n\t"
			"xor %%ebp, %%ebp" : : "r" (kern_stack_top));

	// 更新全局 multiboot_t 指针
	glb_mboot_ptr = glb_mboot_old + KERN_OFFSET;

	// 调用内核初始化函数
	kern_init();
}
