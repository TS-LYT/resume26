#include <stdbool.h>
#include <stdio.h>

#define MAX_SIZE 100

typedef struct {
    int data[MAX_SIZE];
    int top;
} Stack;

typedef struct {
    Stack in;
    Stack out;
} MyQueue;

static void stack_init(Stack* s)
{
    s->top = 0;
}

static bool stack_empty(const Stack* s)
{
    return s->top == 0;
}

static void stack_push(Stack* s, int value)
{
    s->data[s->top] = value;
    ++s->top;
}

static int stack_pop(Stack* s)
{
    --s->top;
    return s->data[s->top];
}

static void queue_init(MyQueue* q)
{
    stack_init(&q->in);
    stack_init(&q->out);
}

static void queue_push(MyQueue* q, int value)
{
    stack_push(&q->in, value);
}

static bool queue_empty(const MyQueue* q)
{
    return stack_empty(&q->in) && stack_empty(&q->out);
}

static int queue_pop(MyQueue* q)
{
    if (stack_empty(&q->out)) {
        while (!stack_empty(&q->in)) {
            stack_push(&q->out, stack_pop(&q->in));
        }
    }

    return stack_pop(&q->out);
}

int main(void)
{
    MyQueue q;
    queue_init(&q);

    queue_push(&q, 10);
    queue_push(&q, 20);
    queue_push(&q, 30);

    while (!queue_empty(&q)) {
        printf("pop %d\n", queue_pop(&q));
    }

    return 0;
}
