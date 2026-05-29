#include <iostream>
#include <queue>
#include <string>

// 项目 3：网络数据包队列
// 使用 queue 模拟网络收包和按顺序处理。

struct Packet {
    int id;
    int len;
    std::string data;
};

class PacketQueue {
public:
    void receive(const std::string& data)
    {
        Packet packet;
        packet.id = next_id_++;
        packet.len = static_cast<int>(data.size());
        packet.data = data;

        packets_.push(packet);
        std::cout << "receive id=" << packet.id << ", data=" << packet.data << "\n";
    }

    void process_all()
    {
        while (!packets_.empty()) {
            Packet packet = packets_.front();
            packets_.pop();

            std::cout << "process id=" << packet.id
                      << ", len=" << packet.len
                      << ", data=" << packet.data << "\n";
        }
    }

private:
    int next_id_ = 1;
    std::queue<Packet> packets_;
};

int main()
{
    PacketQueue queue;

    queue.receive("GET /status");
    queue.receive("POST /config");
    queue.receive("PING");

    std::cout << "\n=== process ===\n";
    queue.process_all();

    return 0;
}
