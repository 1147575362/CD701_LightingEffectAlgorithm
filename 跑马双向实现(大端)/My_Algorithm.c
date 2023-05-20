#include "My_Algorithm.h"

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

bool RunningLamp(uint8_t data[], uint8_t size, uint8_t led_nums, uint8_t dir, uint8_t times)
{
	if (data == NULL)
		return 0;
	if (led_nums > size * 8)
		return 0;

	uint64_t buff_a = 0, buff_b = 0;

	switch (dir)
	{
	// 左移
	case Left:
	{
		uint8_t i ,j;
		for (i = 0, j = 1; i < led_nums / 8; i++, j++) // 将data放入buffa、buffb中
		{
			uint64_t temp_data = (uint64_t)data[i];
			if(j <= 8)
				buff_a |= (temp_data << (led_nums > 64 ? 64 : led_nums) - j*8);
			else
				buff_b |= temp_data << led_nums - (j-8)*8;
		}
		if(j <= 8)
			buff_a |= (data[i] >> (8 - (led_nums > 64 ? 64 : led_nums) % 8));	// 在buffa中处理剩余未填满的位
		else
			buff_b |= (data[i] >> (8 - led_nums % 8));  						// 在buffb中处理剩余未填满的位

		//  若数组全0则初始化
		if (buff_a == 0 && buff_b == 0)
			(led_nums <= 64) ? (buff_a = 0x01U) : (buff_b = 0x01U);

		// 开始移动
		for (uint8_t i = 0; i < times; i++)
		{
			if (led_nums <= 64) // 灯珠数量64个及以下，只用buff_a存储
			{
				if (buff_a & ((uint64_t)0x01U << led_nums - 1)) // 若最高位为1
				{
					buff_a = buff_a << 1;
					buff_a |= (0x01U << 0); // 将第0位置1
				}
				else
					buff_a = buff_a << 1;
			}
			
			else // 灯珠数量超过了64个，剩余灯珠用buff_b处理
			{
				if (!(buff_a & ((uint64_t)0x01U << 63)) && !(buff_b & ((uint64_t)0x01U << led_nums - 65)))
				{
					buff_a = buff_a << 1;
					buff_b = buff_b << 1;
					continue;
				}
				if (buff_a & ((uint64_t)0x01U << 63) && (buff_b & ((uint64_t)0x01U << led_nums - 65)))
				{
					buff_a = buff_a << 1;
					buff_b = buff_b << 1;
					buff_a |= 0x01U; 
					buff_b |= 0x01U; 
					continue;
				}
				if (buff_a & ((uint64_t)0x01U << 63))
				{
					buff_a = buff_a << 1;
					buff_b = buff_b << 1;
					buff_b |= 0x01U; 
					continue;
				}
				if (buff_b & ((uint64_t)0x01U << led_nums - 65))
				{
					buff_a = buff_a << 1;
					buff_b = buff_b << 1;
					buff_a |= 0x01U; 
				}
			}
		}

		// 写进data数组里
		for (i = 0, j = 1; i < led_nums / 8; i++, j++) // 将data放入buffa、buffb中
		{
			if(j <= 8)
				data[i] = (buff_a >> (led_nums > 64 ? 64 : led_nums) - j*8) & 0xff;
			else
				data[i] = (buff_b >> led_nums - (j-8)*8) & 0xff;
		}
		//把剩余的部分写到末尾
		if(led_nums % 8)	
		{
		//1.将buff中的剩余部分分开
			uint8_t temp_data = 0, temp_data_remain = 0;
			for(int t =  0; t < led_nums % 8; t++)
				temp_data |= 0x01 << t;			// 变化的部分
			for(int t =  0; t < 8 - led_nums % 8; t++)
				temp_data_remain |= 0x01 << t;	// 不变的剩余部分
		//2.将剩余部分的bit左移，放置到data的最后一个字节，并且要保留最后一个字节的其他内容不变
			temp_data_remain &= data[i];
			if(j <= 8)
				data[i] =((buff_a & temp_data) << (8 - led_nums % 8)) | temp_data_remain;
			else
				data[i] =((buff_b & temp_data) << (8 - led_nums % 8)) | temp_data_remain;
		}
		// printf("buff_a = 0x%llx, \t", buff_a);
		// printf("buff_b = 0x%llx\n ", buff_b);
	}
	break;
	// 右移
	case Right:
	{
		uint8_t i ,j;
		for (i = 0, j = 1; i < led_nums / 8; i++, j++) // 将data放入buffa、buffb中
		{
			uint64_t temp_data = (uint64_t)data[i];
			if(j <= 8)
				buff_a |= (temp_data << (led_nums > 64 ? 64 : led_nums) - j*8);
			else
				buff_b |= temp_data << led_nums - (j-8)*8;
		}
		if(j <= 8)
			buff_a |= (data[i] >> (8 - (led_nums > 64 ? 64 : led_nums) % 8));	// 在buffa中处理剩余未填满的位
		else
			buff_b |= (data[i] >> (8 - led_nums % 8));  // 在buffb中处理剩余未填满的位

		//  若数组全0则初始化
		if (buff_a == 0 && buff_b == 0)
			(led_nums <= 64) ? (buff_a = (uint64_t)0x80U << led_nums - 8) : (buff_a = (uint64_t)0x80U << 56);

		//开始移动
		for (uint8_t i = 0; i < times; i++)
		{
			if (led_nums <= 64) // 灯珠数量64个及以下，只用buff_a存储
			{
				if (buff_a & (uint64_t)0x01U) // 若最低位为1	这两处的强转必须加
				{
					buff_a = buff_a >> 1;
					buff_a |= (uint64_t)0x01U << led_nums - 1; // 将最高位置1
				}
				else
					buff_a = buff_a >> 1;
			}
			else // 灯珠数量超过了64个，剩余灯珠用buff_b处理
			{
				if(!(buff_a & 0x01) && !(buff_b & 0x01))
				{
					buff_a = buff_a >> 1;
					buff_b = buff_b >> 1;
					continue;
				}
				if((buff_a & 0x01) && (buff_b & 0x01))
				{
					buff_a = buff_a >> 1;
					buff_b = buff_b >> 1;
					buff_b |= ((uint64_t)0x80U << led_nums - 64 - 8); // 将buff_b最高位置1
					buff_a |= ((uint64_t)0x80U << 56); // 将buff_a最高位置1
					continue;
				}
				if (buff_a & 0x01) // buff_a最低位为1
				{
					buff_a = buff_a >> 1;
					buff_b = buff_b >> 1;
					buff_b |= ((uint64_t)0x80U << led_nums - 64 - 8); // 将buff_b最高位置1
				}
				if (buff_b & 0x01) // 如果buff_b的最低位为1
				{
					buff_a = buff_a >> 1;
					buff_b = buff_b >> 1;
					buff_a |= ((uint64_t)0x80U << 56); // 将buff_a最高位置1
				}

			}
		}

		//写进data数组里
		for (i = 0, j = 1; i < led_nums / 8; i++, j++) // 将data放入buffa、buffb中，类似大端模式
		{
			if(j <= 8)
				data[i] = (buff_a >> (led_nums > 64 ? 64 : led_nums) - j*8) & 0xff;
			else
				data[i] = (buff_b >> led_nums - (j-8)*8) & 0xff;
		}
		//把剩余的部分写到末尾
		if(led_nums % 8)	
		{
		//1.将buff中的剩余部分分开
			uint8_t temp_data = 0, temp_data_remain = 0;
			for(uint8_t t =  0; t < led_nums % 8; t++)
				temp_data |= 0x01 << t;
			for(uint8_t t =  0; t < 8 - led_nums % 8; t++)
				temp_data_remain |= 0x01 << t;
		//2.将剩余部分左移若干位，放置到data的最后一个字节，并且要保留最后一个字节的其他内容不变
			temp_data_remain &= data[i];
			if(j <= 8)

				data[i] =((buff_a & temp_data) << (8 - led_nums % 8)) | temp_data_remain;
			else
				data[i] =((buff_b & temp_data) << (8 - led_nums % 8)) | temp_data_remain;
		}
		// printf("buff_a = 0x%llx, \t", buff_a);
		// printf("buff_b = 0x%llx\n ", buff_b);
	}
	break;
	}
}

