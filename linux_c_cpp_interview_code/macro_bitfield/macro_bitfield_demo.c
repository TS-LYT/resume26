#include <stdint.h>
#include <stdio.h>

#define ARRAY_SIZE(arr) ((int)(sizeof(arr) / sizeof((arr)[0])))
#define BIT(n) (1U << (n))
#define SET_FLAG(value, flag) ((value) |= (flag))
#define CLEAR_FLAG(value, flag) ((value) &= ~(flag))
#define HAS_FLAG(value, flag) (((value) & (flag)) != 0)

#define LOG_INFO(fmt, ...) \
    printf("[INFO] " fmt "\n", ##__VA_ARGS__)

// 函数式宏要给参数加括号，减少运算优先级问题。
// 但它仍然有副作用风险，比如 SQUARE(i++) 会让 i 自增两次。
#define SQUARE(x) ((x) * (x))

typedef struct {
    unsigned enable : 1; // 占 1 bit。
    unsigned mode : 3;   // 占 3 bit，能表示 0~7。
    unsigned error : 1;  // 占 1 bit。
    unsigned reserved : 3;
} DeviceStatus;

int main(void)
{
    int nums[] = {1, 2, 3, 4};
    LOG_INFO("array size=%d", ARRAY_SIZE(nums));

    uint32_t flags = 0;
    SET_FLAG(flags, BIT(0));
    SET_FLAG(flags, BIT(3));

    printf("flags=0x%08x\n", flags);
    printf("has bit3=%s\n", HAS_FLAG(flags, BIT(3)) ? "yes" : "no");

    CLEAR_FLAG(flags, BIT(0));
    printf("after clear bit0 flags=0x%08x\n", flags);

    printf("square=%d\n", SQUARE(5));

    DeviceStatus status = {0};
    status.enable = 1;
    status.mode = 5;
    status.error = 0;

    printf("status enable=%u mode=%u error=%u sizeof=%zu\n",
           status.enable,
           status.mode,
           status.error,
           sizeof(status));

    return 0;
}
