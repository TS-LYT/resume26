#include <stdbool.h>
#include <stdio.h>

#define QUEUE_CAPACITY 5

typedef struct {
    int data[QUEUE_CAPACITY];
    int front; // 队头元素所在位置。
    int rear;  // 下一个元素写入位置。
    int size;  // 当前元素个数。
} CircularQueue;

static void queue_init(CircularQueue* q)
{
    q->front = 0;
    q->rear = 0;
    q->size = 0;
}

static bool queue_empty(const CircularQueue* q)
{
    return q->size == 0;
}

static bool queue_full(const CircularQueue* q)
{
    return q->size == QUEUE_CAPACITY;
}

static bool queue_push(CircularQueue* q, int value)
{
    if (queue_full(q)) {
        return false;
    }

    q->data[q->rear] = value;
    q->rear = (q->rear + 1) % QUEUE_CAPACITY;
    ++q->size;
    return true;
}

static bool queue_pop(CircularQueue* q, int* out_value)
{
    if (queue_empty(q)) {
        return false;
    }

    *out_value = q->data[q->front];
    q->front = (q->front + 1) % QUEUE_CAPACITY;
    --q->size;
    return true;
}

int main(void)
{
    CircularQueue q;
    queue_init(&q);

    queue_push(&q, 10);
    queue_push(&q, 20);
    queue_push(&q, 30);

    int value = 0;
    while (queue_pop(&q, &value)) {
        printf("pop %d\n", value);
    }

    return 0;
}