uint8_t FlowingLamp(uint8_t data[], uint8_t size, uint8_t led_nums, uint8_t dir, uint8_t times, uint8_t Act)
{
	if (data == NULL)
		return 0;
	if (led_nums > size * 8)
		return 0;
	//	mid用于控制close、diffuse模式的中间位置
	uint8_t mid = led_nums % 2 ? led_nums / 2 + 1 : led_nums / 2;
	switch (dir)
	{
	case Left:
		if (Act == High)
		{
			for (uint8_t i = 0, j = led_nums - 1; i < times; i++, j--) // i控制次数，j控制位置
			{
				if (!(i % led_nums))
				{
					memset(data, 0, size);
					j = led_nums - 1;
				}
				data[j / 8] |= 0x01 << (j % 8);
			}
		}
		else
		{
			memset(data, 0xff, size);								   // 将数组置1
			for (uint8_t i = 0, j = led_nums - 1; i < times; i++, j--) // i控制次数，j控制位置
			{
				if (!(i % led_nums))
				{
					memset(data, 0xff, size);
					j = led_nums - 1;
				}
				data[j / 8] &= ~(0x01 << (j % 8));
			}
		}
		break;
	case Right:
		if (Act == High)
		{
			for (uint8_t i = 0, j = 0; i < times; i++, j++)
			{
				if (!(i % led_nums))
				{
					memset(data, 0, size);
					j = 0;
				}
				data[j / 8] |= 0x01 << (j % 8);
			}
		}
		else
		{
			memset(data, 0xff, size); // 将数组置1
			for (uint8_t i = 0, j = 0; i < times; i++, j++)
			{
				if (!(i % led_nums))
				{
					memset(data, 0xff, size);
					j = 0;
				}
				data[j / 8] &= ~(0x01 << (j % 8));
			}
		}
		break;
	case Close:
		if (Act == High)
		{
			for (uint8_t i = 0, j = 0, k = led_nums - 1; i < times; i++, j++, k--)
			{
				if (!(i % mid))
				{
					memset(data, 0, size);
					j = 0;
					k = led_nums - 1;
				}
				data[j / 8] |= 0x01 << (j % 8);
				data[k / 8] |= 0x01 << (k % 8);
			}
		}
		else
		{
			memset(data, 0xff, size); // 将数组置1
			for (uint8_t i = 0, j = 0, k = led_nums - 1; i < times; i++, j++, k--)
			{
				if (!(i % mid))
				{
					memset(data, 0xff, size); // 将数组置1
					j = 0;
					k = led_nums - 1;
				}
				data[j / 8] &= ~(0x01 << (j % 8));
				data[k / 8] &= ~(0x01 << (k % 8));
			}
		}
		break;
	case Diffuse:
		if (Act == High)
		{
			//	若led个数为奇数则j = led_nums/2
			//	若led个数为偶数则j = led_nums/2 - 1
			for (uint8_t i = 0, j = led_nums % 2 ? led_nums / 2 : led_nums / 2 - 1, k = led_nums / 2; i < times; i++, j--, k++)
			{
				if (!(i % mid))
				{
					memset(data, 0, size);
					j = led_nums % 2 ? led_nums / 2 : led_nums / 2 - 1;
					k = led_nums / 2;
				}
				data[j / 8] |= 0x01 << (j % 8);
				data[k / 8] |= 0x01 << (k % 8);
			}
		}
		else
		{
			memset(data, 0xff, size); // 将数组置1
			for (uint8_t i = 0, j = led_nums % 2 ? led_nums / 2 : led_nums / 2 - 1, k = led_nums / 2; i < times; i++, j--, k++)
			{
				if (!(i % mid))
				{
					memset(data, 0xff, size); // 将数组置1
					j = led_nums % 2 ? led_nums / 2 : led_nums / 2 - 1;
					k = led_nums / 2;
				}
				data[j / 8] &= ~(0x01 << (j % 8));
				data[k / 8] &= ~(0x01 << (k % 8));
			}
		}
		break;
	default:
		break;
	}
}


int main(int argc, char const *argv[])
{
	uint8_t data[] = {0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff    ,0x80, 0x00, 0x00, 0x00, 0x01};

	// 跑马函数示例
	printf("t =   , data = ");
	printf2(data, sizeof(data));

	for (uint8_t t = 0 ; t < 100; t++)
	{
		RunningLamp(data, sizeof(data), 100, Right, 1);
		printf("t = %3d, data = ", t);
		printf2(data, sizeof(data));
	}

	// 流水函数示例
	//  FlowingLamp(data, sizeof(data), 15, Diffuse, 3, High);

	//打印二进制
	// printf("data = ");
	// for (uint8_t i = 0; i < sizeof(data); i++)
	// {
	// 	printf2(data[i]);
	// }
	return 0;
}
