#ifndef __DOUBLETPEAK_H__
#define __DOUBLETPEAK_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

/*
	printf2:调试显示用，打印二进制
		n:一个8b的数据
*/
void printf2(char n);


/*
	DoubletPeak:双峰灯效(大端)
        data[]:数组元素个数>=6(因为灯珠数量为43)，元素类型uint8_t
        size:数组的大小，传sizeof即可
        times:移动次数
*/
uint8_t DoubletPeak(uint8_t data[], uint8_t size, uint8_t times);


#endif