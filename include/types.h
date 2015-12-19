/*
 * types.h -- 一些变量类型的定义
 * Version: 1.0  12/14/2015 04:46:35 PM
 * Authors:
 *     Jichao Wu (Atkjest), karma_wjc@yeah.net
 */

#ifndef __TYPES_H__
#define __TYPES_H__

#ifndef NULL
    #define NULL 0
#endif

#ifndef BOOLEAN
    #define TRUE 1
    #define FALSE 0
#endif

#ifndef PRECISION
 	#define PRECISION 1e-5			//0.00001
#endif
 
//变量类型定义
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned char uint8_t;
typedef char int8_t;

#endif
