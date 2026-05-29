#include <iostream>
#include <queue>
#include <string>

// queue 是队列，特点是先进先出 FIFO。
// push 从队尾放入，front 访问队头，pop 删除队头。
// 注意：pop 不返回元素，工程里通常先 front() 取值，再 pop() 删除。

struct Packet {
    int id;
    int len;
    std::string data;
};

static void receive_packet(std::queue<Packet>& packets, const Packet& packet)
{
    packets.push(packet);
    std::cout << "receive packet id=" << packet.id
              << ", queue size=" << packets.size() << "\n";
}

static void process_one_packet(std::queue<Packet>& packets)
{
    if (packets.empty()) {
        std::cout << "no packet to process\n";
        return;
    }

    Packet packet = packets.front();
    std::cout << "process packet id=" << packet.id
              << ", len=" << packet.len
              << ", data=" << packet.data << "\n";

    packets.pop();
}

int main()
{
    std::queue<Packet> packets;

    receive_packet(packets, {1, 5, "hello"});
    receive_packet(packets, {2, 4, "ping"});
    receive_packet(packets, {3, 3, "ack"});

    std::cout << "\nfront id=" << packets.front().id
              << ", back id=" << packets.back().id << "\n\n";

    while (!packets.empty()) {
        process_one_packet(packets);
    }

    return 0;
}
