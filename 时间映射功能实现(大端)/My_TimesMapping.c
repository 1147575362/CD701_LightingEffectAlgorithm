#include "My_TimesMapping.h"
#define MAX_LINE_LENGTH 106 // 表示读取文件时每行最大的字符数（包括换行符）

//                  0     1    2     3     4      5    6      7    8     9     10    11    12
uint8_t data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

uint8_t** array = NULL; // txt里面的数据存放在这个二维数组里面
uint8_t num_rows = 0;       // 二维数组的行数 0~255
uint16_t num_cols = 0;       // 二维数组的列数 0~65535


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


uint8_t ReadFile(const char* filename, uint8_t*** array, uint8_t* num_rows, uint16_t* num_cols) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Failed to open file: %s\n", filename);
        return -1;
    }

    // 给全局变量array分配内存
    uint8_t** data = NULL;
    uint8_t max_rows = MAX_LINE_LENGTH;
    uint16_t max_cols = MAX_LINE_LENGTH;
    data = (uint8_t**)malloc(max_rows * sizeof(uint8_t*));
    for (uint8_t i = 0; i < max_rows; i++) {
        data[i] = (uint8_t*)malloc(max_cols * sizeof(uint8_t));
    }

    // 一行行的读文件
    uint8_t buffer[MAX_LINE_LENGTH];
    uint8_t row_count = 0;
    while (fgets(buffer, MAX_LINE_LENGTH, fp) != NULL) {
        // 把txt文件中每行最后面的换行符置换成'\0'
        uint8_t length = strlen(buffer);
        if (buffer[length - 1] == '\n') {
            buffer[length - 1] = '\0';
            length--;
        }

        // 将每一行的数据拿出来放入数组对应位置
        uint8_t* token;
        token = strtok(buffer, ",");    //strtok: 将字符串按照指定的分隔符进行分割
        int col_count = 0;
        while (token != NULL) {
            // 确保这个数组有足够的空间存储元素
            if (row_count >= max_rows) {
                max_rows *= 2;
                data = (uint8_t**)realloc(data, max_rows * sizeof(uint8_t*));
                for (uint8_t i = row_count; i < max_rows; i++) {
                    data[i] = (uint8_t*)malloc(max_cols * sizeof(uint8_t));
                }
            }
            if (col_count >= max_cols) {
                max_cols *= 2;
                for (uint8_t i = 0; i < row_count; i++) {
                    data[i] = (uint8_t*)realloc(data[i], max_cols * sizeof(uint8_t));
                }
                data[row_count] = (uint8_t*)malloc(max_cols * sizeof(uint8_t));
            }

            // 将元素存储到数组里
            data[row_count][col_count] = atoi(token);   //atoi: 将字符串转换为整数
            col_count++;
            token = strtok(NULL, ",");                  //第一次传入要分割的字符串, 第二次传入NULL, 会按照上一次分割的位置继续分割
        }

        // 更新列数
        if (col_count > *num_cols) {
            *num_cols = col_count;
        }

        row_count++;
    }

    // 关闭文件
    fclose(fp);

    // 更新行数
    *num_rows = row_count;

    *array = data;

    return 0;
}


uint8_t SetDataFromArray(int current_time)
{
    if(current_time > num_rows)
        return -1;
    uint8_t x;
    // 每一个时刻传进来的数据先将其初始化置0
    memset(data, 0x00, sizeof(data));   
    // 将相应位置的bit位置1
    for(uint8_t i = 1; i < num_cols; i++)
    {
        x = array[current_time-1][i];
        if(x == 0)  break;
        data[(x - 1) / 8] |= 0x80 >> ((x - 1) % 8);
    }
    return 0;
}


int main(int argc, char const *argv[])
{
    //分配内存给二维数组
    array = (uint8_t**)malloc(MAX_LINE_LENGTH * sizeof(uint8_t*));
    for (uint8_t i = 0; i < MAX_LINE_LENGTH; i++) {
        array[i] = (uint8_t*)malloc(MAX_LINE_LENGTH * sizeof(uint8_t));
    }

    // 调用ReadFile函数读txt文件
    if (ReadFile("./map.txt", &array, &num_rows, &num_cols) != 0) {
        return -1;
    }

    // (调试)打印二位数组
    for (uint8_t i = 0; i < num_rows; i++) {
        for (uint8_t j = 0; j < num_cols; j++) {
            printf("%d ", array[i][j]);
        }
        printf("\n");
    }

    // (调试)打印每个时刻的数据
    for(uint8_t i = 1; i <= num_rows; i++)
    {
        SetDataFromArray(i);
        printf2(data,sizeof(data));
    }

    // 释放二维数组内存
    for (uint8_t i = 0; i < MAX_LINE_LENGTH; i++) {
        free(array[i]);
    }
    free(array);

    return 0;
}
