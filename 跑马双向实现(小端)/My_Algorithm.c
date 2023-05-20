#include "My_Algorithm.h"
uint8_t gl_data[] = {0xff, 0xf0, 0x00, 0x00, 0x00, 0x00,	 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xf0};
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

void copy_bits(char* dst, char* src, int dst_offset, int src_offset, int num_bits) {
    int dst_byte = dst_offset / 8;  // 目标地址的字节偏移
    int dst_bit = dst_offset % 8;   // 目标地址的bit偏移
    int src_byte = src_offset / 8;  // 源地址的字节偏移
    int src_bit = src_offset % 8;   // 源地址的bit偏移

    // 先将目标地址的前面几个bit位清零
    char mask = 0xff << (8 - dst_bit);
     dst[dst_byte] &= mask;

    // 逐bit拷贝数据
    for (int i = 0; i < num_bits; i++) {
        char bit = (src[src_byte] >> (7 - src_bit)) & 0x01;  // 取出源地址当前bit位的值
        dst[dst_byte] |= bit << (7 - dst_bit);  // 将bit值写入目标地址的当前bit位

        // 更新源地址和目标地址的偏移
        src_bit++;
        if (src_bit >= 8) {
            src_bit = 0;
            src_byte++;
        }
        dst_bit++;
        if (dst_bit >= 8) {
            dst_bit = 0;
            dst_byte++;
            // 清零下一个字节的前面几个bit位
            mask = 0xff << (8 - dst_bit);
            dst[dst_byte] &= mask;
        }
    }
}

uint64_t little_to_big(uint64_t num) {
    uint8_t bytes[8];
    for (int i = 0; i < 8; i++) {
        bytes[i] = (uint8_t)(num >> (i * 8));
    }

    uint64_t result = 
        ((uint64_t)bytes[0] << 56) |
        ((uint64_t)bytes[1] << 48) |
        ((uint64_t)bytes[2] << 40) |
        ((uint64_t)bytes[3] << 32) |
        ((uint64_t)bytes[4] << 24) |
        ((uint64_t)bytes[5] << 16) |
        ((uint64_t)bytes[6] << 8) |
        ((uint64_t)bytes[7]);

    return result;
}

uint64_t big_to_little(uint64_t num) {
    uint8_t bytes[8];
    for (int i = 0; i < 8; i++) {
        bytes[i] = (uint8_t)(num >> ((7 - i) * 8));
    }

    uint64_t result = 
        ((uint64_t)bytes[7] << 56) |
        ((uint64_t)bytes[6] << 48) |
        ((uint64_t)bytes[5] << 40) |
        ((uint64_t)bytes[4] << 32) |
        ((uint64_t)bytes[3] << 24) |
        ((uint64_t)bytes[2] << 16) |
        ((uint64_t)bytes[1] << 8) |
        ((uint64_t)bytes[0]);

    return result;
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

		buff_a = little_to_big(buff_a);
		buff_b = little_to_big(buff_b);
		
		// buff_a = big_to_little(buff_a);
		// buff_b = big_to_little(buff_b);

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
	case Close:
	{
		// 处理左半边
		RunningLamp(gl_data, size, led_nums/2, Right, times);

		if(led_nums % 8 == 0)
			RunningLamp(gl_data, size, led_nums/2, Left, times);
		else
		{
			//2.若右部分的最左边不与8对齐，则将其对齐至一个新数组内
			uint8_t temp_array_size = size - led_nums/2;
			uint8_t temp_data[7] = {0};
			uint64_t temp_i64 = 0;
			copy_bits(temp_data, gl_data, 0, led_nums/2, led_nums/2);
			RunningLamp(temp_data, sizeof(temp_data), led_nums/2, Left, times);
			copy_bits(gl_data, temp_data, led_nums/2, 0, led_nums/2);
		}
		break;
	}
	default:
		break;
	break;
	}	// switch end
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


	// 跑马函数示例
	printf("t =    , data = ");
	printf2(gl_data, sizeof(gl_data));

	for (uint8_t t = 0 ; t < 100; t++)
	{
		RunningLamp(gl_data, sizeof(gl_data), 64, Right, 1);
		printf("t = %3d, data = ", t);
		printf2(gl_data, sizeof(gl_data));
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
