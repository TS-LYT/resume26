#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct {
    int ready;
    char text[64];
} SharedData;

int main(void)
{
    SharedData* data = (SharedData*)mmap(NULL,
                                         sizeof(SharedData),
                                         PROT_READ | PROT_WRITE,
                                         MAP_SHARED | MAP_ANONYMOUS,
                                         -1,
                                         0);
    if (data == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    data->ready = 0;
    strcpy(data->text, "");

    pid_t pid = fork();
    if (pid == 0) {
        strcpy(data->text, "hello shared memory");
        data->ready = 1;
        return 0;
    }

    while (!data->ready) {
        usleep(1000);
    }

    printf("parent read: %s\n", data->text);
    waitpid(pid, NULL, 0);
    munmap(data, sizeof(SharedData));
    return 0;
}
