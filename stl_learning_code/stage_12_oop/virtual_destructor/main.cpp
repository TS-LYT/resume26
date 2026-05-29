#include <iostream>
#include <memory>
#include <vector>

// Device 是接口类：只定义统一行为，不关心具体设备怎么实现。
// 纯虚函数 = 0 表示子类必须实现。
class Device {
public:
    virtual ~Device()
    {
        std::cout << "Device destructor\n";
    }

    virtual void open() = 0;
    virtual void close() = 0;
};

class UartDevice : public Device {
public:
    UartDevice()
    {
        std::cout << "UartDevice constructor\n";
    }

    ~UartDevice() override
    {
        std::cout << "UartDevice destructor\n";
    }

    void open() override
    {
        std::cout << "open uart\n";
    }

    void close() override
    {
        std::cout << "close uart\n";
    }
};

class TcpDevice : public Device {
public:
    TcpDevice()
    {
        std::cout << "TcpDevice constructor\n";
    }

    ~TcpDevice() override
    {
        std::cout << "TcpDevice destructor\n";
    }

    void open() override
    {
        std::cout << "connect tcp\n";
    }

    void close() override
    {
        std::cout << "disconnect tcp\n";
    }
};

int main()
{
    std::vector<std::unique_ptr<Device>> devices;
    devices.push_back(std::make_unique<UartDevice>());
    devices.push_back(std::make_unique<TcpDevice>());

    for (const auto& device : devices) {
        device->open();
        device->close();
    }

    std::cout << "leaving main, unique_ptr will delete devices\n";
    return 0;
}
