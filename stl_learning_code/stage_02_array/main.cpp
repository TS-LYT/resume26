#include <array>
#include <iostream>

// array 是 C++ 标准库封装的固定大小数组。
// 类似 C 语言 int data[5]，但多了 size()、at()、front()、back() 等接口。
// 大小在编译期确定，不能 push_back，不能动态扩容。
// 嵌入式里，如果数据数量固定，array 比 vector 更可控。

static void print_array(const std::array<int, 5>& samples)
{
    std::cout << "samples: ";
    for (int value : samples) {
        std::cout << value << " ";
    }
    std::cout << "\n";
}

static double average(const std::array<int, 5>& samples)
{
    int sum = 0;
    for (int value : samples) {
        sum += value;
    }
    return static_cast<double>(sum) / samples.size();
}

int main()
{
    std::cout << "=== array 固定传感器采样 ===\n";

    std::array<int, 5> samples = {31, 33, 29, 35, 30};
    print_array(samples);

    std::cout << "size=" << samples.size() << "\n";
    std::cout << "front=" << samples.front() << "\n";
    std::cout << "back=" << samples.back() << "\n";

    samples[2] = 32;     // 不检查越界。
    samples.at(4) = 34;  // 检查越界。
    print_array(samples);

    std::cout << "average=" << average(samples) << "\n";

    return 0;
}
