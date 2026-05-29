#include <stdio.h>

static void reverse_range(int* nums, int left, int right)
{
    while (left < right) {
        int tmp = nums[left];
        nums[left] = nums[right];
        nums[right] = tmp;
        ++left;
        --right;
    }
}

static void rotate(int* nums, int nums_size, int k)
{
    if (nums_size <= 0) {
        return;
    }

    k %= nums_size;
    reverse_range(nums, 0, nums_size - 1);
    reverse_range(nums, 0, k - 1);
    reverse_range(nums, k, nums_size - 1);
}

static void print_array(const int* nums, int nums_size)
{
    for (int i = 0; i < nums_size; ++i) {
        printf("%d ", nums[i]);
    }
    printf("\n");
}

int main(void)
{
    int nums[] = {1, 2, 3, 4, 5, 6, 7};
    int nums_size = (int)(sizeof(nums) / sizeof(nums[0]));

    rotate(nums, nums_size, 3);
    print_array(nums, nums_size);

    return 0;
}
