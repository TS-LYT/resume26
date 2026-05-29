#include <iostream>
#include <stack>
#include <vector>

static std::vector<int> daily_temperatures(const std::vector<int>& temperatures)
{
    std::vector<int> answer(temperatures.size(), 0);
    std::stack<int> stack; // 保存下标。

    for (int i = 0; i < static_cast<int>(temperatures.size()); ++i) {
        while (!stack.empty() && temperatures[i] > temperatures[stack.top()]) {
            int prev_index = stack.top();
            stack.pop();

            answer[prev_index] = i - prev_index;
        }

        stack.push(i);
    }

    return answer;
}

int main()
{
    std::vector<int> temperatures = {73, 74, 75, 71, 69, 72, 76, 73};
    std::vector<int> answer = daily_temperatures(temperatures);

    std::cout << "answer: ";
    for (int value : answer) {
        std::cout << value << " ";
    }
    std::cout << "\n";

    return 0;
}
