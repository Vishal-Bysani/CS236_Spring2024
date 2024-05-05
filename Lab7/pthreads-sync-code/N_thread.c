#include<stdio.h>
#include<pthread.h>

pthread_mutex_t lock;

void thread_print(void *arg){
    pthread_mutex_lock(&lock);
    int *i=(int *)arg;
    printf("I am thread %d\n",*i);
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

int main(){
    int n;
    printf("Enter the number of threads: ");
    scanf("%d",&n);
    pthread_t t[n];
    pthread_mutex_init(&lock,NULL);
    int arg[n];
    for(int i=0;i<n;i++){
        arg[i]=i;
        pthread_create(&t[i],NULL,(void *)thread_print,(void *)&arg[i]);
    }
    for(int i=0;i<n;i++){
        pthread_join(t[i],NULL);
    }
    printf("I am the main thread\n");
    return 0;
}
