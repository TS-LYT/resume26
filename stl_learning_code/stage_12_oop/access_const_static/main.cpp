#include <iostream>
#include <string>

class Sensor {
public:
    Sensor(int id, std::string name)
        : id_(id), name_(std::move(name))
    {
        ++sensor_count_;
    }

    ~Sensor()
    {
        --sensor_count_;
    }

    int id() const
    {
        // const 成员函数承诺不修改当前对象。
        return id_;
    }

    const std::string& name() const
    {
        return name_;
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

    static int sensor_count()
    {
        // static 成员函数没有 this 指针，只能直接访问 static 成员。
        return sensor_count_;
    }

private:
    int id_;
    std::string name_;
    int value_ = 0;

    static int sensor_count_;
};

int Sensor::sensor_count_ = 0;

int main()
{
    std::cout << "count=" << Sensor::sensor_count() << "\n";

    Sensor a(1, "temperature");
    Sensor b(2, "humidity");

    a.update_value(31);
    b.update_value(58);

    const Sensor& ref = a;
    std::cout << "const ref id=" << ref.id() << ", name=" << ref.name() << "\n";

    a.print();
    b.print();
    std::cout << "count=" << Sensor::sensor_count() << "\n";

    return 0;
}
