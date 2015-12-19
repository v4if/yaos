/*
 * hw_port.h -- 端口的读写操作
 * Version: 1.0  12/15/2015 08:14:35 PM
 * Authors:
 *     Jichao Wu (Atkjest), karma_wjc@yeah.net
 */
#ifndef _HW_PORT_H
#define _HW_PORT_H

#include <../include/types.h>

//端口写一个字节
void outb(uint16_t port,uint8_t value);
//端口读一个字节
uint8_t inb(uint16_t port);
//端口读一个字
uint16_t inw(uint16_t port);

#endif

