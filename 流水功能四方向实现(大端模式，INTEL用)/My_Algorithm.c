#include "My_Algorithm.h"

// 输出二进制 只能输出char类型 其他类型要修改
void printf2(char n)
{
	uint8_t i = 0;
	for (i = 0; i < 8; i++)
		if (n & (0x80 >> i))
			printf("1");
		else
			printf("0");
	printf(" ");
}

int RunningLamp(__UINT8_TYPE__ data[], uint8_t len, uint8_t dir, uint8_t times)
{
	uint64_t buff_a = 0, buff_b = 0;
	uint8_t buff_size = (len * 8 > 100) ? (100) : (len * 8); // 最多100颗灯珠
	printf("buff size = %d\n", buff_size);

	switch (dir)
	{
	// 左移
	case Left:
		for (int i = len - 1, j = 0; i >= 0; i--, j++) // 将data放入buffa、buffb中，类似小端模式
		{
			uint64_t temp_data = (uint64_t)data[i];
			if (j > 7) // buff_a放满了，剩下的放到buff_b里
				buff_b |= (temp_data << ((j - 8) * 8));
			else
				buff_a |= (temp_data << (j * 8));
		}
		//  若数组全0则初始化
		if (buff_a == 0 && buff_b == 0)
			buff_a = 1;
		// 开始移动
		for (uint8_t i = 0; i < times; i++)
		{
			if (buff_size <= 64) // 灯珠数量64个及以下，只用buff_a存储
			{
				if (buff_a & ((uint64_t)0x01U << buff_size - 1)) // 若最高位为1
				{
					buff_a &= ~((uint64_t)0x01U << buff_size - 1); // 将最高位置0
					buff_a = buff_a << 1;
					buff_a |= (0x01U << 0); // 将第0位置1
				}
				else
					buff_a = buff_a << 1;
			}
			else // 灯珠数量超过了64个，剩余灯珠用buff_b处理
			{
				if (buff_a & ((uint64_t)0x01U << 63)) // buff_a最高位为1
				{
					buff_a &= ~((uint64_t)0x01U << 63); // 将buff_a最高位置0
					buff_a = buff_a << 1;
					buff_b = buff_b << 1;
					buff_b |= (0x01U << 0); // 将buff_b第0位置1
				}
				else if (buff_b & ((uint64_t)0x01U << buff_size - 64 - 1)) // 如果buff_b的最高位为1
				{
					buff_b &= ~((uint64_t)0x01U << buff_size - 64 - 1); // 将buff_b的最高位置0
					buff_a = buff_a << 1;
					buff_b = buff_b << 1;
					buff_a |= (0x01U << 0); // 将buff_a第0位置1
				}
				else
				{
					buff_a = buff_a << 1;
					buff_b = buff_b << 1;
				}
			}
		}
		printf("buff_b = 0x%llx, \t", buff_b);
		printf("buff_a = 0x%llx\n ", buff_a);
		// 写进data数组里
		for (int i = len - 1, j = 0; i >= 0; i--, j++)
		{
			if (j > 7)
				data[i] = (buff_b >> (j - 8) * 8) | 0x00; // 将buff放到最低位
			else
				data[i] = (buff_a >> j * 8) | 0x00;
		}
		return 1;
		break;
	// 右移
	case Right:

		for (int i = len - 1, j = 0, k = 7; i >= 0; i--, j++, k--) // 将data放入buffa、buffb中，类似大端模式
		{
			uint64_t temp_data = (uint64_t)data[j];
			if (j > 7) // buff_a放满了，剩下的放到buff_b里
				buff_b |= (temp_data << (i * 8));
			else
				buff_a |= (temp_data << (k * 8));
		}
		//  若数组全0则初始化
		if (buff_a == 0 && buff_b == 0)
			(buff_size < 64) ? (buff_a = (uint64_t)0x80U << buff_size - 8) : (buff_a = (uint64_t)0x80U << 56);
		// 最多只能有100颗灯珠，data大小超过100则置标志位，data数组后4位弃用
		bool overflow_flag = false;
		if (len * 8 > 100)
			overflow_flag = true;

		// 开始移动
		for (uint8_t i = 0; i < times; i++)
		{
			if (buff_size <= 64) // 灯珠数量64个及以下，只用buff_a存储
			{
				if (buff_a & ((uint64_t)0x01U << 0)) // 若最低位为1
				{
					buff_a &= ~((uint64_t)0x01U << 0); // 将最低位置0
					buff_a = buff_a >> 1;
					buff_a |= ((uint64_t)0x01U << buff_size - 1); // 将最高位置1
				}
				else
					buff_a = buff_a >> 1;
			}
			else // 灯珠数量超过了64个，剩余灯珠用buff_b处理
			{
				if (buff_a & ((uint64_t)0x01U << 0)) // buff_a最低位为1
				{
					buff_a &= ~((uint64_t)0x01U << 0); // 将buff_a最低位置0
					buff_a = buff_a >> 1;
					buff_b = buff_b >> 1;
					buff_b |= ((uint64_t)0x80U << len * 8 - 64 - 8); // 将buff_b最高位置1
				}
				else if ((buff_b & ((uint64_t)0x01U << 0)) && !overflow_flag) // 如果buff_b的最低位为1且data未溢出
				{

					buff_b &= ~((uint64_t)0x01U << 0); // 将buff_b的最低位置0
					buff_a = buff_a >> 1;
					buff_b = buff_b >> 1;
					buff_a |= ((uint64_t)0x80U << 56); // 将buff_a最高位置1
				}
				else if ((buff_b & ((uint64_t)0x01U << 4)) && overflow_flag) // 如果buff_b的第4位为1且data溢出
				{
					buff_b &= ~((uint64_t)0x01U << 4); // 将buff_b的第4位置0
					buff_a = buff_a >> 1;
					buff_b = buff_b >> 1;
					buff_a |= ((uint64_t)0x80U << 56); // 将buff_a最高位置1
				}
				else
				{
					buff_a = buff_a >> 1;
					buff_b = buff_b >> 1;
				}
			}
		}
		printf("buff_a = 0x%llx, \t", buff_a);
		printf("buff_b = 0x%llx\n ", buff_b);
		// 写进data数组里
		for (int i = len - 1, j = 0, k = 7; i >= 0; i--, j++, k--)
		{
			if (j > 7)
				data[j] = (buff_b >> i * 8) | 0x00;
			else
				data[j] = (buff_a >> k * 8) | 0x00;
		}
		return 1;
		break;
	case Close:
		if (!(len % 2)) // len为偶数
		{
			for (uint8_t i = 0, j = len / 2, k = len / 2 - 1; i < len / 2; i++, j++, k--)
			{
				uint64_t temp_data_a = (uint64_t)data[i];
				uint64_t temp_data_b = (uint64_t)data[j];
				buff_a |= (temp_data_a << (k * 8));
				buff_b |= (temp_data_b << (k * 8));
			}
		}
		else
		{
			for (uint8_t i = 0, j = len / 2 + 1, k = len / 2 - 1; i < len / 2; i++, j++, k--)
			{
				uint64_t temp_data_a = (uint64_t)data[i];
				uint64_t temp_data_b = (uint64_t)data[j];
				buff_a |= (temp_data_a << (k * 8) + 4);
				buff_b |= (temp_data_b << (k * 8));
			}
			uint64_t temp_data_mid = (uint64_t)data[len / 2];  // mid取数组的中间8bit
			buff_a |= (temp_data_mid >> 4);					   // mid的左4位给buffa
			buff_b |= (temp_data_mid & 0x0f) << (len / 2 * 8); // mid的右4位给buffb
		}
		//  若数组全0则初始化
		if (buff_a == 0 && buff_b == 0)
		{
			if (!(len % 2))
				buff_a |= (uint64_t)0x80 << ((len / 2 - 1) * 8);
			else
				buff_a |= (uint64_t)0x80 << (len / 2 * 8 - 4);
			buff_b |= 0x01;
		}
		// 开始移动
		for (uint8_t i = 0; i < times; i++)
		{
			if (buff_a & ((uint64_t)0x01U << 0)) // 若buff_a最低位为1
			{
				buff_a &= ~((uint64_t)0x01U << 0); // 将buff_a最低位置0
				buff_a = buff_a >> 1;
				buff_a |= ((uint64_t)0x01U << buff_size / 2 - 1); // 将buff_a最高位置1

				buff_b &= ~((uint64_t)0x01U << buff_size / 2 - 1); // 将buff_b最高位置0
				buff_b = buff_b << 1;
				buff_b |= ((uint64_t)0x01U << 0); // 将buff_b最低位置1
			}
			else
			{
				buff_a = buff_a >> 1;
				buff_b = buff_b << 1;
			}
		}
		//  数据写回
		if (!(len % 2)) // len为偶数
		{
			for (int i = 0, j = len / 2, k = len / 2 - 1; i < len / 2; i++, j++, k--)
			{
				data[i] = (buff_a >> k * 8) | 0x00;
				data[j] = (buff_b >> k * 8) | 0x00;
			}
		}
		else
		{
			for (uint8_t i = 0, j = len / 2 + 1, k = len / 2; i < len / 2; i++, j++, k--)
			{
				data[i] = (buff_a >> (k * 8 - 4)) | 0x00;
				data[j] = (buff_b >> (k - 1) * 8) | 0x00;
			}
			//  将buff_a的低4位和buff_b的高4位拼接成8位的temp_data_mid
			uint8_t temp_data_mid = 0;
			temp_data_mid |= (buff_a & 0x0f) << 4;		// 提取buff_a的低4位给temp_data_mid的高4位
			temp_data_mid |= (buff_b >> (len / 2 * 8)); // 提取buff_b的高4位给temp_data_mid的低4位
			data[len / 2] = temp_data_mid;
		}
		printf("buff_a = 0x%llx, \t", buff_a);
		printf("buff_b = 0x%llx\n ", buff_b);
		break;
	case Diffuse:
		if (!(len % 2)) // len为偶数
		{
			for (uint8_t i = 0, j = len / 2, k = len / 2 - 1; i < len / 2; i++, j++, k--)
			{
				uint64_t temp_data_a = (uint64_t)data[i];
				uint64_t temp_data_b = (uint64_t)data[j];
				buff_a |= (temp_data_a << (k * 8));
				buff_b |= (temp_data_b << (k * 8));
			}
		}
		else
		{
			for (uint8_t i = 0, j = len / 2 + 1, k = len / 2 - 1; i < len / 2; i++, j++, k--)
			{
				uint64_t temp_data_a = (uint64_t)data[i];
				uint64_t temp_data_b = (uint64_t)data[j];
				buff_a |= (temp_data_a << (k * 8) + 4);
				buff_b |= (temp_data_b << (k * 8));
			}
			uint64_t temp_data_mid = (uint64_t)data[len / 2];  // mid取数组的中间8bit
			buff_a |= (temp_data_mid >> 4);					   // mid的左4位给buffa
			buff_b |= (temp_data_mid & 0x0f) << (len / 2 * 8); // mid的右4位给buffb
		}
		//  若数组全0则初始化
		if (buff_a == 0 && buff_b == 0)
		{
			if (!(len % 2))
				buff_b |= (uint64_t)0x80 << ((len / 2 - 1) * 8);
			else
				buff_b |= (uint64_t)0x80 << (len / 2 * 8 - 4);
			buff_a |= 0x01;
		}
		// 开始移动
		for (uint8_t i = 0; i < times; i++)
		{
			if (buff_b & ((uint64_t)0x01U << 0)) // 若buff_b最低位为1
			{
				buff_b &= ~((uint64_t)0x01U << 0); // 将buff_b最低位置0
				buff_b = buff_b >> 1;
				buff_b |= ((uint64_t)0x01U << buff_size / 2 - 1); // 将buff_a最高位置1

				buff_a &= ~((uint64_t)0x01U << buff_size / 2 - 1); // 将buff_a最高位置0
				buff_a = buff_a << 1;
				buff_a |= ((uint64_t)0x01U << 0); // 将buff_a最低位置1
			}
			else
			{
				buff_a = buff_a << 1;
				buff_b = buff_b >> 1;
			}
		}

		//  数据写回
		if (!(len % 2)) // len为偶数
		{
			for (uint8_t i = 0, j = len / 2, k = len / 2 - 1; i < len / 2; i++, j++, k--)
			{
				data[i] = (buff_a >> k * 8) | 0x00;
				data[j] = (buff_b >> k * 8) | 0x00;
			}
		}
		else
		{
			for (uint8_t i = 0, j = len / 2 + 1, k = len / 2; i < len / 2; i++, j++, k--)
			{
				data[i] = (buff_a >> (k * 8 - 4)) | 0x00;
				data[j] = (buff_b >> (k - 1) * 8) | 0x00;
			}
			//  将buff_a的低4位和buff_b的高4位拼接成8位的temp_data_mid
			uint8_t temp_data_mid = 0;
			temp_data_mid |= (buff_a & 0x0f) << 4;		// 提取buff_a的低4位给temp_data_mid的高4位
			temp_data_mid |= (buff_b >> (len / 2 * 8)); // 提取buff_b的高4位给temp_data_mid的低4位
			data[len / 2] = temp_data_mid;
		}
		printf("buff_a = 0x%llx, \t", buff_a);
		printf("buff_b = 0x%llx\n ", buff_b);
		break;
	default:
		break;
	}
}

