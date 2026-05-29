#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    int fds[2];
    if (pipe(fds) < 0) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        close(fds[0]); // 子进程只写，关闭读端。
        const char* msg = "hello from child";
        write(fds[1], msg, strlen(msg));
        close(fds[1]);
        return 0;
    }

    close(fds[1]); // 父进程只读，关闭写端。
    char buffer[128] = {0};
    ssize_t n = read(fds[0], buffer, sizeof(buffer) - 1);
    if (n > 0) {
        buffer[n] = '\0';
        printf("parent read: %s\n", buffer);
    }

    close(fds[0]);
    waitpid(pid, NULL, 0);
    return 0;
}
