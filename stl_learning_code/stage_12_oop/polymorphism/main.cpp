#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// 多态的核心：用基类指针或引用，调用派生类重写的虚函数。
// 在工程里，这能让上层逻辑面向统一接口编程。
// 例如不同传感器都实现 read_value()，主循环不用关心具体是哪种传感器。

class Sensor {
public:
    explicit Sensor(std::string name)
        : name_(std::move(name))
    {
    }

    // 基类有虚函数时，析构函数通常也要写成 virtual。
    virtual ~Sensor() = default;

    // = 0 表示纯虚函数，Sensor 变成抽象类，不能直接创建对象。
    virtual int read_value() const = 0;

    virtual void print() const
    {
        std::cout << name_ << " value=" << read_value() << "\n";
    }

protected:
    std::string name_;
};

class TemperatureSensor : public Sensor {
public:
    TemperatureSensor()
        : Sensor("temperature")
    {
    }

    int read_value() const override
    {
        return 31;
    }
};

class HumiditySensor : public Sensor {
public:
    HumiditySensor()
        : Sensor("humidity")
    {
    }

    int read_value() const override
    {
        return 58;
    }
};

int main()
{
    std::vector<std::unique_ptr<Sensor>> sensors;

    // unique_ptr 表示独占所有权，离开作用域会自动 delete。
    // 这里用基类指针保存不同派生类对象。
    sensors.push_back(std::make_unique<TemperatureSensor>());
    sensors.push_back(std::make_unique<HumiditySensor>());

    for (const auto& sensor : sensors) {
        sensor->print(); // 实际调用哪个 read_value，由对象真实类型决定。
    }

    return 0;
}
