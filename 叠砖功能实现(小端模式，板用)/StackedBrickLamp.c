#include "StackedBrickLamp.h"

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

uint8_t StackedBrickLamp(uint8_t data[], uint8_t size, uint8_t led_nums, uint8_t dir, uint16_t times, uint8_t bits_lens, uint8_t Act)
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
			for (uint16_t i = 1, j = led_nums; i <= times; i++, j--) // i控制次数， j控制位置
			{
				// 正常移动阶段
				data[(j - 1) / 8] |= 0x01 << ((j - 1) % 8);
				if ((j + bits_lens) <= led_nums)
				{
					data[(j - 1 + bits_lens) / 8] &= ~(0x01 << ((j - 1 + bits_lens) % 8));
				}

                // 叠满了就清零 继续流程
                if (brick_nums >= led_nums) 
                {
                    // memset(data, 0x00, size);
                    // brick_nums = 0;
                    // j = led_nums + 1;
					return 0;
                }

				// j到了该固定的地方了
				if (j == brick_nums + 1)
				{
					brick_nums += bits_lens;
					j = led_nums + 1; // for循环里j还会再-1
				}
			}
		}
		else
		{
			memset(data, 0xff, size);	//得全部置1才看得出效果
			for (uint16_t i = 1, j = led_nums; i <= times; i++, j--) // i控制次数， j控制位置
			{
				// 正常移动阶段
				data[(j - 1) / 8] &= ~(0x01 << ((j - 1) % 8));
				if ((j + bits_lens) <= led_nums)
				{
					data[(j - 1 + bits_lens) / 8] |= 0x01 << ((j - 1 + bits_lens) % 8);
				}

                // 叠满了就置1 继续流程
                if (brick_nums >= led_nums) 
                {
                    // memset(data, 0xff, size);
                    // brick_nums = 0;
                    // j = led_nums + 1;
					return 0;
                }

				// j到了该固定的地方了
				if (j == brick_nums + 1)
				{
					brick_nums += bits_lens;
					printf("brick_nums = %d\n", brick_nums);
					j = led_nums + 1; // for循环里j还会再-1
				}
			}
		}
		break;
	}

	case Right:
	{
        bool reset_flag;
		if (Act == High)
		{
			for (uint16_t i = 1, j = 1; i <= times; i++, j++) // i控制次数， j控制位置
			{
                reset_flag = false;
				// 正常移动阶段
				data[(j - 1) / 8] |= 0x01 << ((j - 1) % 8);
				if ((j - bits_lens) >= 0)
					data[(j - 1 - bits_lens) / 8] &= ~(0x01 << ((j - 1 - bits_lens) % 8));

                // 叠满了就清零 继续流程
                if (brick_nums >= led_nums) 
                {
                    // memset(data, 0x00, size);
                    // brick_nums = 0;
                    // j = 0;
                    // reset_flag = true;
					return 0;
                }

				// j到了该固定的地方了
				if (j % (led_nums - brick_nums) == 0 && reset_flag == false)
				{
					brick_nums += bits_lens;
					j = 0; // for循环里j还会再+1
				}
			}
		}
		else
		{
			memset(data, 0xff, size);	//得全部置1才看得出效果
			for (uint16_t i = 1, j = 1; i <= times; i++, j++) // i控制次数， j控制位置
			{
                reset_flag = false;

				// 正常移动阶段
				data[(j - 1) / 8] &= ~(0x01 << ((j - 1) % 8));
				if ((j - bits_lens) >= 0)
					data[(j - 1 - bits_lens) / 8] |= 0x01 << ((j - 1 - bits_lens) % 8);

                // 叠满了就置1 继续流程
                if (brick_nums >= led_nums) 
                {
                    // memset(data, 0xff, size);
                    // brick_nums = 0;
                    // j = 0;
                    // reset_flag = true;
					return 0;
                }

				// j到了该固定的地方了
				if (j % (led_nums - brick_nums) == 0 && reset_flag == false)
				{
					brick_nums += bits_lens;
					j = 0; // for循环里j还会再+1
				}
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

	/*每次当砖块叠满时，就退出*/
	for(int i = 0; i < 1024; i++)
	{
		// 叠砖函数示例 (小端模式)
		bool ret = StackedBrickLamp(data, sizeof(data), 31, Left, i, 3, High);
		if(ret == 0)
			/*若想循环执行，则在此处置i为0*/
			break;
		// 打印二进制
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
