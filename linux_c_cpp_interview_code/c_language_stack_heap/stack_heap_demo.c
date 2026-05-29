#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int g_global_value = 100;       // 全局变量：位于全局/静态区。
static int g_static_value = 200; // static 全局变量：只在本文件可见。

static void static_local_demo(void)
{
    // static 局部变量只初始化一次，函数返回后值仍然保留。
    static int count = 0;
    ++count;
    printf("static local count=%d\n", count);
}

static char* make_string_on_heap(const char* text)
{
    // malloc 从堆上申请内存。调用者必须负责 free。
    size_t len = strlen(text);
    char* buffer = (char*)malloc(len + 1);
    if (buffer == NULL) {
        return NULL;
    }

    strcpy(buffer, text);
    return buffer;
}

int main(void)
{
    int local_value = 10;              // 局部变量：通常在栈上。
    const char* literal = "read only"; // 字符串字面量：通常在只读常量区。

    char* heap_text = make_string_on_heap("hello heap");
    if (heap_text == NULL) {
        printf("malloc failed\n");
        return 1;
    }

    printf("global=%d static_global=%d local=%d literal=%s heap=%s\n",
           g_global_value,
           g_static_value,
           local_value,
           literal,
           heap_text);

    static_local_demo();
    static_local_demo();
    static_local_demo();

    // malloc 申请的内存要 free。free 后不要再访问 heap_text 指向的内容。
    free(heap_text);
    heap_text = NULL;

    return 0;
}
