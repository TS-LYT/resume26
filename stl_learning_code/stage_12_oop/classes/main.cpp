#include <iostream>
#include <string>

// 类 class 可以理解为“数据 + 操作数据的函数”的封装。
// C 语言里常写 struct + 一组函数，例如 sensor_init(&s)、sensor_print(&s)。
// C++ 类把这些函数放到结构体内部，称为成员函数。

class Sensor {
public:
    // 构造函数：创建对象时自动执行，常用于初始化成员变量。
    Sensor(int id, const std::string& name)
        : id_(id), name_(name), value_(0)
    {
    }

    void update_value(int value)
    {
        value_ = value;
    }

    void print() const
    {
        std::cout << "id=" << id_
                  << ", name=" << name_
                  << ", value=" << value_ << "\n";
    }

private:
    // private 表示外部不能直接访问，必须通过成员函数访问。
    int id_;
    std::string name_;
    int value_;
};

int main()
{
    Sensor sensor(1, "temperature");
    sensor.update_value(31);
    sensor.print();

    return 0;
}
