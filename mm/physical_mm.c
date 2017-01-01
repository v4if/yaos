/* 
 * @File   Description: 全局物理内存管理
 *
 * @Create 		  Date:	2016-01-03 14:22:24
 * @Last Modified time:	2016-01-07 14:22:15
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */

#include "physical_mm.h"
#include "multiboot.h"
#include "stdio.h"

/*
 * 内核本身加载到物理内存的位置信息，这块内存必须是物理内存管理所保留的，全局物理内存管理
 */

//物理内存页面管理的栈
static uint32_t mm_stack[PAGE_MAX_SIZE];
//页面管理栈帧
static uint32_t mm_top = 0;
//物理内存页面数
static uint32_t mm_page_count = 0;

//打印bios提供的物理内存布局
void print_bios_map()
{
	//内存缓冲区地址和长度
	uint32_t mm_addr_start = glb_mboot_ptr->mmap_addr;
	uint32_t mm_addr_end = glb_mboot_ptr->mmap_addr + glb_mboot_ptr->mmap_length;			//mmap_entry_t 的字节长度

	mmap_entry_t *mm_entry = (mmap_entry_t *)mm_addr_start;

	for (; (uint32_t)mm_entry < mm_addr_end; mm_entry++)
	{
		printk("base_addr:%x\taddr_len:%x\tmm_type:%x\n",
			mm_entry->base_addr_low,mm_entry->length_low,mm_entry->type);
	}
}

//打印内核在内存中的使用情况
void print_kern_map()
{
	printk("kern in mm start:%x\n",kern_start);
	printk("kern in mm end  :%x\n",kern_end);
	//向上取整
	printk("kern in mm used :%dKB\n",(kern_end - kern_start + 1023) / 1024); 
}

//初始化内存物理分页
void init_physical_mm()
{
	//内存缓冲区地址和长度
	uint32_t mm_addr_start = glb_mboot_ptr->mmap_addr;
	uint32_t mm_addr_end = glb_mboot_ptr->mmap_addr + glb_mboot_ptr->mmap_length;			//mmap_entry_t 的字节长度

	mmap_entry_t *mm_entry = (mmap_entry_t *)mm_addr_start;

	for (; (uint32_t)mm_entry < mm_addr_end; mm_entry++)
	{
		// 如果是可用内存 ( 按照协议，1 表示可用内存，其它数字指保留区域 )
		if ( mm_entry->type == 1 && mm_entry->base_addr_low == 0x100000)
		{
			// 把内核结束位置到物理内存结束位置的内存段，按页存储到页管理栈里
			// 最多支持512MB的物理内存
			uint32_t phy_addr = 0x100000 + (kern_end - kern_start);
			uint32_t phy_end = 0x100000 + mm_entry->length_low;
			//将物理内存按照页存储到页管理栈里
			while(phy_addr < phy_end && phy_addr <= MM_MAX_SIZE)
			{
				//如果剩余内存不足一页则退出
				if (phy_end - phy_addr < MM_PAGE_SIZE)
				{
					break;
				}
				mm_free_page(phy_addr);
				phy_addr += MM_PAGE_SIZE;
				mm_page_count++;
			}
		}
	}
}

//分配内存物理页
uint32_t mm_alloc_page()
{
	if (mm_top <= 0)
	{
		printk("alloc error : out of memory !");
		return -1;
	}
	uint32_t page_addr = mm_stack[--mm_top];
	return page_addr;
}

//释放内存物理页
void mm_free_page(uint32_t p_addr)
{
	if (mm_top >= PAGE_MAX_SIZE)
	{
		printk("free error : out of stack !");
		return;
	}
	mm_stack[mm_top++] = p_addr;
}

//打印物理页内存栈 在我的电脑上会有一万多个页，是个不小的内存开销
void print_stack_map()
{
	uint32_t i;
	for (i = 0; i < mm_top; ++i)
	{
		printk("page %d -> alloc memory address : %x\n",i,mm_stack[i]);
	}
}

//打印内核物理页面数
void print_page_count()
{
	printk("physical mm page count : %d\n",mm_page_count);
}
