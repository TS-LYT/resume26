#include <stdio.h>

// 打印数组，方便观察算法执行后的结果。
static void print_array(const int* nums, int nums_size)
{
    printf("[");
    for (int i = 0; i < nums_size; ++i) {
        printf("%d", nums[i]);
        if (i + 1 < nums_size) {
            printf(", ");
        }
    }
    printf("]\n");
}

// 移动零：把非零元素按原来的相对顺序放到前面，剩余位置补 0。
// nums 是数组首地址，nums_size 是数组元素个数。
static void move_zeroes(int* nums, int nums_size)
{
    int write = 0; // 下一个非零元素应该写入的位置。

    for (int read = 0; read < nums_size; ++read) {
        if (nums[read] != 0) {
            nums[write] = nums[read];
            ++write;
        }
    }

    // write 之后的位置都应该是 0。
    while (write < nums_size) {
        nums[write] = 0;
        ++write;
    }
}

int main(void)
{
    int nums[] = {0, 1, 0, 3, 12};
    int nums_size = (int)(sizeof(nums) / sizeof(nums[0]));

    printf("before: ");
    print_array(nums, nums_size);

    move_zeroes(nums, nums_size);

    printf("after : ");
    print_array(nums, nums_size);

    return 0;
}
