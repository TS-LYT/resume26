#include <iostream>
#include <stack>
#include <string>

// stack 是栈，特点是后进先出 LIFO。
// push 入栈，top 查看栈顶，pop 删除栈顶。
// 常用于括号匹配、表达式解析、撤销操作、函数调用栈思想。

static bool is_pair(char left, char right)
{
    return (left == '(' && right == ')') ||
           (left == '[' && right == ']') ||
           (left == '{' && right == '}');
}

static bool brackets_match(const std::string& text)
{
    std::stack<char> left_brackets;

    for (char ch : text) {
        if (ch == '(' || ch == '[' || ch == '{') {
            left_brackets.push(ch);
        } else if (ch == ')' || ch == ']' || ch == '}') {
            if (left_brackets.empty()) {
                return false;
            }
            if (!is_pair(left_brackets.top(), ch)) {
                return false;
            }
            left_brackets.pop();
        }
    }

    return left_brackets.empty();
}

int main()
{
    std::cout << "=== 1. 用 stack 反向输出 ===\n";
    std::stack<int> values;
    values.push(10);
    values.push(20);
    values.push(30);

    while (!values.empty()) {
        std::cout << values.top() << " ";
        values.pop();
    }
    std::cout << "\n";

    std::cout << "\n=== 2. 括号匹配 ===\n";
    std::string ok = "{[()]}";
    std::string bad = "{[(])}";

    std::cout << ok << " -> "
              << (brackets_match(ok) ? "match" : "not match") << "\n";
    std::cout << bad << " -> "
              << (brackets_match(bad) ? "match" : "not match") << "\n";

    std::cout << "\n=== 3. 模拟函数调用栈思想 ===\n";
    std::stack<std::string> call_stack;
    call_stack.push("main");
    call_stack.push("read_config");
    call_stack.push("open_socket");

    while (!call_stack.empty()) {
        std::cout << "return from " << call_stack.top() << "\n";
        call_stack.pop();
    }

    return 0;
}
