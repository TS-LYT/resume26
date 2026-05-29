#include <iostream>
#include <stack>

class MinStack {
public:
    void push(int value)
    {
        data_.push(value);

        if (mins_.empty() || value < mins_.top()) {
            mins_.push(value);
        } else {
            mins_.push(mins_.top());
        }
    }

    void pop()
    {
        data_.pop();
        mins_.pop();
    }

    int top() const
    {
        return data_.top();
    }

    int get_min() const
    {
        return mins_.top();
    }

private:
    std::stack<int> data_;
    std::stack<int> mins_;
};

int main()
{
    MinStack s;
    s.push(3);
    s.push(1);
    s.push(2);

    std::cout << "top=" << s.top() << " min=" << s.get_min() << "\n";
    s.pop();
    std::cout << "top=" << s.top() << " min=" << s.get_min() << "\n";
    s.pop();
    std::cout << "top=" << s.top() << " min=" << s.get_min() << "\n";

    return 0;
}
