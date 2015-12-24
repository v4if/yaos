/* 
 * @File   Description: 一些数学函数库
 *
 * @Create 		  Date:	2015-12-19 11:02:43
 * @Last Modified time:	2015-12-22 19:13:47
 *
 * @Author: 
 * 		atkjest(Jichao Wu)		
 *		karma_wjc@yeah.net	
 */
#include "math.h"

//求整形的绝对值
int abs(int n)
{
	if (n < 0)
	{
		n = 0 - n;
	}

	return n;
}

//求实型的绝对值
float fabs(float n)
{
	if (n < 0)
	{
		n = 0 - n;
	}

	return n;
}

float pow(float x,int y)
{
	float ret = x;
	//0的0次幂没有意义
	if (x ==0 && y == 0)
	{
		return -1;
	}

	//0的任何非0次幂都是0
	if (x == 0)
	{
		return 0;
	}

	//任何数的0次幂都为1
	if (y == 0)
	{
		return 1;
	}

	if (y > 0)
	{
		while (--y)
		{
			ret = ret * x;
		}
	}
	else
	{
		while (++y)
		{
			ret = ret * x;
		}
		ret = 1.0 / ret;
	}

	return ret;
}
