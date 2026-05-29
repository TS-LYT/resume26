#include <cstring>
#include <iostream>

// 这个例子故意不用 std::string，而是手动管理 char*。
// 目的不是推荐这样写，而是让你看懂构造、析构、拷贝的意义。
// 真实工程里优先用 std::string、std::vector、unique_ptr 等 RAII 类型。

class DeviceName {
public:
    explicit DeviceName(const char* name)
    {
        std::cout << "constructor\n";
        copy_from(name);
    }

    // 拷贝构造：用一个已有对象创建新对象时调用。
    DeviceName(const DeviceName& other)
    {
        std::cout << "copy constructor\n";
        copy_from(other.name_);
    }

    // 拷贝赋值：两个已经存在的对象之间赋值时调用。
    DeviceName& operator=(const DeviceName& other)
    {
        std::cout << "copy assignment\n";

        if (this == &other) {
            return *this;
        }

        delete[] name_;
        copy_from(other.name_);
        return *this;
    }

    ~DeviceName()
    {
        std::cout << "destructor: " << name_ << "\n";
        delete[] name_;
    }

    void print() const
    {
        std::cout << "device name=" << name_ << "\n";
    }

private:
    void copy_from(const char* name)
    {
        std::size_t len = std::strlen(name);
        name_ = new char[len + 1];
        std::strcpy(name_, name);
    }

    char* name_ = nullptr;
};

static void pass_by_value(DeviceName name)
{
    // 按值传参会产生拷贝，所以这里会调用拷贝构造函数。
    name.print();
}

int main()
{
    DeviceName a("sensor-a");
    DeviceName b = a;

    DeviceName c("sensor-c");
    c = a;

    pass_by_value(a);

    std::cout << "main ending\n";
    return 0;
}
