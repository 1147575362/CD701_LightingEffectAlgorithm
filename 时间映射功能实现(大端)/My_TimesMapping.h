#ifndef __MY_TIMESMAPPING_H__
#define __MY_TIMESMAPPING_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

/*
	printf2:调试显示用，打印二进制
		data:uint_8类型的数组
		size:数组的大小，传sizeof即可
*/
void printf2(uint8_t data[], uint8_t size);

/*
	read_file:从txt文件中读取数据存入二维数组中
		filename:txt文件相对/绝对路径
		array:二维数组名(首地址)，用形参指针修改
        num_rows:二维数组的行数(地址)，用形参指针修改
        num_cols:二维数组的列数(地址)，用形参指针修改
        return:失败返回-1， 成功返回0
*/
uint8_t ReadFile(const char* filename, uint8_t*** array, uint8_t* num_rows, uint16_t* num_cols);

/*
	SetDataFromArray:将当前时刻的数据更新到data数组中
    current_time:当前时刻的值
    return:失败返回-1，成功返回0
*/
uint8_t SetDataFromArray(int current_time);

#endif