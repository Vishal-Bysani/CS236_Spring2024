#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include "zemaphore.h"

void zem_init(zem_t *s, int value) {
  s->value = value;
  s->c = PTHREAD_COND_INITIALIZER;
  s->m = PTHREAD_MUTEX_INITIALIZER;
}

void zem_down(zem_t *s) {
    pthread_mutex_lock(&s->m);
    s->value -=1;
    if(s->value<0) pthread_cond_wait(&s->c, &s->m);
    pthread_mutex_unlock(&s->m);
}   

void zem_up(zem_t *s) {
    pthread_mutex_lock(&s->m);
    s->value+=1;
    pthread_cond_signal(&s->c);
    pthread_mutex_unlock(&s->m);
}
