#include <iostream>
#include <vector>

// C++ 版本使用 vector，逻辑和 C 版本完全一样。
// vector 的 size() 返回元素个数，nums[i] 可以像数组一样访问元素。
static void print_array(const std::vector<int>& nums)
{
    std::cout << "[";
    for (std::size_t i = 0; i < nums.size(); ++i) {
        std::cout << nums[i];
        if (i + 1 < nums.size()) {
            std::cout << ", ";
        }
    }
    std::cout << "]\n";
}

static void move_zeroes(std::vector<int>& nums)
{
    std::size_t write = 0;

    for (std::size_t read = 0; read < nums.size(); ++read) {
        if (nums[read] != 0) {
            nums[write] = nums[read];
            ++write;
        }
    }

    while (write < nums.size()) {
        nums[write] = 0;
        ++write;
    }
}

int main()
{
    std::vector<int> nums = {0, 1, 0, 3, 12};

    std::cout << "before: ";
    print_array(nums);

    move_zeroes(nums);

    std::cout << "after : ";
    print_array(nums);

    return 0;
}
