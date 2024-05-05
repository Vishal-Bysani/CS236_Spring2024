#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;
void f(void){
    for(int i=0;i<1000;i++){
        pthread_mutex_lock(&lock);
        counter+=1;
        pthread_mutex_unlock(&lock);
    }
}
int main(){
    pthread_t p[10];
    for(int i=0;i<10;i++){
        pthread_create(&p[i], NULL, f, NULL);
    }
    for(int i=0;i<10;i++){
        pthread_join(p[i], NULL);
    }
}