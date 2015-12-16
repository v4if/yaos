/*
 * entry.c -- 由grub引导的内核入口函数
 * Version: 1.0  12/14/2015 04:55:31 PM
 * Authors:
 *     Jichao Wu (Atkjest), karma_wjc@yeah.net
 */

#include "types.h"

int kern_entry()
{
	uint8_t *input = (uint8_t *)0xB8000;
    uint8_t color = (0 << 4) | (15 & 0x0F);

    *input++ = 'H'; *input++ = color;
    *input++ = 'e'; *input++ = color;
    *input++ = 'l'; *input++ = color;
    *input++ = 'l'; *input++ = color;
    *input++ = 'o'; *input++ = color;
    *input++ = ','; *input++ = color;
    *input++ = ' '; *input++ = color;
    *input++ = 'O'; *input++ = color;
    *input++ = 'S'; *input++ = color;
    *input++ = ' '; *input++ = color;
    *input++ = 'K'; *input++ = color;
    *input++ = 'e'; *input++ = color;
    *input++ = 'r'; *input++ = color;
    *input++ = 'n'; *input++ = color;
    *input++ = 'e'; *input++ = color;
    *input++ = 'l'; *input++ = color;
    *input++ = '!'; *input++ = color;

	return 0;
}

