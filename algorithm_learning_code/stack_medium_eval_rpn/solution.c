#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int is_operator(const char* token)
{
    return strlen(token) == 1 &&
           (token[0] == '+' || token[0] == '-' || token[0] == '*' || token[0] == '/');
}

static int apply_operator(int left, int right, char op)
{
    if (op == '+') {
        return left + right;
    }
    if (op == '-') {
        return left - right;
    }
    if (op == '*') {
        return left * right;
    }
    return left / right;
}

static int eval_rpn(const char* tokens[], int tokens_size)
{
    int stack[100] = {0};
    int top = 0;

    for (int i = 0; i < tokens_size; ++i) {
        if (!is_operator(tokens[i])) {
            stack[top++] = atoi(tokens[i]);
        } else {
            int right = stack[--top];
            int left = stack[--top];
            stack[top++] = apply_operator(left, right, tokens[i][0]);
        }
    }

    return stack[top - 1];
}

int main(void)
{
    const char* tokens[] = {"2", "1", "+", "3", "*"};
    int tokens_size = (int)(sizeof(tokens) / sizeof(tokens[0]));

    printf("answer=%d\n", eval_rpn(tokens, tokens_size));
    return 0;
}
