#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    int value = 100;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // 子进程拥有父进程地址空间的拷贝。
        // 修改 value 不会影响父进程里的 value。
        value = 200;
        printf("child pid=%d value=%d\n", getpid(), value);
        return 0;
    }

    waitpid(pid, NULL, 0);
    printf("parent pid=%d child=%d value=%d\n", getpid(), pid, value);
    return 0;
}
