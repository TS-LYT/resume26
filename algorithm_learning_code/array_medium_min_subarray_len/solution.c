#include <limits.h>
#include <stdio.h>

static int min_sub_array_len(int target, const int* nums, int nums_size)
{
    int left = 0;       // 窗口左边界。
    int sum = 0;        // 当前窗口内元素和。
    int best = INT_MAX; // 当前找到的最短长度。

    for (int right = 0; right < nums_size; ++right) {
        sum += nums[right]; // 右边界扩展，把新元素加入窗口。

        // 当前窗口满足 sum >= target 后，尝试不断缩小左边界。
        while (sum >= target) {
            int len = right - left + 1;
            if (len < best) {
                best = len;
            }

            sum -= nums[left];
            ++left;
        }
    }

    return best == INT_MAX ? 0 : best;
}

int main(void)
{
    int nums[] = {2, 3, 1, 2, 4, 3};
    int nums_size = (int)(sizeof(nums) / sizeof(nums[0]));
    int target = 7;

    int answer = min_sub_array_len(target, nums, nums_size);
    printf("answer=%d\n", answer);

    return 0;
}
