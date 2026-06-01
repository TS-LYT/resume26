# C/C++ 语法面试题训练

这份题库用于训练 C/C++ 基础语法、常见陷阱和高频面试问题。C 语言部分覆盖较深，C++ 部分以基础语法、面向对象和常见易错点为主，不涉及模板、智能指针等高级内容。

建议学习方式：

1. 先自己口述答案。
2. 再看“考点”和“陷阱”。
3. 遇到代码题，先判断能否编译，再判断运行结果是否确定。
4. 高频题要能用自己的话讲清楚。

标记说明：

- `高频`：面试经常问，必须熟。
- `陷阱`：容易答错，重点训练。
- `代码题`：适合手写或现场分析。

## 一、C 语言基础类型

### 1. `char`、`short`、`int`、`long` 的大小固定吗？ `高频`

考点：

- C 标准没有规定这些类型的固定字节数，只规定了相对大小关系。
- 常见 32 位或 64 位 Linux 上：
  - `char` 通常 1 字节。
  - `short` 通常 2 字节。
  - `int` 通常 4 字节。
  - `long` 在 32 位通常 4 字节，在 64 位 Linux 通常 8 字节。

陷阱：

- 不要说 `int` 一定是 4 字节。
- 要根据平台和编译器判断。

### 2. `sizeof(char)` 等于多少？

答案：

```c
sizeof(char) == 1
```

注意：这里的 1 表示 1 个 `char` 单位，不一定等于 8 bit。只是绝大多数平台上 1 byte 为 8 bit。

### 3. `signed char` 和 `unsigned char` 有什么区别？ `高频`

考点：

- `signed char` 可以表示负数。
- `unsigned char` 只表示非负数。
- 普通 `char` 是 signed 还是 unsigned，由编译器实现决定。

陷阱：

```c
char c = 0xff;
if (c == 255) {
}
```

这段代码结果不可靠，因为 `char` 是否有符号取决于平台。

### 4. `float` 和 `double` 有什么区别？

考点：

- `float` 单精度，通常 4 字节。
- `double` 双精度，通常 8 字节。
- 浮点数有精度误差，不能随便用 `==` 比较。

陷阱：

```c
double a = 0.1 + 0.2;
if (a == 0.3) {
}
```

通常不建议这样比较，应使用误差范围。

### 5. `const int *p`、`int const *p`、`int * const p` 区别？ `高频`

答案：

```c
const int *p;      // p 指向的 int 不能通过 p 修改，p 本身可变
int const *p;      // 同上
int * const p;     // p 本身不能变，但可以通过 p 修改指向的 int
const int * const p; // p 本身不能变，指向的 int 也不能通过 p 修改
```

记忆方法：

- `const` 修饰它右边的东西。
- 如果右边没有东西，就修饰左边。

## 二、变量、存储期和作用域

### 6. 局部变量、全局变量、静态变量分别存在哪里？ `高频`

常见回答：

- 局部非静态变量：栈。
- `malloc` 申请的内存：堆。
- 全局变量：全局/静态存储区。
- `static` 变量：全局/静态存储区。
- 字符串字面量：只读常量区，具体由平台实现。

陷阱：

- “栈、堆、全局区、常量区”是常见工程说法，不是 C 标准严格规定的内存模型。

### 7. `static` 修饰局部变量有什么作用？ `高频`

答案：

- 改变变量存储期。
- 静态局部变量只初始化一次。
- 函数返回后变量仍然存在。
- 作用域仍然只在函数内部。

示例：

```c
void test(void)
{
    static int count = 0;
    count++;
    printf("%d\n", count);
}
```

每调用一次，`count` 会继续累加。

### 8. `static` 修饰全局变量和函数有什么作用？

答案：

- 限制链接属性。
- 只能在当前 `.c` 文件内部访问。
- 避免和其他文件中的同名符号冲突。

高频口述：

`static` 修饰全局变量或函数时，表示内部链接；修饰局部变量时，表示静态存储期。

### 9. `extern` 有什么作用？

答案：

- 声明变量或函数在其他文件中定义。
- 用于跨文件访问全局变量或函数。

示例：

```c
extern int g_count;
```

陷阱：

- `extern int a;` 是声明。
- `int a;` 在全局作用域通常是定义。

### 10. 全局变量不初始化时值是多少？

答案：

