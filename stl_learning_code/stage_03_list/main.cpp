#include <iostream>
#include <list>
#include <string>

// list 是双向链表。
// 每个节点保存数据，同时有指向前一个节点和后一个节点的指针。
// 优点：已经找到位置时，插入和删除快。
// 缺点：内存不连续，不能用下标访问，缓存命中率通常不如 vector。

struct DeviceNode {
    int node_id;
    std::string name;
    bool online;
};

static void print_nodes(const std::list<DeviceNode>& nodes)
{
    std::cout << "nodes:\n";
    for (const DeviceNode& node : nodes) {
        std::cout << "  id=" << node.node_id
                  << ", name=" << node.name
                  << ", online=" << (node.online ? "yes" : "no") << "\n";
    }
}

static void set_online(std::list<DeviceNode>& nodes, int node_id, bool online)
{
    for (DeviceNode& node : nodes) {
        if (node.node_id == node_id) {
            node.online = online;
            return;
        }
    }
}

int main()
{
    std::list<DeviceNode> nodes;

    std::cout << "=== 1. 头部和尾部插入 ===\n";
    nodes.push_back({1001, "temperature", true});
    nodes.push_back({1002, "humidity", true});
    nodes.push_front({1000, "gateway", true});
    print_nodes(nodes);

    std::cout << "\n=== 2. insert 在指定位置前插入 ===\n";
    auto it = nodes.begin();
    ++it; // 指向第二个节点。
    nodes.insert(it, {1003, "pressure", false});
    print_nodes(nodes);

    std::cout << "\n=== 3. 修改节点在线状态 ===\n";
    set_online(nodes, 1003, true);
    set_online(nodes, 1002, false);
    print_nodes(nodes);

    std::cout << "\n=== 4. 删除指定节点 ===\n";
    for (auto iter = nodes.begin(); iter != nodes.end(); ++iter) {
        if (iter->node_id == 1002) {
            nodes.erase(iter);
            break; // erase 后 iter 失效，所以这里立刻退出循环。
        }
    }
    print_nodes(nodes);

    std::cout << "\n=== 5. remove_if 按条件删除 ===\n";
    nodes.remove_if([](const DeviceNode& node) {
        return !node.online;
    });
    print_nodes(nodes);

    std::cout << "\nsize=" << nodes.size()
              << ", empty=" << (nodes.empty() ? "yes" : "no") << "\n";

    return 0;
}
