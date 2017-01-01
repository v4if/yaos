/* 
 * @File   Description: 堆内存动态分配管理函数
 *
 * @Create 		  Date:	2016-01-06 16:20:01
 * @Last Modified time:	2016-01-08 19:25:23
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */
#include "physical_mm.h"
#include "virtual_mm.h"
#include "heap_mm.h"
#include "stdio.h"
#include "console.h"

/*
 * 堆内存管理头是用双向链表的方式组织的，即每个管理头的空间都需要动态申请，内存申请和释放的时候要通过该结构去管理
 * 那么问题来了，管理内存动态申请空间的管理头的空间也需要动态申请，陷入死循环
 * 这里把申请的空间先放置管理头，然后减去管理头的空间返回给申请者，管理头的大小为12byte
 */

//堆内存管理头，全局变量，记录堆内存管理头的起始地址
heap_header_t *glb_heap_header;
//堆内存最大位置，尾部
uint32_t glb_heap_max;

//打印堆内存管理结构
static void print_heap_header();
//向全局物理内存申请内存页 
static heap_header_t *alloc_chunk(heap_header_t *rear_heap_header,uint32_t len);
//按照申请长度切割内存块
static bool_t split_chunk(heap_header_t *cur_heap_header,uint32_t len);
//向全局物理内存释放内存页
static bool_t free_chunk(heap_header_t *rear_heap_header);
//合并内存块
static heap_header_t *glue_chunk(heap_header_t *cur_heap_header);


//初始化堆内存管理
void init_heap_mm()
{
	glb_heap_header = 0;
	glb_heap_max = HEAP_BASE;
}

//申请堆内存空间
void *mallock(uint32_t len)
{
	heap_header_t *cur_heap_header = glb_heap_header;
	//指向堆内存最后一个管理结构
	heap_header_t *rear_heap_header = glb_heap_header;
	//申请长度需要加上堆内存管理头
	len += sizeof(heap_header_t);

	while(cur_heap_header)
	{
		if (cur_heap_header->allocated == 0 && cur_heap_header->length >= len)
		{
			goto todo_split;
		}

		rear_heap_header = cur_heap_header;
		cur_heap_header = cur_heap_header->next;
	}

	//现有内存堆不能满足内存申请需求，需要从全局物理内存管理中申请内存页
	//这里传递的一定是最后一个内存管理结构，这样申请的内存页才能链接上
	cur_heap_header = alloc_chunk(rear_heap_header,len);
	if (cur_heap_header == 0)
	{
		//物理内存不足
		return 0;
	}
	//第一次申请内存页则初始化内存管理结构头指针
	if (glb_heap_header == 0)
	{
		glb_heap_header = cur_heap_header;
	}

todo_split:
	//按照申请内存长度切割内存块
	split_chunk(cur_heap_header,len);
	cur_heap_header->allocated = 1;

	//返回的指针在管理结构之后
	return (void *)((uint32_t)cur_heap_header + sizeof(heap_header_t));
}

//释放堆内存空间
bool_t freek(void *va)
{
	if ((uint32_t)va < HEAP_BASE)
	{
		return false_t;
	}

	//管理结构头地址
	heap_header_t *cur_heap_header = (heap_header_t *)((uint32_t)va - sizeof(heap_header_t));
	cur_heap_header->allocated = 0;

	// printk("----- freek -----\n");

	cur_heap_header = glue_chunk(cur_heap_header);
	//如果后面没有链表内存块了就直接释放掉
	if (cur_heap_header->next == 0)
	{
		free_chunk(cur_heap_header);
	}

	return true_t;
}

