#include <deque>
#include <iostream>
#include <string>

// 项目 4：双端优先缓存
// 普通任务从尾部进入，紧急任务从头部进入。

struct Task {
    int id;
    std::string name;
    bool urgent;
};

class PriorityBuffer {
public:
    void add_task(const Task& task)
    {
        if (task.urgent) {
            tasks_.push_front(task);
        } else {
            tasks_.push_back(task);
        }
    }

    void run()
    {
        while (!tasks_.empty()) {
            Task task = tasks_.front();
            tasks_.pop_front();

            std::cout << "run task id=" << task.id
                      << ", name=" << task.name
                      << ", urgent=" << (task.urgent ? "yes" : "no") << "\n";
        }
    }

private:
    std::deque<Task> tasks_;
};

int main()
{
    PriorityBuffer buffer;

    buffer.add_task({1, "read_sensor", false});
    buffer.add_task({2, "upload_log", false});
    buffer.add_task({3, "handle_alarm", true});
    buffer.add_task({4, "save_config", false});

    buffer.run();
    return 0;
}