- 全局变量、静态变量不显式初始化时会被初始化为 0。
- 局部自动变量不初始化时值不确定。

代码题：

```c
int g;

int main(void)
{
    int a;
    static int b;
    printf("%d %d\n", g, b);
    printf("%d\n", a);
}
```

考点：

- `g` 和 `b` 是 0。
- `a` 未初始化，读取是未定义/不确定行为范畴，不能依赖输出。

## 三、指针

### 11. 指针是什么？ `高频`

答案：

指针变量保存的是地址。通过指针可以间接访问某块内存。

示例：

```c
int a = 10;
int *p = &a;
*p = 20;
```

### 12. `*p++`、`(*p)++`、`*++p` 区别？ `高频` `陷阱`

```c
*p++;      // 等价于 *(p++)，先取 *p，再让 p 后移
(*p)++;    // p 不变，让 *p 的值加 1
*++p;      // p 先后移，再取 *p
```

陷阱：

- 后缀 `++` 优先级高于 `*`。
- 但表达式的副作用发生时机也要注意，不要写太复杂的表达式。

### 13. 野指针和悬空指针区别？

答案：

- 野指针：没有初始化，指向未知位置。
- 悬空指针：曾经指向有效内存，但内存已经释放或生命周期结束。

示例：

```c
int *p;        // 野指针

int *q = malloc(sizeof(int));
free(q);       // q 成为悬空指针
q = NULL;      // 好习惯
```

### 14. `NULL` 指针可以 `free` 吗？

答案：

可以。`free(NULL)` 是安全的，不做任何事情。

陷阱：

```c
int *p = malloc(sizeof(int));
free(p);
free(p);       // 错误，重复释放
```

`free(p)` 后最好设置：

```c
p = NULL;
```

### 15. `void *` 指针有什么用？

答案：

`void *` 是通用指针，可以保存任意对象指针，但不能直接解引用，因为不知道指向对象的类型和大小。

示例：

```c
void *p = malloc(100);
int *ip = (int *)p;
```

### 16. 指针和数组有什么关系？ `高频`

数组名在大多数表达式中会转换为指向首元素的指针。

```c
int a[5];
int *p = a;
```

但数组不是指针。

陷阱：

```c
int a[5];
printf("%zu\n", sizeof(a)); // 整个数组大小
printf("%zu\n", sizeof(p)); // 指针大小
```

### 17. 函数参数中的数组会退化成什么？ `高频`

```c
void func(int a[])
{
    printf("%zu\n", sizeof(a));
}
```

函数参数里的 `int a[]` 等价于 `int *a`，所以 `sizeof(a)` 是指针大小，不是数组大小。

### 18. 二级指针常见用途是什么？

常见用途：

- 修改调用者的指针。
- 表示字符串数组。
- 动态二维数组。
- 链表头指针修改。

示例：

```c
void alloc_int(int **pp)
{
    *pp = malloc(sizeof(int));
}
```

调用：

```c
int *p = NULL;
alloc_int(&p);
```

### 19. `int *p[10]` 和 `int (*p)[10]` 区别？ `高频` `陷阱`

```c
int *p[10];    // p 是数组，数组有 10 个元素，每个元素是 int *
int (*p)[10];  // p 是指针，指向含有 10 个 int 的数组
```

记忆：

- `[]` 优先级高于 `*`。
- 有括号时先看括号。

### 20. 函数指针怎么声明？

示例：

```c
int add(int a, int b)
{
    return a + b;
}

int (*fp)(int, int) = add;
int ret = fp(1, 2);
```

考点：

- `fp` 是指针。
- 指向参数为两个 `int`、返回值为 `int` 的函数。

## 四、数组和字符串

### 21. 字符数组和字符串字面量区别？ `高频`

```c
char a[] = "hello";
char *p = "hello";
```

区别：

- `a` 是数组，内容拷贝到数组中，可以修改。
- `p` 指向字符串字面量，通常位于只读区域，不应该修改。

陷阱：

```c
p[0] = 'H'; // 错误，可能崩溃
```

### 22. `sizeof("abc")` 是多少？

答案：

```c
sizeof("abc") == 4
```

因为字符串字面量包含结尾的 `'\0'`。

### 23. `strlen` 和 `sizeof` 区别？ `高频`

```c
char s[] = "hello";
printf("%zu\n", sizeof(s));  // 6
printf("%zu\n", strlen(s));  // 5
```

