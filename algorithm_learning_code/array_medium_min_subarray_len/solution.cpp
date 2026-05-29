#include <climits>
#include <iostream>
#include <vector>

static int min_sub_array_len(int target, const std::vector<int>& nums)
{
    std::size_t left = 0;
    int sum = 0;
    int best = INT_MAX;

    for (std::size_t right = 0; right < nums.size(); ++right) {
        sum += nums[right];

        while (sum >= target) {
            int len = static_cast<int>(right - left + 1);
            if (len < best) {
                best = len;
            }

            sum -= nums[left];
            ++left;
        }
    }

    return best == INT_MAX ? 0 : best;
}

int main()
{
    std::vector<int> nums = {2, 3, 1, 2, 4, 3};
    int target = 7;

    std::cout << "answer=" << min_sub_array_len(target, nums) << "\n";
    return 0;
}
