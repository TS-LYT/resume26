#include <deque>
#include <iostream>
#include <string>

// deque 是双端队列。
// 它支持头部和尾部快速插入删除：push_front、push_back、pop_front、pop_back。
// deque 通常不是一整块连续内存，而是分段连续。
// queue 默认底层常用 deque，因为队尾插入和队头删除都很方便。

struct Task {
    int id;
    std::string name;
    bool urgent;
};

static void add_task(std::deque<Task>& tasks, const Task& task)
{
    if (task.urgent) {
        tasks.push_front(task);
    } else {
        tasks.push_back(task);
    }
}

static void print_tasks(const std::deque<Task>& tasks)
{
    std::cout << "tasks: ";
    for (const Task& task : tasks) {
        std::cout << "[" << task.id << "," << task.name
                  << "," << (task.urgent ? "urgent" : "normal") << "] ";
    }
    std::cout << "\n";
}

int main()
{
    std::deque<Task> tasks;

    add_task(tasks, {1, "read_sensor", false});
    add_task(tasks, {2, "upload_log", false});
    add_task(tasks, {3, "alarm_shutdown", true});
    add_task(tasks, {4, "save_config", false});

    print_tasks(tasks);
    std::cout << "front=" << tasks.front().name
              << ", back=" << tasks.back().name << "\n";

    std::cout << "\n=== 从头部依次处理任务 ===\n";
    while (!tasks.empty()) {
        Task task = tasks.front();
        std::cout << "process " << task.name << "\n";
        tasks.pop_front();
    }

    return 0;
}