区别：

- `sizeof` 是编译期计算对象大小，数组时包含 `'\0'`。
- `strlen` 是运行期计算字符串长度，不包含 `'\0'`。

陷阱：

`strlen` 要求字符串必须以 `'\0'` 结尾，否则可能越界读取。

### 24. `strcpy` 有什么风险？

答案：

`strcpy` 不检查目标缓冲区大小，容易缓冲区溢出。

更安全做法：

- 明确目标缓冲区大小。
- 使用 `snprintf`。
- 使用带长度限制的拷贝，并确保结尾 `'\0'`。

示例：

```c
snprintf(dst, sizeof(dst), "%s", src);
```

### 25. 数组越界一定会崩溃吗？ `陷阱`

答案：

不一定。数组越界是未定义行为，可能崩溃，也可能看起来正常，也可能破坏其他变量。

面试要点：

未定义行为不能用某一次运行结果解释为正确。

## 五、函数和参数传递

### 26. C 语言函数参数是值传递还是引用传递？ `高频`

答案：

C 语言只有值传递。

指针参数也是值传递，只是传递的值是地址。

示例：

```c
void change(int x)
{
    x = 100;
}

void change_by_ptr(int *p)
{
    *p = 100;
}
```

### 27. 如何在函数中修改调用者的指针？

需要传二级指针：

```c
void set_ptr(int **pp)
{
    *pp = malloc(sizeof(int));
}
```

### 28. 返回局部变量地址有什么问题？ `高频` `陷阱`

错误示例：

```c
int *func(void)
{
    int a = 10;
    return &a;
}
```

原因：

`a` 是局部变量，函数返回后生命周期结束，返回它的地址会产生悬空指针。

### 29. `inline` 函数一定会内联吗？

答案：

不一定。`inline` 是给编译器的建议，是否真正内联由编译器决定。

### 30. 递归函数有什么风险？

风险：

- 递归太深导致栈溢出。
- 终止条件写错导致无限递归。
- 重复计算导致效率低。

## 六、结构体、联合体和枚举

### 31. 结构体大小怎么算？ `高频`

考点：

- 成员按对齐要求存放。
- 结构体总大小通常要是最大对齐成员大小的整数倍。

示例：

```c
struct A {
    char c;
    int i;
};
```

常见 32 位或 64 位平台上大小通常是 8，不是 5。

陷阱：

- 具体大小受平台、编译器、对齐规则影响。

### 32. 如何减少结构体 padding？

把成员按从大到小排列通常可以减少填充。

```c
struct A {
    int i;
    char c;
    char d;
};
```

### 33. `#pragma pack(1)` 有什么风险？

作用：

- 改变结构体对齐，减少 padding。

风险：

- 某些平台上非对齐访问效率低。
- 某些架构可能直接异常。
- 不能随意用于所有结构体。

### 34. 联合体 union 的特点是什么？

答案：

- 所有成员共享同一块内存。
- 联合体大小至少能容纳最大成员。
- 同一时刻通常只应该按最后写入的成员解释。

示例：

```c
union U {
    int i;
    char c[4];
};
```

### 35. 枚举 enum 的作用是什么？

答案：

枚举用于定义一组命名整数常量，提高代码可读性。

示例：

```c
enum State {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_DONE
};
```

## 七、预处理、宏和 typedef

### 36. 宏函数有什么风险？ `高频` `陷阱`

错误示例：

```c
#define SQUARE(x) x * x
```

调用：

```c
SQUARE(1 + 2)  // 展开为 1 + 2 * 1 + 2，结果不是 9
```

改进：

```c
#define SQUARE(x) ((x) * (x))
```

仍有陷阱：

```c
SQUARE(i++) // i++ 会执行两次
```

### 37. `#define` 和 `const` 区别？

`#define`：

- 预处理阶段文本替换。
- 没有类型检查。
- 不占用普通变量存储。

`const`：

- 有类型。
- 编译器可以检查类型。
- 更适合定义常量对象。

### 38. `typedef` 和 `#define` 定义类型有什么区别？

示例：

```c
#define PINT int *
typedef int *TINT;

PINT a, b;  // a 是 int *，b 是 int
TINT c, d;  // c 和 d 都是 int *
```

这是经典陷阱。

### 39. 头文件为什么要加 include guard？

防止头文件重复包含导致重复定义。

