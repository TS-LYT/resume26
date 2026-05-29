#include <stdio.h>

static void daily_temperatures(const int* temperatures, int size, int* answer)
{
    int stack[100] = {0}; // 保存下标。
    int top = 0;

    for (int i = 0; i < size; ++i) {
        answer[i] = 0;

        // 当前温度更高，说明栈顶那天找到了答案。
        while (top > 0 && temperatures[i] > temperatures[stack[top - 1]]) {
            int prev_index = stack[top - 1];
            --top;

            answer[prev_index] = i - prev_index;
        }

        stack[top] = i;
        ++top;
    }
}

int main(void)
{
    int temperatures[] = {73, 74, 75, 71, 69, 72, 76, 73};
    int size = (int)(sizeof(temperatures) / sizeof(temperatures[0]));
    int answer[100] = {0};

    daily_temperatures(temperatures, size, answer);

    printf("answer: ");
    for (int i = 0; i < size; ++i) {
        printf("%d ", answer[i]);
    }
    printf("\n");

    return 0;
}
