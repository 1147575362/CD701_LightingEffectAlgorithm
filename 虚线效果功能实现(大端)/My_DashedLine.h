#ifndef __MY_DASHEDLINE_H__
#define __MY_DASHEDLINE_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include<unistd.h>

#define BUFF_SIZE 13



// 灯光流动方向
enum dir
{
	Left = 1,
	Right,
	Close,
	Diffuse
};

typedef union 
{
    struct
    {
        uint16_t group_1:                   11;
        uint16_t group_2:                   11;
        uint16_t group_3:                   11;
        uint16_t group_4:                   11;
        uint16_t group_5:                   11;
        uint16_t group_6:                   11;
        uint16_t group_7:                   11;
        uint16_t group_8:                   11;
        uint16_t group_9:                   12;
        uint16_t        :                    4;
    } group;
    uint8_t data[BUFF_SIZE];
} Led_Data_t;


/*
	printf2:调试显示用，打印二进制
		data:uint_8类型的数组
		size:数组的大小，传sizeof即可
*/
void printf2(uint8_t data[], uint8_t size);

/*
	printf2:调试显示用，打印二进制
		n:一个uint16_t的数据
*/
void printf2_16(uint16_t n);

/*
	DashedLineSwimming:虚线游动
		data:uint_8类型的数组
		size:数组的大小，传sizeof即可
        led_nums:传100或？？？
        dir:游动方向
        times:次数，若led_nums传入100则是1~22之间循环，不到达23
*/
bool DashedLineSwimming(uint8_t data[], uint8_t size, uint8_t led_nums, uint8_t dir, uint16_t times);

/*
	u16Swimming_odd:奇数组虚线游动
        x:u16类型的变量，用于存储11颗/12颗led的变化
        dir:游动方向
        times:次数，若led_nums传入100则是1~22之间循环，不到达23
        @return:改变后的x
*/
uint16_t u16Swimming_odd(uint16_t x, uint8_t dir, uint8_t times);

/*
	u16Swimming_even:偶数组虚线游动
        x:u16类型的变量，用于存储11颗/12颗led的变化
        dir:游动方向
        times:次数，若led_nums传入100则是1~22之间循环，不到达23
        @return:改变后的x
*/
uint16_t u16Swimming_even(uint16_t x, uint8_t dir, uint8_t times);


/*
	u16Swimming_mid:中间组虚线游动
        x:u16类型的变量，用于存储11颗/12颗led的变化
        dir:游动方向
        times:次数，若led_nums传入100则是1~22之间循环，不到达23
        @return:改变后的x
*/
uint16_t u16Swimming_mid(uint16_t x, uint8_t dir, uint8_t times);


/*
	DashedLineMovement:虚线平移
		data:uint_8类型的数组
		size:数组的大小，传sizeof即可
        led_nums:传100或？？？
        dir:游动方向
        times:次数，若led_nums传入100则是1~22之间循环，不到达23
*/
bool DashedLineMovement(uint8_t data[], uint8_t size, uint8_t led_nums, uint8_t dir, uint16_t times);

/*
	u16Movement_odd:奇数组虚线平移
        x:u16类型的变量，用于存储11颗/12颗led的变化
        dir:游动方向
        times:次数，若led_nums传入100则是1~22之间循环，不到达23
        @return:改变后的x
*/
uint16_t u16Movement_odd(uint16_t x, uint8_t dir, uint8_t times);

/*
	u16Movement_even:偶数组虚线平移
        x:u16类型的变量，用于存储11颗/12颗led的变化
        dir:游动方向
        times:次数，若led_nums传入100则是1~22之间循环，不到达23
        @return:改变后的x
*/
uint16_t u16Movement_even(uint16_t x, uint8_t dir, uint8_t times);


/*
	u16Movement_mid:中间组虚线平移
        x:u16类型的变量，用于存储11颗/12颗led的变化
        dir:游动方向
        times:次数，若led_nums传入100则是1~22之间循环，不到达23
        @return:改变后的x
*/
uint16_t u16Movement_mid(uint16_t x, uint8_t dir, uint8_t times);

#endif