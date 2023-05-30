#include "My_DashedLine.h"
Led_Data_t LedData;
bool group_9_flag;  // 因第九组有12bits 所以需要特殊标注处理 
//                  0     1    2     3     4      5    6      7    8     9     10    11    12
uint8_t data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// 仅用于调试 输出二进制 只能输入char类型 其他类型要修改
void printf2(uint8_t data[], uint8_t size)
{
    uint8_t i = 0, j = 0;
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < 8; j++)
            if (data[i] & (0x80 >> j))
                // printf("1");
                printf("\033[41m1\033[0m");
            else
                printf("0");
        printf("");
    }
    putchar('\n');
}

// 仅用于调试 输出二进制 只能输入uint16_t类型 其他类型要修改
void printf2_16(uint16_t n)
{
    uint8_t i = 0;
    for (i = 0; i < 16; i++)
        if (n & (0x8000 >> i))
            // printf("1");
            printf("\033[41m1\033[0m");
        else
            printf("0");
    printf("");
}

bool DashedLineSwimming(uint8_t data[], uint8_t size, uint8_t led_nums, uint8_t dir, uint16_t times)
{
    if (data == NULL)
        return 0;
    if (led_nums > size * 8)
        return 0;

    if (led_nums == 100)
    {
        // 1~8组11bits 9组12bits 从数组中依次取出
        // LedData.group.group_1 = (data[0] << 3) | (data[1] >> 5);
        // LedData.group.group_2 = ((data[2] & 0xfc) >> 2) | ((data[1] & 0x1f) << 6);
        // LedData.group.group_3 = ((data[2] & 0x03) << 9) | (data[3] << 1) | (data[4] >> 7);
        // LedData.group.group_4 = ((data[4] & 0x7f) << 4) | (data[5] >> 4);
        // LedData.group.group_5 = ((data[5] & 0x0f) << 7) | (data[6] >> 1);
        // LedData.group.group_6 = ((data[6] & 0x01) << 10) | (data[7] << 2) | (data[8] >> 6);
        // LedData.group.group_7 = ((data[8] & 0x3f) << 5) | (data[9] >> 3);
        // LedData.group.group_8 = ((data[9] & 0x07) << 8) | (data[10]);
        // LedData.group.group_9 = ((data[11]) << 4) | (data[12] >> 4);

        // return的是uint16_t类型的数据 用11位类型的数据接收 直接截断低11位
        LedData.group.group_1 = u16Swimming_odd(LedData.group.group_1, dir, times);
        LedData.group.group_2 = u16Swimming_even(LedData.group.group_2, dir, times);
        LedData.group.group_3 = u16Swimming_odd(LedData.group.group_3, dir, times);
        LedData.group.group_4 = u16Swimming_even(LedData.group.group_4, dir, times);
        LedData.group.group_5 = u16Swimming_odd(LedData.group.group_5, dir, times);
        LedData.group.group_6 = u16Swimming_even(LedData.group.group_6, dir, times);
        LedData.group.group_7 = u16Swimming_odd(LedData.group.group_7, dir, times);
        LedData.group.group_8 = u16Swimming_even(LedData.group.group_8, dir, times);
        group_9_flag = 1;
        LedData.group.group_9 = u16Swimming_odd(LedData.group.group_9, dir, times); // 第9组有12颗灯 所以需要特殊处理以保持与其他组同步
        group_9_flag = 0;
        // 联合体自动写入有bit位错位 现手动写回
        data[0] = LedData.group.group_1 >> 3;
        data[1] = (LedData.group.group_1 & 0x07) << 5;
        data[1] |= LedData.group.group_2 >> 6;
        data[2] = (LedData.group.group_2 & 0x3f) << 2;
        data[2] |= LedData.group.group_3 >> 9;
        data[3] = (LedData.group.group_3 & 0x1fe) >> 1;
        data[4] = (LedData.group.group_3 & 0x01) << 7;
        data[4] |= LedData.group.group_4 >> 4;
        data[5] = (LedData.group.group_4 & 0x0f) << 4;
        data[5] |= LedData.group.group_5 >> 7;
        data[6] = (LedData.group.group_5 & 0x7f) << 1;
        data[6] |= LedData.group.group_6 >> 10;
        data[7] = (LedData.group.group_6 & 0x3fc) >> 2;
        data[8] = (LedData.group.group_6 & 0x03) << 6;
        data[8] |= LedData.group.group_7 >> 5;
        data[9] = (LedData.group.group_7 & 0x1f) << 3;
        data[9] |= LedData.group.group_8 >> 8;
        data[10] = LedData.group.group_8 & 0xff;
        data[11] = LedData.group.group_9 >> 4;
        data[12] = (LedData.group.group_9 & 0x0f) << 4;
    }
}

uint16_t u16Swimming_odd(uint16_t x, uint8_t dir, uint8_t times)
{
    switch (dir)
    {
    case Left:  //  左←————右
        if(group_9_flag == 0)
        {
            for (uint8_t i = 0; i < times; i++)     //  点亮阶段
                x |= 0x01 << i;
            if (times >= 11)                        //  熄灭阶段
                for (uint8_t j = 0; j < times - 11; j++)
                    x &= ~(0x01 << j);
        }
        else
        {
            for (uint8_t i = 0; i < times + 1; i++) //  点亮阶段
                x |= 0x01 << i;
            if (times >= 11)                        //  熄灭阶段
                for (uint8_t j = 0; j < times - 10; j++)
                    x &= ~(0x01 << j);
        }
        break;

    case Right: //  左————→右
        if(group_9_flag == 0)
        {
            for (uint8_t i = 0; i < times; i++)     //  点亮阶段
                x |= 0x400 >> i;
            if (times >= 11)                        //  熄灭阶段
                for (uint8_t j = 0; j <times - 11; j++)
                    x &= ~(0x400 >> j);
        }
        else
        {
            for (uint8_t i = 0; i < times; i++)     //  点亮阶段
                x |= 0x800 >> i;
            if (times >= 11)                        //  熄灭阶段
                for (uint8_t j = 0; j <= times - 11; j++)
                    x &= ~(0x800 >> j);
        }
        break;
    default:
        break;
    }
    return x;
}

uint16_t u16Swimming_even(uint16_t x, uint8_t dir, uint8_t times)
{
    switch (dir)
    {
    case Left:  //  左←————右
        for (uint8_t j = 0; j < times - 11; j++)        //  点亮阶段
            x |= 0x01 << j;
        if (times <= 11)                                //  熄灭阶段
            for (uint8_t i = 0; i < times; i++)
                x &= ~(0x01 << i);
        break;
    case Right:
        for (uint8_t j = 0; j <times - 11; j++)         //  点亮阶段
            x |= 0x400 >> j;
        if (times <= 11)                                //  熄灭阶段
            for (uint8_t i = 0; i < times; i++)
                x &= ~(0x400 >> i);
    default:
        break;
    }
    return x;
}

int main(int argc, char const *argv[])
{
    for (uint8_t i = 1; i < 23; i++)
    {
        DashedLineSwimming(data, sizeof(data), 100, Right, i);
        // // // 数组打印调试
        printf("i = %2d, data = ", i);
        printf2(data, sizeof(data));

        // 单组打印调试
        // printf("0x%3x, ", LedData.group.group_1);
        // printf2_16(LedData.group.group_1);
        // putchar('\n');
        if (i >= 22)
            i = 0;
        usleep(50000);
    }
    return 0;
}
