#include <iostream>
#include <string>

// 继承表示“is-a”关系。
// TemperatureSensor 是 Sensor 的一种，所以可以继承 Sensor。
// 基类保存公共属性和公共行为，派生类扩展自己的特殊行为。

class Sensor {
public:
    Sensor(int id, const std::string& name)
        : id_(id), name_(name)
    {
    }

    void print_base_info() const
    {
        std::cout << "id=" << id_ << ", name=" << name_;
    }

protected:
    // protected 表示外部不能访问，但子类可以访问。
    int id_;
    std::string name_;
};

class TemperatureSensor : public Sensor {
public:
    TemperatureSensor(int id, const std::string& name)
        : Sensor(id, name), temperature_(0)
    {
    }

    void set_temperature(int temperature)
    {
        temperature_ = temperature;
    }

    void print() const
    {
        print_base_info();
        std::cout << ", temperature=" << temperature_ << "\n";
    }

private:
    int temperature_;
};

int main()
{
    TemperatureSensor sensor(1, "temp-1");
    sensor.set_temperature(31);
    sensor.print();

    return 0;
}
