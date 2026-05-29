#include <algorithm>
#include <iostream>
#include <vector>

static void rotate(std::vector<int>& nums, int k)
{
    if (nums.empty()) {
        return;
    }

    k %= static_cast<int>(nums.size());
    std::reverse(nums.begin(), nums.end());
    std::reverse(nums.begin(), nums.begin() + k);
    std::reverse(nums.begin() + k, nums.end());
}

int main()
{
    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7};

    rotate(nums, 3);

    for (int value : nums) {
        std::cout << value << " ";
    }
    std::cout << "\n";

    return 0;
}