//向全局物理内存申请内存页 
static heap_header_t *alloc_chunk(heap_header_t *rear_heap_header,uint32_t len)
{
	// printk("----- alloc_chunk : %x\tglb_heap_max : %x-----\n",rear_heap_header,glb_heap_max);
	//添加堆内存管理头用
	uint32_t heap_max_old = glb_heap_max;
	uint32_t chunk_base = (uint32_t)rear_heap_header;

	if (rear_heap_header == 0)
	{
		chunk_base = HEAP_BASE;
	}
	else if (rear_heap_header != 0 && rear_heap_header->allocated == 1)
	{
		//存在管理节点，但是最后一块已经被分配
		chunk_base += rear_heap_header->length;
	}

	//申请内存页
	while(chunk_base + len > glb_heap_max)
	{	
		// printk("----- alloc_chunk -----\n");
		uint32_t page_addr = mm_alloc_page();
		if (page_addr == -1)
		{
			//物理内存不足
			return 0;
		}
		map(p_kern_dir, glb_heap_max, page_addr, PAGE_PRESENT | PAGE_WRITE);
		glb_heap_max += MM_PAGE_SIZE;

		// uint32_t *data = (uint32_t *)0xE0000000;
		// printk("map_test:%x\n",*data);
	}

	//最后一块内存仍有可用空间
	if (rear_heap_header != 0 && rear_heap_header->allocated == 0)
	{
		rear_heap_header->length = glb_heap_max - (uint32_t)rear_heap_header;
		// print_heap_header();
		return rear_heap_header;
	}
	else
	{
		heap_header_t *heap_header_new = (heap_header_t *)heap_max_old;

		//内存块链接 如果不是0则肯定存在前驱
		if (rear_heap_header != 0)
		{
			//申请内存页存在前驱
			rear_heap_header->next = heap_header_new;
			heap_header_new->prev = rear_heap_header;
		}
		else
		{
			heap_header_new->prev = 0;
		}
		heap_header_new->next = 0;

		//为新申请的内存块分配长度
		heap_header_new->allocated = 0;
		heap_header_new->length = glb_heap_max - heap_max_old;

		return heap_header_new;
	}
}

//按照申请长度切割内存块
static bool_t split_chunk(heap_header_t *cur_heap_header,uint32_t len)
{
	//切割之前保证剩下的内存块能够容纳一个内存管理块的大小
	if (cur_heap_header->length - len > sizeof(heap_header_t))
	{
		// printk("cur_heap_header_offset: %d\tlen%d\n",(uint32_t)cur_heap_header - HEAP_BASE,len);
		// printk("chunk_len: %d\n",cur_heap_header->length);

		//切割无非就是添加内存管理块然后链接之
		//注意这里一定是(uint32_t)cur_heap_header!!!
		heap_header_t *heap_header_new = (heap_header_t *)((uint32_t)cur_heap_header + len);

		//内存块链接
		if (cur_heap_header->next)
		{
			//被切割内存块存在后继
			cur_heap_header->next->prev = heap_header_new;
			heap_header_new->next = cur_heap_header->next;
		}
		else
		{
			heap_header_new->next = 0;
		}
		heap_header_new->prev = cur_heap_header;
		cur_heap_header->next = heap_header_new;

		//为新切割的内存块分配长度
		heap_header_new->allocated = 0;
		heap_header_new->length = cur_heap_header->length - len;
		//修改当前内存块长度
		cur_heap_header->length = len;

		return true_t;
	}

	return false_t;
}

