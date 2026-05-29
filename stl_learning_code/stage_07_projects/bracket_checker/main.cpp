#include <iostream>
#include <stack>
#include <string>

// 项目 5：括号匹配检查
// 支持 () [] {}，常见于表达式解析和配置文本检查。

static bool is_left(char ch)
{
    return ch == '(' || ch == '[' || ch == '{';
}

static bool is_right(char ch)
{
    return ch == ')' || ch == ']' || ch == '}';
}

static bool is_match(char left, char right)
{
    return (left == '(' && right == ')') ||
           (left == '[' && right == ']') ||
           (left == '{' && right == '}');
}

static bool check_brackets(const std::string& text)
{
    std::stack<char> stack;

    for (char ch : text) {
        if (is_left(ch)) {
            stack.push(ch);
        } else if (is_right(ch)) {
            if (stack.empty()) {
                return false;
            }
            if (!is_match(stack.top(), ch)) {
                return false;
            }
            stack.pop();
        }
    }

    return stack.empty();
}

int main()
{
    const std::string tests[] = {
        "{[()]}",
        "{[(])}",
        "config[0] = {value}",
        "config[0 = {value}"
    };

    for (const std::string& text : tests) {
        std::cout << text << " -> "
                  << (check_brackets(text) ? "match" : "not match") << "\n";
    }

    return 0;
}