示例：

```c
#ifndef DEMO_H
#define DEMO_H

// declarations

#endif
```

### 40. `#include <stdio.h>` 和 `#include "demo.h"` 区别？

常见规则：

- `<...>` 通常用于系统头文件。
- `"..."` 通常先从当前目录或指定工程目录查找，再查系统路径。

## 八、运算符和表达式陷阱

### 41. `i = i++` 结果是什么？ `高频` `陷阱`

在 C/C++ 中，这类表达式涉及未定义行为或不同标准下规则复杂，不应该写。

面试回答：

不要依赖这种表达式结果。代码中应该拆开写，保证可读性和确定性。

### 42. `a && b` 和 `a & b` 区别？

```c
a && b   // 逻辑与，结果为 0 或 1，有短路
a & b    // 按位与，对每一位运算，无短路
```

### 43. 左移和右移有什么注意点？

注意：

- 对负数移位要谨慎。
- 移位位数不能大于等于类型宽度。
- 有符号整数溢出是未定义行为。

错误示例：

```c
int x = 1 << 32; // 如果 int 是 32 位，这是未定义行为
```

### 44. `++i` 和 `i++` 区别？

- `++i`：先加再使用。
- `i++`：先使用再加。

对于普通整数，单独一行时结果一样：

```c
++i;
i++;
```

对迭代器或对象，前置自增可能更高效。

### 45. 逗号表达式是什么？

```c
int a = (1, 2, 3);
```

结果：

```text
a = 3
```

逗号表达式从左到右求值，整个表达式结果是最后一个表达式的值。

## 九、内存管理

### 46. `malloc` 和 `calloc` 区别？ `高频`

```c
malloc(size)
calloc(n, size)
```

区别：

- `malloc` 只申请内存，不初始化。
- `calloc` 申请 `n * size` 字节，并初始化为 0。

### 47. `realloc` 使用有什么坑？ `高频`

错误示例：

```c
p = realloc(p, new_size);
```

如果失败，返回 `NULL`，原来的 `p` 会丢失，造成内存泄漏。

推荐：

```c
void *tmp = realloc(p, new_size);
if (tmp != NULL) {
    p = tmp;
}
```

### 48. `malloc` 申请 0 字节会怎样？

答案：

实现定义。可能返回 `NULL`，也可能返回一个不能解引用但可以传给 `free` 的指针。

### 49. 内存泄漏、越界、重复释放分别是什么？

内存泄漏：

- 申请后没有释放。

越界：

- 访问了申请范围之外的内存。

重复释放：

- 同一块内存释放两次。

常用工具：

```bash
valgrind --leak-check=full ./app
gcc -fsanitize=address -g app.c -o app
```

### 50. `memcpy` 和 `memmove` 区别？

```c
memcpy(dst, src, n);   // 源和目的不能重叠
memmove(dst, src, n);  // 允许重叠
```

陷阱：

如果源和目的区域重叠，使用 `memcpy` 是未定义行为。

## 十、文件、编译和链接

### 51. 声明和定义有什么区别？ `高频`

声明：

- 告诉编译器有这个符号。

定义：

- 真正分配存储或实现函数。

示例：

```c
extern int g;  // 声明
int g = 10;    // 定义

int add(int, int);      // 函数声明
int add(int a, int b)   // 函数定义
{
    return a + b;
}
```

### 52. 编译过程分几步？ `高频`

常见过程：

```text
预处理 -> 编译 -> 汇编 -> 链接
```

对应：

- 预处理：展开宏、处理头文件、处理条件编译、删除注释。
- 编译：把预处理后的 C/C++ 代码翻译成汇编代码。
- 汇编：把汇编代码转换成目标文件 `.o`。
- 链接：把多个目标文件和库链接成可执行程序。

用 gcc 分步观察：

```bash
gcc -E main.c -o main.i   # 预处理，生成 .i
gcc -S main.i -o main.s   # 编译，生成 .s 汇编文件
gcc -c main.s -o main.o   # 汇编，生成 .o 目标文件
gcc main.o -o app         # 链接，生成可执行程序
```

实际开发中通常一条命令完成：

```bash
gcc main.c -o app
```

它内部仍然会经历预处理、编译、汇编、链接这几个阶段。

各阶段常见问题：

