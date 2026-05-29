#include <stdio.h>
#include <stdlib.h>

typedef void (*event_callback_t)(int event_id, void* user_data);

static void change_value_by_pointer(int* p)
{
    // 一级指针可以修改它指向的变量。
    if (p != NULL) {
        *p = 123;
    }
}

static void allocate_by_double_pointer(int** out)
{
    // 二级指针可以修改外部的指针变量。
    *out = (int*)malloc(sizeof(int));
    if (*out != NULL) {
        **out = 456;
    }
}

static void print_array_pointer(int (*arr)[3], int rows)
{
    // arr 是数组指针：每次 arr[i] 代表一整行 int[3]。
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < 3; ++j) {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
}

static void trigger_event(event_callback_t cb, void* user_data)
{
    // 真实工程里，事件循环或中断处理逻辑会在合适时机调用回调。
    if (cb != NULL) {
        cb(1001, user_data);
    }
}

static void on_event(int event_id, void* user_data)
{
    const char* name = (const char*)user_data;
    printf("callback: event=%d user=%s\n", event_id, name);
}

int main(void)
{
    int value = 10;
    change_value_by_pointer(&value);
    printf("value=%d\n", value);

    int* heap_value = NULL;
    allocate_by_double_pointer(&heap_value);
    if (heap_value != NULL) {
        printf("heap_value=%d\n", *heap_value);
        free(heap_value);
    }

    int matrix[2][3] = {
        {1, 2, 3},
        {4, 5, 6}
    };
    print_array_pointer(matrix, 2);

    int a = 1;
    int b = 2;
    int c = 3;
    int* pointer_array[3] = {&a, &b, &c}; // 指针数组：数组元素是指针。
    for (int i = 0; i < 3; ++i) {
        printf("pointer_array[%d]=%d\n", i, *pointer_array[i]);
    }

    trigger_event(on_event, "sensor-module");
    return 0;
}
