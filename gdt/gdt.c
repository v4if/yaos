/* 
 * @File   Description: 全局描述符表相关函数
 *
 * @Create 		  Date:	2015-12-23 18:59:33
 * @Last Modified time:	2015-12-24 13:29:58
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */

/*
* 内核进入保护模式后开始按照保护模式进行寻址，这里构造的是平坦模式，即0x0000 0000~0xFFFF FFFF为一个大段
* 如果gdt加载不正确的话，可能会造成内核循环启动
*/

#include "gdt.h"

#define GDT_LENTH 5

//GDTR
gdtr_t gdtr;

//GDT
gdt_entry_t gdt_entry[GDT_LENTH];

//全局描述符表构造函数
static void gdt_set_gate(uint16_t selector,uint32_t base,uint32_t limit,uint8_t access,uint8_t granularity);

//初始化全局描述符表，并将gdt的基地址加载到gdtr
void init_gdt()
{
	//全局描述符表界限从0开始编址
	gdtr.limit = sizeof(gdt_entry_t) * GDT_LENTH - 1;
	//全局描述符表基址
	gdtr.base = (uint32_t)&gdt_entry[0];

	//对照段描述符寄存器添加attribute，寄存器的读写要按照手册
	// 采用 Intel 平坦模型
	gdt_set_gate(0, 0, 0, 0,0);             		// 按照 Intel 文档要求，第一个描述符必须全 0
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A,0xCF); 	// 指令段
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92,0xCF); 	// 数据段
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA,0xCF); 	// 用户模式代码段
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2,0xCF); 	// 用户模式数据段

	//将gdt的基地址加载到gdtr，这里加载的是gdtr全局描述符表寄存器
	gdtr_flush((uint32_t)&gdtr);
}
/*
typedef struct gdt_entry_t
{
	uint16_t limit_low;			//段界限低16位
	uint16_t base_low;			//基地址低16位
	uint8_t  base_middle;		//基地址中间8位
	//uint16_t attribute;			//段属性（含段界限的高4位）
	uint8_t  access;			//段存在位、描述符特权级、描述符类型、描述符子类别
	uint8_t  granularity;		//段粒度   含段界限的高4位
	uint8_t  base_high;			//基地址高8位
}__attribute__((packed)) gdt_entry_t;
*/
static void gdt_set_gate(uint16_t selector,uint32_t base,uint32_t limit,uint8_t access,uint8_t granularity)
{
	gdt_entry[selector].base_low = (base & 0xFFFF);
	gdt_entry[selector].base_middle = (base >> 16) & 0xFF;
	gdt_entry[selector].base_high = (base >> 24) & 0xFF;

	gdt_entry[selector].limit_low = (limit & 0xFFFF);
	gdt_entry[selector].granularity = (limit >> 16) & 0xF;

	gdt_entry[selector].access = access;
	gdt_entry[selector].granularity |= granularity & 0xF0;
}
