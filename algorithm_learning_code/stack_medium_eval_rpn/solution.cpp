#include <iostream>
#include <stack>
#include <string>
#include <vector>

static bool is_operator(const std::string& token)
{
    return token == "+" || token == "-" || token == "*" || token == "/";
}

static int apply_operator(int left, int right, const std::string& op)
{
    if (op == "+") {
        return left + right;
    }
    if (op == "-") {
        return left - right;
    }
    if (op == "*") {
        return left * right;
    }
    return left / right;
}

static int eval_rpn(const std::vector<std::string>& tokens)
{
    std::stack<int> stack;

    for (const std::string& token : tokens) {
        if (!is_operator(token)) {
            stack.push(std::stoi(token));
        } else {
            int right = stack.top();
            stack.pop();
            int left = stack.top();
            stack.pop();

            stack.push(apply_operator(left, right, token));
        }
    }

    return stack.top();
}

int main()
{
    std::vector<std::string> tokens = {"2", "1", "+", "3", "*"};
    std::cout << "answer=" << eval_rpn(tokens) << "\n";
    return 0;
}
