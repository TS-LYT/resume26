#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static bool is_pair(char left, char right)
{
    return (left == '(' && right == ')') ||
           (left == '[' && right == ']') ||
           (left == '{' && right == '}');
}

static bool is_valid_parentheses(const char* text)
{
    char stack[100] = {0};
    int top = 0; // top 指向下一个可写位置，也表示当前栈大小。

    for (int i = 0; text[i] != '\0'; ++i) {
        char ch = text[i];

        if (ch == '(' || ch == '[' || ch == '{') {
            stack[top] = ch;
            ++top;
        } else if (ch == ')' || ch == ']' || ch == '}') {
            if (top == 0) {
                return false;
            }

            --top;
            if (!is_pair(stack[top], ch)) {
                return false;
            }
        }
    }

    return top == 0;
}

int main(void)
{
    const char* tests[] = {"{[()]}", "{[(])}", "([]){}", "([)]"};
    int count = (int)(sizeof(tests) / sizeof(tests[0]));

    for (int i = 0; i < count; ++i) {
        printf("%s -> %s\n",
               tests[i],
               is_valid_parentheses(tests[i]) ? "true" : "false");
    }

    return 0;
}
