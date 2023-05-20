#include "My_simplerunning.h"

// 仅用于调试 输出二进制 只能输出char类型 其他类型要修改
void printf2(char n)
{
	uint8_t i = 0;
	for (i = 0; i < 8; i++)
		if (n & (0x80 >> i))
			// printf("1");
			printf("\033[41m1\033[0m");
		else
			printf("0");
	printf("");
}

uint8_t My_simplerunning(uint8_t data[], uint8_t size, uint8_t led_nums, uint8_t dir, uint16_t times, uint8_t bits_lens, uint8_t Act)
{
	if (data == NULL)
		return 0;
	if (led_nums > size * 8)
		return 0;
	uint8_t brick_nums = 0;
	switch (dir)
	{
	case Left:
	{
		if (Act == High)
		{
			for (int16_t i = 1, j = led_nums; i <= times; i++, j--) // i控制次数， j控制位置
			{
				// 正常移动阶段
				data[(j - 1) / 8] |= 0x80 >> ((j - 1) % 8);
				if ((j + bits_lens) <= led_nums)
				{
					data[(j - 1 + bits_lens) / 8] &= ~(0x80 >> ((j - 1 + bits_lens) % 8));
				}
			}
		}
		else
		{
			memset(data, 0xff, size);	//得全部置1才看得出效果
			for (int16_t i = 1, j = led_nums; i <= times; i++, j--) // i控制次数， j控制位置
			{
				// 正常移动阶段
				data[(j - 1) / 8] &= ~(0x80 >> ((j - 1) % 8));
				if ((j + bits_lens) <= led_nums)
				{
					data[(j - 1 + bits_lens) / 8] |= 0x80 >> ((j - 1 + bits_lens) % 8);
				}
			}
		}
		break;
	}

	case Right:
	{
		memset(data, 0x00, size);
		if (Act == High)
		{
			for (int16_t i = 1, j = 1; i <= times; i++, j++) // i控制次数， j控制位置
			{
				// 正常移动阶段
				data[(j - 1) / 8] |= 0x80 >> ((j - 1) % 8);
				if ((j - bits_lens) >= 0)
					data[(j - 1 - bits_lens) / 8] &= ~(0x80 >> ((j - 1 - bits_lens) % 8));
			}
		}
		else
		{
			memset(data, 0xff, size);	//得全部置1才看得出效果
			for (int16_t i = 1, j = 1; i <= times; i++, j++) // i控制次数， j控制位置
			{
				// 正常移动阶段
				data[(j - 1) / 8] &= ~(0x80 >> ((j - 1) % 8));
				if ((j - bits_lens) >= 0)
					data[(j - 1 - bits_lens) / 8] |= 0x80 >> ((j - 1 - bits_lens) % 8);
			}
		}
		break;
	}
	default:
		break;
	}
    return 1;
}
int main(int argc, char const *argv[])
{
	uint8_t data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	for(int i = 0; i < 100; i++)
	{
		// 叠砖函数示例 (大端模式)
		bool ret = My_simplerunning(data, sizeof(data), 100, Right, i, 5, High);
		// if(ret == 0)
		// 	/*若想循环执行，则在此处置i为0*/
		// 	break;
		// 打印二进制
		if(i == 99)
			i = 0;
		printf("i = %3d, ret = %d, data = ", i, ret);
		for (int i = 0; i < sizeof(data); i++)
		{
			// printf("%d ",data[i]);
			printf2(data[i]);
		}
		putchar('\n');
	}

	return 0;
}
