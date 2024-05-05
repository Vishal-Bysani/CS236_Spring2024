#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int done = 1;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

void thread_print(void *arg) {
    sleep(rand() % 3 + 1); // sleep for a random time between 1 and 5 seconds

    pthread_mutex_lock(&m);
    while (done != *(int *)arg) {
        pthread_cond_wait(&c, &m);
    }
    printf("I am thread %d\n", *(int *)arg);
    done++;
    pthread_cond_broadcast(&c);
    pthread_mutex_unlock(&m);
    pthread_exit(NULL);
}

int main() {
    int n;
    printf("Enter the number of threads: ");
    scanf("%d", &n);

    pthread_t t[n];
    int arg[n];
    for (int i = 0; i < n; i++) {
        arg[i] = i + 1;
        pthread_create(&t[i], NULL, (void *)thread_print, (void *)&arg[i]);
    }
    for (int i = 0; i < n; i++) {
        pthread_join(t[i], NULL);
    }
    printf("I am the main thread\n");
    return 0;
}
