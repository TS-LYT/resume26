#include <iostream>
#include <stack>
#include <string>

static bool is_pair(char left, char right)
{
    return (left == '(' && right == ')') ||
           (left == '[' && right == ']') ||
           (left == '{' && right == '}');
}

static bool is_valid_parentheses(const std::string& text)
{
    std::stack<char> stack;

    for (char ch : text) {
        if (ch == '(' || ch == '[' || ch == '{') {
            stack.push(ch);
        } else if (ch == ')' || ch == ']' || ch == '}') {
            if (stack.empty()) {
                return false;
            }

            if (!is_pair(stack.top(), ch)) {
                return false;
            }

            stack.pop();
        }
    }

    return stack.empty();
}

int main()
{
    std::string tests[] = {"{[()]}", "{[(])}", "([]){}", "([)]"};

    for (const std::string& text : tests) {
        std::cout << text << " -> "
                  << (is_valid_parentheses(text) ? "true" : "false") << "\n";
    }

    return 0;
}
