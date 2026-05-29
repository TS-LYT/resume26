#include <iostream>
#include <list>
#include <string>

// 项目 2：设备节点管理
// 使用 list 保存设备节点，模拟设备上线、下线、删除。

struct Node {
    int node_id;
    std::string name;
    bool online;
};

class NodeManager {
public:
    void add_node(int node_id, const std::string& name)
    {
        nodes_.push_back({node_id, name, true});
    }

    bool remove_node(int node_id)
    {
        for (auto it = nodes_.begin(); it != nodes_.end(); ++it) {
            if (it->node_id == node_id) {
                nodes_.erase(it);
                return true;
            }
        }
        return false;
    }

    bool set_online(int node_id, bool online)
    {
        for (Node& node : nodes_) {
            if (node.node_id == node_id) {
                node.online = online;
                return true;
            }
        }
        return false;
    }

    void print() const
    {
        for (const Node& node : nodes_) {
            std::cout << "id=" << node.node_id
                      << ", name=" << node.name
                      << ", online=" << (node.online ? "yes" : "no") << "\n";
        }
    }

private:
    std::list<Node> nodes_;
};

int main()
{
    NodeManager manager;

    manager.add_node(1001, "temperature");
    manager.add_node(1002, "humidity");
    manager.add_node(1003, "pressure");

    std::cout << "=== after add ===\n";
    manager.print();

    manager.set_online(1002, false);
    std::cout << "\n=== after 1002 offline ===\n";
    manager.print();

    manager.remove_node(1003);
    std::cout << "\n=== after remove 1003 ===\n";
    manager.print();

    return 0;
}
