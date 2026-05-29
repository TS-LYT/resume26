#include <iostream>
#include <vector>
#include <limits>

// vector 可以理解为 C 语言里的“动态数组”：
// 1. 内存通常是连续的，支持 data[i] 这样的随机访问。
// 2. 可以 push_back 在尾部追加元素。
// 3. 容量不够时会自动扩容，但扩容会申请新内存并移动旧数据。
// 4. 嵌入式实时场景要小心运行时扩容，常用 reserve 提前分配空间。

static void print_vector(const std::vector<int>& values)
{
    std::cout << "values: ";
    for (int value : values) {
        std::cout << value << " ";
    }
    std::cout << "\n";
}

static void print_size_capacity(const std::vector<int>& values)
{
    std::cout << "size=" << values.size()
              << ", capacity=" << values.capacity() << "\n";
}

static void print_stat(const std::vector<int>& samples)
{
    if (samples.empty()) {
        std::cout << "no samples\n";
        return;
    }

    int min_value = std::numeric_limits<int>::max();
    int max_value = std::numeric_limits<int>::min();
    int sum = 0;

    for (int sample : samples) {
        if (sample < min_value) {
            min_value = sample;
        }
        if (sample > max_value) {
            max_value = sample;
        }
        sum += sample;
    }

    double average = static_cast<double>(sum) / samples.size();
    std::cout << "min=" << min_value
              << ", max=" << max_value
              << ", average=" << average << "\n";
}

int main()
{
    std::cout << "=== 1. 基本 push_back 和遍历 ===\n";
    std::vector<int> numbers;
    numbers.push_back(10);
    numbers.push_back(20);
    numbers.push_back(30);
    print_vector(numbers);

    std::cout << "\n=== 2. 下标访问 [] 和 at() ===\n";
    numbers[1] = 25;       // [] 不检查越界，速度快，但用错更危险。
    numbers.at(2) = 35;    // at() 会检查越界，越界会抛异常。
    print_vector(numbers);

    std::cout << "\n=== 3. 删除最后一个元素 ===\n";
    if (!numbers.empty()) {
        numbers.pop_back();
    }
    print_vector(numbers);

    std::cout << "\n=== 4. 观察 size 和 capacity 变化 ===\n";
    std::vector<int> grow;
    for (int i = 0; i < 10; ++i) {
        grow.push_back(i);
        std::cout << "push " << i << ": ";
        print_size_capacity(grow);
    }

    std::cout << "\n=== 5. reserve 提前分配空间 ===\n";
    std::vector<int> samples;
    samples.reserve(8);
    print_size_capacity(samples);

    samples.push_back(31);
    samples.push_back(33);
    samples.push_back(29);
    samples.push_back(35);
    samples.push_back(30);
    print_vector(samples);
    print_size_capacity(samples);
    print_stat(samples);

    std::cout << "\n=== 6. resize 和 clear ===\n";
    samples.resize(7); // 扩大 size，新元素是 0。
    print_vector(samples);
    print_size_capacity(samples);

    samples.clear(); // 清空元素，通常不释放 capacity。
    print_size_capacity(samples);

    return 0;
}
