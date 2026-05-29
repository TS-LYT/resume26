#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    printf("argc=%d\n", argc);
    for (int i = 0; i < argc; ++i) {
        printf("argv[%d]=%s\n", i, argv[i]);
    }

    const char* mode = getenv("APP_MODE");
    if (mode == NULL) {
        printf("APP_MODE not set, use default mode\n");
    } else {
        printf("APP_MODE=%s\n", mode);
    }

    setenv("APP_MODE", "runtime-set", 1);
    printf("APP_MODE after setenv=%s\n", getenv("APP_MODE"));

    char cwd[512] = {0};
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("getcwd failed: %s\n", strerror(errno));
        return 1;
    }
    printf("current working directory=%s\n", cwd);

    if (access("README.md", F_OK) == 0) {
        printf("README.md exists in current directory\n");
    } else {
        printf("README.md not found: %s\n", strerror(errno));
    }

    return 0;
}
