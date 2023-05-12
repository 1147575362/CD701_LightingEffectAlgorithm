#include "DoubletPeak.h"

// 仅用于调试 输出二进制 只能输出char类型 其他类型要修改
void printf2(char n)
{
	uint8_t i = 0;
	for (i = 0; i < 8; i++)
		if (n & (0x80 >> i))
			printf("1");
		else
			printf("0");
	// printf(" ");
}

uint8_t DoubletPeak(uint8_t data[], uint8_t size, uint8_t times)
{
	if (data == NULL)
		return 0;
	if (size * 8 < 20)
		return 0;
	int i;

	for (int8_t i = 0; i < times; i++)
	{
		if (i < 10)
			data[i / 8] |= 0x80 >> (i % 8);
		if ((i - 3) >= 0 && (i - 3) <= 12)
			data[(i - 3) / 8] &= ~(0x80 >> ((i - 3) % 8));

		if (i >= 4 && i < 14)
			data[(i+6) / 8] |= 0x80 >> ((i+6) % 8);
		if (i >= 7 && i < 20)
			data[(i+3) / 8] &= ~(0x80 >> ((i+3) % 8));
	}

	return 1;
}

int main(int argc, char const *argv[])
{
	uint8_t data[] = {0x00, 0x00, 0x00};
	// printf("data len = %d \n", sizeof(data));

	for (int i = 0; i < 18; i++)
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
