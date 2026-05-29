#include <stdio.h>

static int binary_search(const int* nums, int nums_size, int target)
{
    int left = 0;
    int right = nums_size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (nums[mid] == target) {
            return mid;
        }

        if (nums[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1;
}

int main(void)
{
    int nums[] = {1, 3, 5, 7, 9, 11};
    int nums_size = (int)(sizeof(nums) / sizeof(nums[0]));

    printf("target 7 index=%d\n", binary_search(nums, nums_size, 7));
    printf("target 8 index=%d\n", binary_search(nums, nums_size, 8));

    return 0;
}