- 预处理阶段：头文件找不到，常见报错是 `No such file or directory`。
- 编译阶段：语法错误、类型错误、函数声明不匹配。
- 汇编阶段：一般较少直接遇到，可能和汇编语法或平台相关。
- 链接阶段：找不到函数或变量定义，常见报错是 `undefined reference`。

常用编译参数：

```bash
gcc -Wall -Wextra -g main.c -o app
```

含义：

- `-Wall`：打开常用警告。
- `-Wextra`：打开更多警告。
- `-g`：生成调试信息，方便 gdb 调试。
- `-O2`：开启优化，常用于 Release。
- `-Iinclude`：添加头文件搜索路径。
- `-Llib`：添加库搜索路径。
- `-lname`：链接库，例如 `-lpthread`。

面试口述：

编译一个 C/C++ 程序通常分为预处理、编译、汇编、链接。预处理负责展开宏和头文件；编译把源码变成汇编；汇编生成 `.o` 目标文件；链接把多个 `.o` 文件和库合成最终可执行文件。头文件找不到通常是预处理问题，语法错误通常是编译问题，`undefined reference` 通常是链接问题。

### 53. 静态库和动态库区别？ `高频`

静态库：

- 链接时拷贝到可执行文件中。
- 可执行文件较大。
- 运行时不依赖对应 `.a` 文件。
- Linux 下常见后缀是 `.a`。

动态库：

- 运行时加载。
- 多进程可共享库代码段。
- 可执行文件较小。
- 部署时要能找到 `.so`。
- Linux 下常见后缀是 `.so`。

假设有下面三个文件：

```text
include/calc.h
src/calc.c
main.c
```

`calc.h`：

```c
#ifndef CALC_H
#define CALC_H

int add(int a, int b);

#endif
```

`calc.c`：

```c
#include "calc.h"

int add(int a, int b)
{
    return a + b;
}
```

`main.c`：

```c
#include <stdio.h>
#include "calc.h"

int main(void)
{
    printf("%d\n", add(1, 2));
    return 0;
}
```

#### 生成静态库 `.a`

第一步：把源文件编译成目标文件：

```bash
gcc -Iinclude -c src/calc.c -o calc.o
```

第二步：用 `ar` 打包成静态库：

```bash
ar rcs libcalc.a calc.o
```

说明：

- 静态库命名通常是 `libxxx.a`。
- 链接时使用 `-lxxx`，不写前缀 `lib`，也不写后缀 `.a`。
- `libcalc.a` 对应链接参数 `-lcalc`。

#### 使用静态库

方式一：直接写库文件路径：

```bash
gcc -Iinclude main.c ./libcalc.a -o app_static
```

方式二：使用 `-L` 和 `-l`：

```bash
gcc -Iinclude main.c -L. -lcalc -o app_static
```

含义：

- `-Iinclude`：指定头文件目录。
- `-L.`：指定库搜索目录为当前目录。
- `-lcalc`：链接 `libcalc.a` 或 `libcalc.so`。

运行：

```bash
./app_static
```

静态库已经被链接进可执行文件，运行时一般不需要再找到原来的 `.a` 文件。

#### 生成动态库 `.so`

第一步：生成位置无关目标文件：

```bash
gcc -Iinclude -fPIC -c src/calc.c -o calc.o
```

第二步：生成动态库：

```bash
gcc -shared -o libcalc.so calc.o
```

也可以一步生成：

```bash
gcc -Iinclude -fPIC -shared src/calc.c -o libcalc.so
```

说明：

- `-fPIC`：生成位置无关代码，动态库通常需要。
- `-shared`：生成共享库。
- 动态库命名通常是 `libxxx.so`。

#### 使用动态库

编译链接：

```bash
gcc -Iinclude main.c -L. -lcalc -o app_shared
```

运行时如果报错：

```text
error while loading shared libraries: libcalc.so: cannot open shared object file
```

说明程序运行时找不到动态库。

临时解决：

```bash
LD_LIBRARY_PATH=. ./app_shared
```

或者：

```bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
./app_shared
```

工程中更常见的做法：

- 把 `.so` 安装到系统库目录，例如 `/usr/lib` 或 `/usr/local/lib`。
- 配置 `/etc/ld.so.conf` 或 `/etc/ld.so.conf.d/` 后执行 `ldconfig`。
- 使用 rpath 指定运行时库路径。

示例：

