#include <iostream>
#include <limits>
#include <vector>

// 项目 1：传感器采样缓存
// 使用 vector 保存采样值，提前 reserve，减少运行时扩容。

class SensorCache {
public:
    explicit SensorCache(std::size_t max_count)
    {
        samples_.reserve(max_count);
    }

    void add_sample(int value)
    {
        samples_.push_back(value);
    }

    void print() const
    {
        std::cout << "samples: ";
        for (int value : samples_) {
            std::cout << value << " ";
        }
        std::cout << "\n";
    }

    int min() const
    {
        int result = std::numeric_limits<int>::max();
        for (int value : samples_) {
            if (value < result) {
                result = value;
            }
        }
        return result;
    }

    int max() const
    {
        int result = std::numeric_limits<int>::min();
        for (int value : samples_) {
            if (value > result) {
                result = value;
            }
        }
        return result;
    }

    double average() const
    {
        int sum = 0;
        for (int value : samples_) {
            sum += value;
        }
        return samples_.empty() ? 0.0 : static_cast<double>(sum) / samples_.size();
    }

private:
    std::vector<int> samples_;
};

int main()
{
    SensorCache cache(16);

    cache.add_sample(31);
    cache.add_sample(33);
    cache.add_sample(29);
    cache.add_sample(35);
    cache.add_sample(30);

    cache.print();
    std::cout << "min=" << cache.min() << "\n";
    std::cout << "max=" << cache.max() << "\n";
    std::cout << "average=" << cache.average() << "\n";

    return 0;
}
