/* 
 * @File   Description: 虚拟内存映射管理
 *
 * @Create 		  Date:	2016-01-05 20:23:55
 * @Last Modified time: 2016-01-19 00:25:50
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */

#include "console.h"
#include "stdio.h"
#include "idt.h"
#include "string.h"
#include "virtual_mm.h"
#include "physical_mm.h"

//内核页目录
p_dir_t p_kern_dir[P_DIR_INDEX_SIZE] __attribute__((aligned(P_TABLE_CAPACITY)));
//内核页表
static p_table_t p_kern_table[P_DIR_MAX_SIZE][P_TABLE_INDEX_SIZE] __attribute__((aligned(P_TABLE_CAPACITY)));

//内存页错误中断处理函数
static void page_fault();

//初始化虚拟内存管理
void init_virtual_mm()
{
	int i;
	//内核偏移页目录项的起始地址
	int p_kern_index = GET_DIR_INDEX(KERN_OFFSET);
	for (i = 0; i < P_DIR_MAX_SIZE; ++i)
	{
		//此处是内核虚拟地址，MMU虚拟内存映射需要物理地址，因此减去偏移
		p_kern_dir[p_kern_index++] = ((uint32_t)p_kern_table[i] - KERN_OFFSET) | PAGE_PRESENT | PAGE_WRITE;
	}

	p_table_t *p_kern_ptr = (p_table_t *)p_kern_table;
	//映射512MB，那么有512MB/4K=128K个表项，一个表项是4个字节，所以一共是512K
	for (i = 0; i < P_TABLE_MAX_SIZE; ++i)
	{
		p_kern_ptr[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
	}

	// 注册页错误中断的处理函数 ( 14 是页故障的中断号 )
	register_interrupt_handler(14, page_fault);

	uint32_t p_kern_dir_addr = (uint32_t)p_kern_dir - KERN_OFFSET;

	//切换页目录
	asm volatile ("mov %0, %%cr3" : : "r" (p_kern_dir_addr));
}

//内存页错误中断处理函数
static void page_fault()
{
	console_puts_color("Page Fault !\n", vga_black, ALLERT_COLOR);
	while(1);
}

//内核虚拟分页测试函数，如果打印Page Fault !则分页映射错误，否则正确
void virtual_page_test()
{
	uint32_t *data = (uint32_t *)(MAX_VIRTUAL_SIZE - 4 + KERN_OFFSET);

	printk("\ntest %x",*data);
}

// 使用 flags 指出的页权限，把物理地址 pa 映射到虚拟地址 va
void map(p_dir_t *p_dir_ptr,uint32_t va,uint32_t pa,uint32_t flags)
{
	// printk("----- enter_page_map -----\n");
	uint32_t p_dir_index = GET_DIR_INDEX(va);
	uint32_t p_table_index = GET_TABLE_INDEX(va);
	// printk("p_dir_index : %d\tp_table_index : %d\n",p_dir_index,p_table_index);
	// printk("----- enter_page_map 1 -----\n");

	//这里一定要记住&PAGE_MASK！！！ 因为后面有页内存flag！！！
	p_table_t *p_table_new = (p_table_t *)(p_dir_ptr[p_dir_index] & PAGE_MASK);

	// printk("----- enter_page_map 2 -----\n");

	//取对应的页目录项，看是否已经映射过
	//对于已经映射过的不用申请页表
	if(p_table_new != 0)
	{
		// printk("----- enter_page_map if switch -----\n");
		//转换到内核线性地址
		p_table_new = (p_table_t *)((uint32_t)p_table_new + KERN_OFFSET);
	}
	else
	{
		// printk("----- enter_page_map else switch -----\n");
		//为页表申请空间
		p_table_new = (p_table_t *)mm_alloc_page();
		//挂载到页目录上
		p_dir_ptr[p_dir_index] = (uint32_t)p_table_new | PAGE_PRESENT | PAGE_WRITE;

		// printk("----- enter_page_map 3 -----\n");

		//将申请的页表转换到内核线性地址并清0
		p_table_new = (p_table_t *)((uint32_t)p_table_new + KERN_OFFSET);
		// printk("----- stl_page_mm : %x -----\n",p_table_new);
		memset(p_table_new, 0, P_TABLE_INDEX_SIZE * 4);
	}

	// printk("----- enter_page_map 4 -----\n");

	//设置页表项
	p_table_new[p_table_index] = (pa & PAGE_MASK) | flags;

	// printk("index 0 : %x\tindex 1 : %x\n",p_table_new[0],p_table_new[1]);
	// 通知 CPU 更新页表缓存
	asm volatile ("invlpg (%0)" : : "a" (va));

	// printk("----- leave_page_map -----\n");
}

// 取消虚拟地址 va 的物理映射
void unmap(p_dir_t *p_dir_ptr, uint32_t va)
{
	uint32_t p_dir_index = GET_DIR_INDEX(va);
	uint32_t p_table_index = GET_TABLE_INDEX(va);

	p_table_t *p_table_new = (p_table_t *)(p_dir_ptr[p_dir_index] & PAGE_MASK);

	//没有映射
	if (p_table_new == 0)
	{
		return;
	}

	//转换到内核线性地址
	p_table_new = (p_table_t *)((uint32_t)p_table_new + KERN_OFFSET);
	//取消映射
	p_table_new[p_table_index] = 0;

	// 通知 CPU 更新页表缓存
	asm volatile ("invlpg (%0)" : : "a" (va));
}

//得到虚拟地址的物理映射地址
void get_mapping(p_dir_t *p_dir_ptr,uint32_t va,uint32_t *pa)
{
	uint32_t p_dir_index = GET_DIR_INDEX(va);
	uint32_t p_table_index = GET_TABLE_INDEX(va);

	p_table_t *p_table = (p_table_t *)(p_dir_ptr[p_dir_index] & PAGE_MASK);

	if (p_table == 0)
	{
		return;
	}

	//转换到内核线性地址
	p_table = (p_table_t *)((uint32_t)p_table + KERN_OFFSET);

	//返回对应的物理地址
	*pa = p_table[p_table_index];
}

//地址映射测试
void map_test()
{
	uint32_t addr = mm_alloc_page();
	map(p_kern_dir,0xE0000000,addr,PAGE_PRESENT | PAGE_WRITE);
	uint32_t *data = (uint32_t *)0xE0000000;
	printk("map_test:%x\n",*data);


	addr = mm_alloc_page();
	map(p_kern_dir,0xE0001000,addr,PAGE_PRESENT | PAGE_WRITE);

	p_table_t *p_table_new = (p_table_t *)(p_kern_dir[GET_DIR_INDEX(0xE0000000)] & PAGE_MASK);
	p_table_new = (p_table_t *)((uint32_t)p_table_new + KERN_OFFSET);
	printk("index 0 : %x\tindex 1 : %x\n",p_table_new[0],p_table_new[1]);

	data = (uint32_t *)0xE0001000;
	printk("map_test:%x\n",*data);
}

//打印内核页目录
void print_page_dir()
{
	//uint32_t *addr = (uint32_t *)(0x1000+KERN_OFFSET);
	uint32_t *addr = p_kern_dir;

	//内核偏移页目录项的起始地址
	int p_kern_index = GET_DIR_INDEX(KERN_OFFSET);
	int i;
	for (i = 0; i < P_DIR_MAX_SIZE; ++i)
	{
		if(addr[p_kern_index] == 0)
			goto dir_todo_zero;
		if ((i % 6) == 0 && i != 0)
		{
			printk("\n");
		}
		printk("%x\t",addr[p_kern_index++]);
	}

dir_todo_zero:
	printk("\nbreak when i = %d",i);
	printk("\nP_DIR_SIZE = %d",P_DIR_MAX_SIZE);
}

//打印内核页表
void print_page_table()
{
	//uint32_t *addr = (uint32_t *)(0x3000+KERN_OFFSET);
	uint32_t *addr = (uint32_t *)p_kern_table;
	int i;
	for (i = 0; i < P_TABLE_MAX_SIZE; ++i)
	{
		if(*(addr+i)==0)
			goto table_todo_zero;
		if ((i % 6) == 0 && i != 0)
		{
			printk("\n");
		}
		printk("%x\t",*(addr+i));
	}
	
table_todo_zero:
	printk("\nbreak when i = %d",i);
	printk("\nP_TABLE_SIZE = %d",P_TABLE_MAX_SIZE);
}