```bash
gcc -Iinclude main.c -L. -lcalc -Wl,-rpath=. -o app_shared
```

#### 查看可执行程序依赖了哪些动态库

```bash
ldd app_shared
```

#### 静态库和动态库链接顺序陷阱

链接静态库时，库一般要放在使用它的目标文件后面：

```bash
gcc main.o -L. -lcalc -o app
```

不要写成：

```bash
gcc -L. -lcalc main.o -o app
```

某些情况下会导致 `undefined reference`。

面试口述：

静态库一般用 `gcc -c` 生成 `.o`，再用 `ar rcs libxxx.a xxx.o` 打包，链接后代码会进入可执行文件。动态库一般用 `-fPIC` 生成位置无关代码，再用 `gcc -shared` 生成 `libxxx.so`，程序运行时还需要动态链接器能找到这个 `.so`。使用库时头文件靠 `-I` 找，库文件靠 `-L` 指定目录，靠 `-lxxx` 指定库名。

### 54. 头文件里能不能定义全局变量？ `陷阱`

不推荐。

错误示例：

```c
// demo.h
int g_count = 0;
```

多个 `.c` 文件包含后可能导致重复定义。

推荐：

```c
// demo.h
extern int g_count;

// demo.c
int g_count = 0;
```

### 55. 什么是链接错误 undefined reference？

含义：

编译器知道有这个函数或变量声明，但链接阶段找不到它的定义。

常见原因：

- 少链接了某个 `.c` 文件。
- 少链接了库。
- 函数名写错。
- C/C++ 混合编译时名字修饰问题。

## 十一、C++ 基础语法

### 56. C++ 和 C 的主要区别？

回答方向：

- C++ 支持类和对象。
- 支持函数重载。
- 支持引用。
- 支持构造函数和析构函数。
- 支持命名空间。
- 支持异常。
- C++ 更强调封装、继承、多态等面向对象思想。

### 57. 引用和指针有什么区别？ `高频`

引用：

- 定义时必须初始化。
- 一旦绑定，不能再绑定到其他对象。
- 使用时像普通变量。
- 通常不能为 NULL。

指针：

- 可以不初始化，但不推荐。
- 可以改变指向。
- 可以为 NULL。
- 需要通过 `*` 解引用。

示例：

```cpp
int a = 10;
int &r = a;
int *p = &a;
```

### 58. `const` 引用有什么用？

```cpp
void print(const std::string &s);
```

作用：

- 避免拷贝，提高效率。
- 保证函数内部不修改实参。
- 可以绑定临时对象。

### 59. 函数重载是什么？

同一作用域中，函数名相同，但参数列表不同。

示例：

```cpp
void print(int x);
void print(double x);
void print(const char *s);
```

注意：

- 仅返回值不同不能构成重载。

### 60. 默认参数有什么注意点？

示例：

```cpp
void func(int a, int b = 10);
```

注意：

- 默认参数通常只写在声明处。
- 默认参数必须从右往左连续提供。

错误：

```cpp
void func(int a = 1, int b); // 错
```

### 61. `new/delete` 和 `malloc/free` 区别？ `高频`

`new/delete`：

- C++ 运算符。
- 会调用构造函数和析构函数。
- 返回具体类型指针。

`malloc/free`：

- C 标准库函数。
- 只申请和释放原始内存。
- 不调用构造和析构。
- 返回 `void *`。

陷阱：

- `new` 要配 `delete`。
- `new[]` 要配 `delete[]`。
- `malloc` 要配 `free`。
- 不要混用。

### 62. 构造函数和析构函数作用是什么？

构造函数：

- 对象创建时自动调用。
- 用于初始化资源。

析构函数：

- 对象销毁时自动调用。
- 用于释放资源。

示例：

```cpp
class File {
public:
    File() {}
    ~File() {}
};
```

### 63. 拷贝构造函数什么时候调用？

常见场景：

- 用一个对象初始化另一个对象。
- 函数参数按值传递对象。
- 函数按值返回对象时可能调用，现代编译器可能优化。

示例：

```cpp
class A {};

A a;
A b = a;
```

### 64. 深拷贝和浅拷贝区别？ `高频`

浅拷贝：

- 只复制指针值。
- 两个对象指向同一块资源。
- 容易重复释放。

深拷贝：

- 重新分配资源，并复制资源内容。
- 两个对象各自管理自己的资源。