//合并内存块
static heap_header_t *glue_chunk(heap_header_t *cur_heap_header)
{
	// printk("----- glue chunk -----\n");
	if (cur_heap_header == 0)
	{
		return false_t;
	}

	// printk("glue chunk 1\n");

	//如果前面有内存块且未被使用则合并
	if (cur_heap_header->prev && cur_heap_header->prev->allocated == 0)
	{
		cur_heap_header->prev->length = cur_heap_header->prev->length + cur_heap_header->length;
		cur_heap_header->prev->next = cur_heap_header->next;
		//存在后继
		if (cur_heap_header->next)
		{
			cur_heap_header->next->prev = cur_heap_header->prev;
		}
		//移动当前节点
		cur_heap_header = cur_heap_header->prev;
	}

	// printk("glue chunk 2\n");

	//如果后面有内存块且未被使用
	if (cur_heap_header->next && cur_heap_header->next->allocated == 0)
	{
		// printk("glue chunk 2-1\n");
		cur_heap_header->length = cur_heap_header->length + cur_heap_header->next->length;
		
		// printk("glue chunk 2-2\n");
		// printk("%x\n",cur_heap_header->next->next);
		// printk("glue chunk 2-3\n");
		//存在后继
		if (cur_heap_header->next->next)
		{
			cur_heap_header->next->next->prev = cur_heap_header;
		}

		//一定要在后面！！！
		cur_heap_header->next = cur_heap_header->next->next;
		// printk("glue chunk 2-4\n");
	}

	// printk("----- leave glue chunk -----\n");
	return cur_heap_header;
}

//向全局物理内存释放内存页
static bool_t free_chunk(heap_header_t *rear_heap_header)
{
	// printk("----- enter free chunk -----\n");
	if ((uint32_t)rear_heap_header < HEAP_BASE)
	{
		return false_t;
	}

	//如果空闲内存超过一页就释放掉
	while((uint32_t)rear_heap_header + MM_PAGE_SIZE <= glb_heap_max)
	{
		// printk("%x\n",rear_heap_header);
		uint32_t page_addr;
		glb_heap_max -= MM_PAGE_SIZE;
		get_mapping(p_kern_dir, glb_heap_max, &page_addr);
		unmap(p_kern_dir, glb_heap_max);
		mm_free_page(page_addr);
	}
	// printk("leave\n");
	//堆内存全部释放
	if (glb_heap_max == HEAP_BASE)
	{	
		glb_heap_header = 0;
	}
	else
	{
		rear_heap_header->length = glb_heap_max - (uint32_t)rear_heap_header;
	}

	return true_t;
}

//打印堆内存管理结构
static void print_heap_header()
{
	heap_header_t *cur_heap_header = glb_heap_header;

	if (cur_heap_header == 0)
	{
		console_puts_color("the glb_heap_header is null\n", vga_black, ALLERT_COLOR);
		return;
	}

	while(cur_heap_header)
	{
		printk("header_offset : %d\tallocated : %d\tlength : %d\n",(uint32_t)cur_heap_header - HEAP_BASE, cur_heap_header->allocated,cur_heap_header->length);
		cur_heap_header = cur_heap_header->next;
	}
}

//堆内存管理测试
void heap_test()
{
#if 0
	char *data = (char *)mallock(sizeof(char)*4080);
	printk("heap_test 1 : malloc byte : %d\taddr : %x\tdata : %d\n",4080,data,*data);
	print_heap_header();
	put_line();

	data = (char *)mallock(sizeof(char)*10);
	printk("heap_test 2: malloc byte : %d\taddr : %x\tdata : %d\n",10,data,*data);
	print_heap_header();
	put_line();

	data = (char *)mallock(sizeof(char)*10);
	printk("heap_test 3: malloc byte : %d\taddr : %x\tdata : %d\n",10,data,*data);
	print_heap_header();
	put_line();

	data = (char *)mallock(sizeof(char)*8192);
	printk("heap_test 4: malloc byte : %d\taddr : %x\tdata : %d\n",8192,data,*data);
	print_heap_header();
	put_line();
#endif
	char *addr1 = (char *)mallock(sizeof(char)*10);
	char *addr2 = (char *)mallock(sizeof(char)*50);
	char *addr3 = (char *)mallock(sizeof(char)*20);
	char *addr4 = (char *)mallock(sizeof(char)*10);

	freek(addr1);
	freek(addr2);
	freek(addr3);
	freek(addr4);

	print_heap_header();

	addr1 = (char *)mallock(sizeof(char)*4096);
	print_heap_header();

	freek(addr1);
	print_heap_header();
}
