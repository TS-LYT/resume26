#include <array>
#include <iostream>

class CircularQueue {
public:
    bool push(int value)
    {
        if (full()) {
            return false;
        }

        data_[rear_] = value;
        rear_ = (rear_ + 1) % data_.size();
        ++size_;
        return true;
    }

    bool pop(int& out_value)
    {
        if (empty()) {
            return false;
        }

        out_value = data_[front_];
        front_ = (front_ + 1) % data_.size();
        --size_;
        return true;
    }

    bool empty() const
    {
        return size_ == 0;
    }

    bool full() const
    {
        return size_ == data_.size();
    }

private:
    std::array<int, 5> data_ {};
    std::size_t front_ = 0;
    std::size_t rear_ = 0;
    std::size_t size_ = 0;
};

int main()
{
    CircularQueue q;
    q.push(10);
    q.push(20);
    q.push(30);

    int value = 0;
    while (q.pop(value)) {
        std::cout << "pop " << value << "\n";
    }

    return 0;
}