常见追问：

如果类里有裸指针资源，通常需要自己实现拷贝构造、赋值运算符和析构函数。

### 65. 什么是 this 指针？

答案：

`this` 是成员函数中的隐式指针，指向当前对象。

示例：

```cpp
class A {
public:
    void set(int value) {
        this->value = value;
    }
private:
    int value;
};
```

### 66. `class` 和 `struct` 在 C++ 中区别？

主要区别：

- `class` 默认访问权限是 `private`。
- `struct` 默认访问权限是 `public`。

除此之外，C++ 中 `struct` 也可以有成员函数、构造函数、继承等。

### 67. public、protected、private 区别？

- `public`：类外可以访问。
- `protected`：类外不能访问，派生类可以访问。
- `private`：类外不能访问，派生类也不能直接访问。

### 68. 虚函数是什么？ `高频`

虚函数用于实现运行时多态。

示例：

```cpp
class Base {
public:
    virtual void show();
};

class Derived : public Base {
public:
    void show() override;
};
```

通过基类指针或引用调用虚函数时，会根据实际对象类型调用对应版本。

### 69. 析构函数为什么常常要写成 virtual？ `高频` `陷阱`

如果通过基类指针删除派生类对象，基类析构函数必须是虚函数，否则可能只调用基类析构，不调用派生类析构，导致资源泄漏。

示例：

```cpp
class Base {
public:
    virtual ~Base() {}
};
```

### 70. 重载、重写、隐藏有什么区别？

重载 overload：

- 同一作用域。
- 函数名相同。
- 参数列表不同。

重写 override：

- 子类重新实现父类虚函数。
- 函数签名要匹配。

隐藏 hide：

- 子类定义了和父类同名函数，可能隐藏父类同名函数。

### 71. C++ 中 `const` 成员函数是什么？

示例：

```cpp
class A {
public:
    int get() const {
        return value;
    }
private:
    int value;
};
```

含义：

- `const` 成员函数承诺不修改对象的普通成员变量。
- `const` 对象只能调用 `const` 成员函数。

### 72. `static` 成员变量和普通成员变量区别？

普通成员变量：

- 每个对象都有一份。

静态成员变量：

- 属于类，不属于某个对象。
- 所有对象共享一份。
- 类内声明，通常类外定义。

### 73. 命名空间 namespace 有什么作用？

作用：

- 避免命名冲突。
- 组织代码。

示例：

```cpp
namespace net {
    void connect();
}
```

不建议在头文件中写：

```cpp
using namespace std;
```

因为会污染包含该头文件的所有源文件。

### 74. C++ 中 `extern "C"` 有什么作用？ `高频`

作用：

告诉 C++ 编译器按 C 语言方式导出符号名，避免 C++ 名字修饰，常用于 C 和 C++ 混合编程。

示例：

```cpp
extern "C" {
    int add(int a, int b);
}
```

### 75. C++ 异常基础

基本形式：

```cpp
try {
    throw 1;
} catch (int e) {
}
```

面试基础点：

- `throw` 抛出异常。
- `try` 包围可能出错的代码。
- `catch` 捕获异常。
- 析构函数中不要随意抛异常。

## 十二、代码分析题

### 76. 下面代码输出什么？ `代码题` `高频`

```c
int a[] = {1, 2, 3, 4};
int *p = a;
printf("%d\n", *(p + 2));
```

答案：

```text
3
```

考点：数组名退化为首元素指针，`p + 2` 指向 `a[2]`。

### 77. 下面代码有什么问题？ `代码题` `陷阱`

```c
char *p = "hello";
p[0] = 'H';
```

答案：

修改字符串字面量是未定义行为，可能崩溃。

### 78. 下面代码有什么问题？ `代码题`

```c
char buf[8];
strcpy(buf, "hello world");
```

答案：

目标缓冲区不够，发生缓冲区溢出。

### 79. 下面代码输出什么？ `代码题`

```c
char s[] = "abc";
printf("%zu %zu\n", sizeof(s), strlen(s));
```

答案：

```text
4 3
```

### 80. 下面代码有什么问题？ `代码题` `高频`

```c
int *p = malloc(sizeof(int));
*p = 10;
free(p);
printf("%d\n", *p);
```

答案：

`free` 后继续使用 `p`，属于 use-after-free。

### 81. 下面代码有什么问题？

