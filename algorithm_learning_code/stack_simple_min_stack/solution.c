#include <stdio.h>

#define MAX_SIZE 100

typedef struct {
    int data[MAX_SIZE];
    int min_data[MAX_SIZE];
    int top;
} MinStack;

static void min_stack_init(MinStack* s)
{
    s->top = 0;
}

static void min_stack_push(MinStack* s, int value)
{
    s->data[s->top] = value;

    if (s->top == 0 || value < s->min_data[s->top - 1]) {
        s->min_data[s->top] = value;
    } else {
        s->min_data[s->top] = s->min_data[s->top - 1];
    }

    ++s->top;
}

static void min_stack_pop(MinStack* s)
{
    if (s->top > 0) {
        --s->top;
    }
}

static int min_stack_top(const MinStack* s)
{
    return s->data[s->top - 1];
}

static int min_stack_get_min(const MinStack* s)
{
    return s->min_data[s->top - 1];
}

int main(void)
{
    MinStack s;
    min_stack_init(&s);

    min_stack_push(&s, 3);
    min_stack_push(&s, 1);
    min_stack_push(&s, 2);

    printf("top=%d min=%d\n", min_stack_top(&s), min_stack_get_min(&s));
    min_stack_pop(&s);
    printf("top=%d min=%d\n", min_stack_top(&s), min_stack_get_min(&s));
    min_stack_pop(&s);
    printf("top=%d min=%d\n", min_stack_top(&s), min_stack_get_min(&s));

    return 0;
}
