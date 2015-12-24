/* 
 * @File   Description: 全局描述符表相关函数声明
 *
 * @Create 		  Date:	2015-12-23 19:00:12
 * @Last Modified time:	2015-12-24 13:30:16
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */
#ifndef _GDT_H
#define _GDT_H

#include "types.h"

/*
 * x86 小端模式，结构体构造的时候低地址在前
 */

//全局描述符类型，每个描述符表项占8个字节
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

//GDTR，全局描述符表寄存器，Intel 48寄存器结构
typedef struct gdtr_t
{
	uint16_t limit;				//全局描述符表限长
	uint32_t base;				//全局描述符表32位基地址
}__attribute__((packed)) gdtr_t;

//gdt全局描述符表初始化
void init_gdt();

//将gdt的基地址加载到gdtr，汇编实现
extern void gdtr_flush(uint32_t gdtr); 

#endif
