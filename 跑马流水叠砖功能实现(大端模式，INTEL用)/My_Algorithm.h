#ifndef __MY_ALGORITHM_H__
#define __MY_ALGORITHM_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

// 灯光流动方向
enum dir
{
	Left = 0,
	Right,
	Close,
	Diffuse
};

//  应用于流水效果，设置高低电平有效
enum Active
{
    High = 0,
    Low
};

/*
	printf2:调试显示用，打印二进制
		n:一个8b的数据
*/
void printf2(char n);

/*
	RunningLamp:沿四个方向循环移动bits  (大端模式)
		data[]:数组元素个数范围1~13，元素类型uint8_t
        len:灯珠数量范围1~100
        dir:移动方向
        times:移动次数
*/
int RunningLamp(__UINT8_TYPE__ data[], uint8_t len, uint8_t dir, uint8_t times);

/*
	FlowingLamp:沿四个方向循环移动bits  (大端模式)
		data[]:数组元素个数范围，元素类型uint8_t
        size:数组的大小，传sizeof即可
		led_nums:灯珠数量，要比数组的bit数小
        dir:移动方向
        times:移动次数
        Act:High为高电平有效，low为低电平有效
*/
uint8_t FlowingLamp(uint8_t data[], uint8_t size, uint8_t led_nums, uint8_t dir, uint8_t times, uint8_t Act);

/*
	StackedBrickLamp:沿左右两个方向叠砖式移动bits  (大端模式)
		data[]:数组元素个数范围，元素类型uint8_t
        size:数组的大小，传sizeof即可
		led_nums:灯珠数量，需要比数组的bit数小
        dir:移动方向
        times:移动次数
		bits_lens:以多长的bit串为单位移动
        Act:High为高电平有效，low为低电平有效
*/
uint8_t StackedBrickLamp(uint8_t data[], uint8_t size, uint8_t led_nums, uint8_t dir, uint8_t times, uint8_t bits_lens, uint8_t Act);

#endif