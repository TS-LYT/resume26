#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* make_message_bad(const char* text)
{
    char* buffer = (char*)malloc(strlen(text) + 1);
    if (buffer == NULL) {
        return NULL;
    }

    strcpy(buffer, text);

    // 这个函数把堆内存返回给调用者。
    // 如果调用者忘记 free，就会形成内存泄漏。
    return buffer;
}

static void leak_example(void)
{
    char* msg = make_message_bad("this memory is leaked");
    printf("%s\n", msg);

    // 故意不 free，用于演示 valgrind/asan 如何发现泄漏。
    // 正确写法应该是：
    // free(msg);
}

static void fixed_example(void)
{
    char* msg = make_message_bad("this memory is freed");
    printf("%s\n", msg);
    free(msg);
}

int main(void)
{
    leak_example();
    fixed_example();
    return 0;
}
