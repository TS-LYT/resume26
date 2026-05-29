#include <iostream>
#include <vector>

static int binary_search(const std::vector<int>& nums, int target)
{
    int left = 0;
    int right = static_cast<int>(nums.size()) - 1;

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

int main()
{
    std::vector<int> nums = {1, 3, 5, 7, 9, 11};

    std::cout << "target 7 index=" << binary_search(nums, 7) << "\n";
    std::cout << "target 8 index=" << binary_search(nums, 8) << "\n";

    return 0;
}