int FlowingLamp(__UINT8_TYPE__ data[], uint8_t size, uint8_t led_nums, uint8_t dir, uint8_t times, uint8_t Act)
{
	if (data == NULL)
		return 0;
	if (led_nums > size * 8)
		return 0;
	//	mid用于控制close、diffuse模式的中间位置
	int mid = led_nums % 2 ? led_nums / 2 + 1 : led_nums / 2;
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
				data[j / 8] |= 0x80 >> (j % 8);
			}
		}
		else
		{
			memset(data, 0xff, size);							   // 将数组置1
			for (uint8_t i = 0, j = led_nums - 1; i < times; i++, j--) // i控制次数，j控制位置
			{
				if (!(i % led_nums))
				{
					memset(data, 0xff, size);
					j = led_nums - 1;
				}
				data[j / 8] &= ~(0x80 >> (j % 8));
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
				data[j / 8] |= 0x80 >> (j % 8);
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
				data[j / 8] &= ~(0x80 >> (j % 8));
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
				data[j / 8] |= 0x80 >> (j % 8);
				data[k / 8] |= 0x80 >> (k % 8);
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
				data[j / 8] &= ~(0x80 >> (j % 8));
				data[k / 8] &= ~(0x80 >> (k % 8));
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
				data[j / 8] |= 0x80 >> (j % 8);
				data[k / 8] |= 0x80 >> (k % 8);
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
				data[j / 8] &= ~(0x80 >> (j % 8));
				data[k / 8] &= ~(0x80 >> (k % 8));
			}
		}
		break;
	default:
		break;
	}
}

int main(int argc, char const *argv[])
{
	uint8_t data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	printf("data len = %d \n", sizeof(data));

	// 跑马函数示例
	//RunningLamp(data, sizeof(data), Right, 79);

	// 流水函数示例
	FlowingLamp(data, sizeof(data), 55, Close, 2, High);

	// 打印二进制
	printf("data = ");
	for (uint8_t i = 0; i < sizeof(data); i++)
	{
		// printf("%d ",data[i]);
		printf2(data[i]);
	}
	return 0;
}
