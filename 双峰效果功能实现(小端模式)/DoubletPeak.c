#include "DoubletPeak.h"

// 仅用于调试 输出二进制 只能输出char类型 其他类型要修改
void printf2(char n)
{
	uint8_t i = 0;
	for (i = 0; i < 8; i++)
		if (n & (0x80 >> i))
			printf("\033[41m1\033[0m");
		else
			printf("0");
	// printf(" ");
}

uint8_t DoubletPeak(uint8_t data[], uint8_t size, uint8_t times)
{
	if (data == NULL)
		return 0;
	if (size * 8 < 43)
		return 0;
	int i;

	for (int8_t i = 0; i < times; i++)
	{
		// 控制A峰
		if (i < 21)
			data[i / 8] |= 0x01 << (i % 8);
		if ((i - 5) >= 0 && (i - 5) <= 23)
			data[(i - 5) / 8] &= ~(0x01 << ((i - 5) % 8));

		// 控制B峰
		if (i >= 12 && i < 35)
			data[(i+8) / 8] |= 0x01 << ((i+8) % 8);
		if (i >= 17 && i < 44)
			data[(i+3) / 8] &= ~(0x01 << ((i+3) % 8));
		
		
	}	
	return 1;
}

int main(int argc, char const *argv[])
{
	uint8_t data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};	// 43颗灯
	// printf("data len = %d \n", sizeof(data));

	for (int i = 0; i < 41; i++)
	{
		DoubletPeak(data, sizeof(data), i);
		// 打印二进制
		printf("i = %2d, data = ", i);
		for (uint8_t i = 0; i < sizeof(data); i++)
		{
			// printf("%d ",data[i]);
			printf2(data[i]);
		}
		putchar('\n');
	}

	return 0;
}
