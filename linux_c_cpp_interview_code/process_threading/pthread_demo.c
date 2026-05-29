#include <pthread.h>
#include <stdio.h>

#define THREAD_COUNT 4
#define LOOP_COUNT 100000

static int g_counter = 0;
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

static void* thread_func(void* arg)
{
    int id = *(int*)arg;

    for (int i = 0; i < LOOP_COUNT; ++i) {
        pthread_mutex_lock(&g_mutex);
        ++g_counter;
        pthread_mutex_unlock(&g_mutex);
    }

    printf("thread %d done\n", id);
    return NULL;
}

int main(void)
{
    pthread_t threads[THREAD_COUNT];
    int ids[THREAD_COUNT];

    for (int i = 0; i < THREAD_COUNT; ++i) {
        ids[i] = i;
        if (pthread_create(&threads[i], NULL, thread_func, &ids[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    for (int i = 0; i < THREAD_COUNT; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("counter=%d expected=%d\n", g_counter, THREAD_COUNT * LOOP_COUNT);
    pthread_mutex_destroy(&g_mutex);
    return 0;
}
