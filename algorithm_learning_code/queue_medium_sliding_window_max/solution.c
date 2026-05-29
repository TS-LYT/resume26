#include <stdio.h>

static void max_sliding_window(const int* nums, int nums_size, int k, int* result, int* result_size)
{
    int deque[100] = {0}; // 保存下标，不直接保存值。
    int front = 0;
    int rear = 0;
    *result_size = 0;

    for (int i = 0; i < nums_size; ++i) {
        // 删除已经离开窗口的队头下标。
        if (front < rear && deque[front] <= i - k) {
            ++front;
        }

        // 保持队列单调递减：队尾对应的值小于等于当前值，就没有保留必要。
        while (front < rear && nums[deque[rear - 1]] <= nums[i]) {
            --rear;
        }

        deque[rear] = i;
        ++rear;

        // i >= k - 1 表示第一个完整窗口已经形成。
        if (i >= k - 1) {
            result[*result_size] = nums[deque[front]];
            ++(*result_size);
        }
    }
}

int main(void)
{
    int nums[] = {1, 3, -1, -3, 5, 3, 6, 7};
    int nums_size = (int)(sizeof(nums) / sizeof(nums[0]));
    int result[100] = {0};
    int result_size = 0;

    max_sliding_window(nums, nums_size, 3, result, &result_size);

    printf("result: ");
    for (int i = 0; i < result_size; ++i) {
        printf("%d ", result[i]);
    }
    printf("\n");

    return 0;
}
