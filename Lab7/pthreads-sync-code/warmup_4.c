#include<pthread.h>
#include<stdio.h>

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

int counter = 0;
int n;
void thread_print(void *arg){
    int i=*(int *)arg;
   while(1){
    pthread_mutex_lock(&m);
    while((counter%n != (i))){
        pthread_cond_wait(&c,&m);
    }
    counter++;
    printf("I am thread %d\n",i);
    pthread_cond_broadcast(&c);
    
    pthread_mutex_unlock(&m);
   }
}

int main(){
    
    printf("Enter the number of threads: ");
    scanf("%d",&n);
    pthread_t t[n];
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