```c
int *p = malloc(10 * sizeof(int));
if (p == NULL) {
    return;
}
for (int i = 0; i <= 10; i++) {
    p[i] = i;
}
free(p);
```

答案：

循环条件应该是 `i < 10`。`i <= 10` 会写 `p[10]`，数组越界。

### 82. 下面代码有没有内存泄漏？ `代码题`

```c
void func(void)
{
    char *p = malloc(100);
    if (do_work() < 0) {
        return;
    }
    free(p);
}
```

答案：

有。`do_work()` 失败时提前返回，`p` 没有释放。

### 83. 下面代码有什么问题？ `代码题`

```c
int *func(void)
{
    int a = 10;
    return &a;
}
```

答案：

返回局部变量地址，函数返回后 `a` 生命周期结束，返回的是悬空指针。

### 84. 下面代码输出一定是 20 吗？ `代码题`

```c
void change(int *p)
{
    int x = 20;
    p = &x;
}

int main(void)
{
    int a = 10;
    int *p = &a;
    change(p);
    printf("%d\n", *p);
}
```

答案：

不是，输出还是 10。因为 C 是值传递，`change` 里修改的是指针形参副本。

### 85. 如何修改上一题，让调用者的指针改变？

答案：

使用二级指针。

```c
void change(int **pp)
{
    static int x = 20;
    *pp = &x;
}
```

### 86. 下面代码有什么问题？ `代码题` `陷阱`

```c
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int i = 1;
int j = 2;
int m = MAX(i++, j++);
```

答案：

宏参数可能被求值多次，`i++` 或 `j++` 可能执行不止一次。

### 87. 下面代码中 `sizeof(a)` 是多少？ `代码题`

```c
void func(int a[10])
{
    printf("%zu\n", sizeof(a));
}
```

答案：

是指针大小，不是 `10 * sizeof(int)`。函数参数中的数组退化为指针。

### 88. 下面 C++ 代码有什么问题？ `代码题` `高频`

```cpp
class Base {
public:
    ~Base() {}
};

class Derived : public Base {
public:
    ~Derived() {}
};

Base *p = new Derived;
delete p;
```

答案：

基类析构函数不是虚函数，通过基类指针删除派生类对象时行为有问题，派生类析构可能不被正确调用。应写成：

```cpp
virtual ~Base() {}
```

### 89. 下面代码能构成重载吗？

```cpp
int func(int a);
double func(int a);
```

答案：

不能。C++ 函数重载不能只靠返回值区分，参数列表必须不同。

### 90. 下面代码有什么问题？

```cpp
int *p = new int[10];
delete p;
```

答案：

`new[]` 必须配 `delete[]`。

正确：

```cpp
delete[] p;
```

## 十三、最高频必练清单

下面这些题建议反复练到能直接口述：

1. `const int *p` 和 `int * const p` 区别。
2. 指针和数组区别。
3. 函数参数数组退化问题。
4. `sizeof` 和 `strlen` 区别。
5. 字符数组和字符串字面量区别。
6. `static` 修饰局部变量、全局变量、函数分别是什么作用。
7. `extern` 声明和定义区别。
8. `malloc/calloc/realloc` 区别和 `realloc` 陷阱。
9. 野指针、悬空指针、内存泄漏、重复释放、越界。
10. 结构体对齐和 padding。
11. 宏函数的副作用问题。
12. 预处理、编译、汇编、链接流程。
13. 静态库和动态库区别。
14. C++ 引用和指针区别。
15. `new/delete` 和 `malloc/free` 区别。
16. 构造函数、析构函数、拷贝构造函数。
17. 深拷贝和浅拷贝。
18. 虚函数和多态。
19. 基类析构函数为什么要 virtual。
20. `extern "C"` 解决什么问题。

## 十四、面试回答习惯

回答语法题时不要只说结论，最好按下面结构：

```text
1. 先说结论。
2. 再解释原因。
3. 指出风险或陷阱。
4. 如果是工程问题，补一句实际写代码时怎么避免。
```

例子：

问题：为什么 `strcpy` 危险？

回答：

```text
strcpy 不检查目标缓冲区大小，如果源字符串比目标数组大，就会写越界，导致内存破坏甚至安全漏洞。实际项目里我会优先使用 snprintf，或者使用带长度限制的拷贝函数，并确保字符串以 '\0' 结尾。
```
