#include <iostream>
#include <stack>

class MyQueue {
public:
    void push(int value)
    {
        in_.push(value);
    }

    int pop()
    {
        move_if_needed();
        int value = out_.top();
        out_.pop();
        return value;
    }

    bool empty() const
    {
        return in_.empty() && out_.empty();
    }

private:
    void move_if_needed()
    {
        if (!out_.empty()) {
            return;
        }

        while (!in_.empty()) {
            out_.push(in_.top());
            in_.pop();
        }
    }

    std::stack<int> in_;
    std::stack<int> out_;
};

int main()
{
    MyQueue q;
    q.push(10);
    q.push(20);
    q.push(30);

    while (!q.empty()) {
        std::cout << "pop " << q.pop() << "\n";
    }

    return 0;
}
