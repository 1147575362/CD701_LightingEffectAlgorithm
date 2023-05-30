#include "My_VoiceLamp.h"

// 输出二进制 只能输出char类型 其他类型要修改
void printf2(uint8_t data[], uint8_t size)
{
	uint8_t i = 0, j = 0;
	for(i = 0; i < size; i++)
	{
		for (j = 0; j < 8; j++)
			if (data[i] & (0x80 >> j))
				// printf("1");
				printf("\033[41m1\033[0m");
			else
				printf("0");
		printf(" ");
	}
	putchar('\n');
}

uint8_t VoiceLamp(uint8_t data[], uint8_t size, uint8_t off_set, uint8_t max_len_left, uint8_t max_len_right)
{
	if (data == NULL)
		return 0;
	if (max_len_left > off_set || max_len_right > size * 8 - off_set)
		return 0;
	memset(data, 0x00, size);

	// 处理左边
	for (uint8_t i = off_set - 1; i > off_set - max_len_left - 1; i--) 
	{
		data[i / 8] |= 0x80 >> (i % 8);
	}

	// 处理右边
	for (uint8_t i = off_set - 1; i < off_set + max_len_right - 1; i++)
	{
		data[i / 8] |= 0x80 >> (i % 8);
	}
}

uint8_t VoiceLampLeft(uint8_t data[], uint8_t size, uint8_t off_set, uint8_t max_len, bool rise_flag)
{
	if (data == NULL)
		return 0;
	if (max_len > off_set)
		return 0;
	uint8_t i = off_set - max_len - 1;
	if(rise_flag)	// 增长态
	{
		data[i / 8] |= 0x80 >> (i % 8);
	}
	else			// 收回态
	{
		data[(i-1) / 8] &= ~(0x80 >> 7 - ((i-1) % 8));
	}
    return 1;
}

uint8_t VoiceLampRight(uint8_t data[], uint8_t size, uint8_t off_set, uint8_t max_len, bool rise_flag)
{
	if (data == NULL)
		return 0;
	if (max_len > size * 8 - off_set)
		return 0;
	uint8_t i = off_set + max_len - 1;
	if(rise_flag)	// 增长态
	{
		data[i / 8] |= 0x80 >> (i % 8);
	}
	else			// 收回态
	{
		data[(i+1) / 8] &= ~(0x80 >>7 - ((i+1) % 8));
	}
    return 1;
}

int main(int argc, char const *argv[])
{
    uint8_t data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	bool rise_flag = true;
	int group = 1;
	int times = 16;
	// 实机测试
	while(times --)
	{
		if(group > 8)
		{
			rise_flag ^= 1;
			group = 0;
		}
			
		VoiceLampLeft(data, sizeof(data), 30, group, rise_flag);
		// VoiceLampRight(data, sizeof(data), 26, group, rise_flag);
		printf("group = %2d, data = ", group);
		printf2(data, sizeof(data));
		group++;
	}

	
    // 声控灯光函数示例1 (左右单独变化)
	//左侧
	// for(int i = 1; i <= 8; i++)
	// {
	// 	VoiceLampLeft(data, sizeof(data), 36, i, true);
	// 	printf("i = %3d, data = ", i);
	// 	printf2(data, sizeof(data));
	// }

	// for(int i = 8; i >= 1; i--)
	// {
	// 	VoiceLampLeft(data, sizeof(data), 36, i, false);
	// 	printf("i = %3d, data = ", i);
	// 	printf2(data, sizeof(data));
	// }
	// //右侧
	// for(int i = 1; i <= 4; i++)
	// {
	// 	VoiceLampRight(data, sizeof(data), 36, i, true);
	// 	printf("i = %3d, data = ", i);
	// 	printf2(data, sizeof(data));
	// }

	// for(int i = 4; i >= 1; i--)
	// {
	// 	VoiceLampRight(data, sizeof(data), 36, i, false);
	// 	printf("i = %3d, data = ", i);
	// 	printf2(data, sizeof(data));
	// }

	// 声控灯光函数示例2 (左右同时变化)
	// int max_len_left =44;
	// int max_len_right = 44;
	// int off_set = 50;

	// int i = 1, j = 1, x = 0, y = 0;	// ij控制次数，xy控制位置
	// bool rise_flag_left = true, rise_flag_right = true; // 控制左右流水的递增和递减方向

	// while(1)
	// {
    //     // 当左右方向全部归中后正常退出
	// 	if(x < 0 && y < 0)	break;  
    //     // 当左右方向的偏移值大于数组大小限制时异常退出
    //     if((max_len_left > off_set) || (max_len_right > sizeof(data) * 8 - off_set))  break;  
    //     // 当任意一边提前走完时 归零等待另一边也走完  
	// 	if(x <= 0) x = 0;
	// 	if(y <= 0) y = 0;
	// 	VoiceLampLeft(data, sizeof(data), off_set, x, rise_flag_left);
	// 	VoiceLampRight(data, sizeof(data), off_set, y, rise_flag_right);
	// 	printf("x = %2d, y = %2d, data = ", x, y);
	// 	if(i++ > max_len_left)
	// 	{
	// 		x--;
	// 		rise_flag_left = false;
	// 	}
	// 	else
	// 		x++;

	// 	if(j++ > max_len_right)
	// 	{
	// 		y--;
	// 		rise_flag_right = false;
	// 	}
	// 	else
	// 		y++;
	// 	printf2(data, sizeof(data));
	// }

	//声控灯光函数示例3 (单函数双参数,内置数组清零)
	// int led_nums_left = 8;
	// int led_nums_right = 4;
	// int x = 1, y = 1;
	// int i = led_nums_left * 2;
	// bool x_flag = false, y_flag = false;
	// while (i--)
	// {
	// 	// 走到了最左或最右阈值就往回走
	// 	if(x >= led_nums_left)	x_flag = true;
	// 	if(y >= led_nums_right)	y_flag = true;

	// 	if(x <= 0) x = 0;
	// 	if(y <= 0) y = 0;
	// 	printf("x = %2d, y = %2d, data = ", x, y);
	// 	if(!x_flag && !y_flag)
	// 		VoiceLamp(data, sizeof(data), 24, x++, y++);
	// 	else if(x_flag && !y_flag)
	// 		VoiceLamp(data, sizeof(data), 24, x--, y++);
	// 	else if(!x_flag && y_flag)
	// 		VoiceLamp(data, sizeof(data), 24, x++, y--);
	// 	else
	// 		VoiceLamp(data, sizeof(data), 24, x--, y--);
	// 	printf2(data, sizeof(data));
	// }
    return 0;
}
