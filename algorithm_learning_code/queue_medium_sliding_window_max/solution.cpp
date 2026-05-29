#include <deque>
#include <iostream>
#include <vector>

static std::vector<int> max_sliding_window(const std::vector<int>& nums, int k)
{
    std::deque<int> q;       // 保存下标。
    std::vector<int> result; // 保存每个窗口最大值。

    for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
        if (!q.empty() && q.front() <= i - k) {
            q.pop_front();
        }

        while (!q.empty() && nums[q.back()] <= nums[i]) {
            q.pop_back();
        }

        q.push_back(i);

        if (i >= k - 1) {
            result.push_back(nums[q.front()]);
        }
    }

    return result;
}

int main()
{
    std::vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    std::vector<int> result = max_sliding_window(nums, 3);

    std::cout << "result: ";
    for (int value : result) {
        std::cout << value << " ";
    }
    std::cout << "\n";

    return 0;
}
