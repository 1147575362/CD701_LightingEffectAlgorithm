#ifndef __MY_VOICELAMP__H__
#define __MY_VOICELAMP__H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>


/*
	printf2:调试显示用，打印二进制
		n:一个8b的数据
		size:数组的大小，传sizeof即可
*/
void printf2(uint8_t data[], uint8_t size);

/*
	VoiceLamp:内置数组清零的类似左右流水效果
		data[]:数组元素个数范围，元素类型uint8_t
        size:数组的大小，传sizeof即可
		off_set:中心处离最左侧的偏移量，bit为单位
        max_len_left:左侧最长的流水距离
		max_len_right:右侧边最长的流水距离
*/
uint8_t VoiceLamp(uint8_t data[], uint8_t size, uint8_t off_set, uint8_t max_len_left, uint8_t max_len_right);

/*
	VoiceLampLeft:内置数组清零的类似左流水效果
		data[]:数组元素个数范围，元素类型uint8_t
        size:数组的大小，传sizeof即可
		off_set:中心处离最左侧的偏移量，bit为单位
        max_len:左侧最长的流水距离
		rise_flag:true为单向增加，false为单向递减(该标志位会自动变化无需手动传参)
*/
uint8_t VoiceLampLeft(uint8_t data[], uint8_t size, uint8_t off_set, uint8_t max_len, bool rise_flag);

/*
	VoiceLampRight:内置数组清零的类似右流水效果
		data[]:数组元素个数范围，元素类型uint8_t
        size:数组的大小，传sizeof即可
		off_set:中心处离最左侧的偏移量，bit为单位
        max_len:右侧最长的流水距离
		rise_flag:true为单向增加，false为单向递减(该标志位会自动变化无需手动传参)
*/
uint8_t VoiceLampRight(uint8_t data[], uint8_t size, uint8_t off_set, uint8_t max_len, bool rise_flag);
#endif