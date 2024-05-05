#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "zemaphore.h"
#define NUM_THREADS 3
#define NUM_ITER 10

zem_t c, lock;
int count = 0;
void *justprint(void *data)
{
  int thread_id = *((int *)data);

  for(int i=0; i < NUM_ITER; i++)
    {
      while(count%NUM_THREADS != thread_id)
      {
        zem_down(&c);
      }
      zem_down(&lock);
      printf("This is thread %d\n", thread_id);
      count++;
      zem_up(&lock);
      zem_up(&c);
    }
  return 0;
}

int main(int argc, char *argv[])
{

  pthread_t mythreads[NUM_THREADS];
  int mythread_id[NUM_THREADS];


  zem_init(&c, 1);
  zem_init(&lock,1);
  for(int i =0; i < NUM_THREADS; i++)
    {
      mythread_id[i] = i;
      pthread_create(&mythreads[i], NULL, justprint, (void *)&mythread_id[i]);
    }
  
  for(int i =0; i < NUM_THREADS; i++)
    {
      pthread_join(mythreads[i], NULL);
    }
  
  return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/types.h>
// #include <string.h>
// #include <errno.h>
// #include <signal.h>
// #include <wait.h>
// #include <pthread.h>
// #include "zemaphore.h"

// #define NUM_THREADS 5
// #define NUM_ITER 5

// struct zemaphore z[NUM_THREADS];

// void *justprint(void *data)
// {
//   int thread_id = *((int *)data);

//   for(int i=0; i < NUM_ITER; i++)
//     {
//       // first go down on z[thread_id - 1 % NUM_THREADS]
//       if (thread_id == 0) {
//         zem_down(&z[NUM_THREADS - 1]);
//       }
//       else {
//         zem_down(&z[thread_id - 1]);
//       }
//       printf("This is thread %d\n", thread_id);
//       // now, go up on z[thread_id]
//       zem_up(&z[thread_id]);
//     }
//   return 0;
// }

// int main(int argc, char *argv[])
// {

//   pthread_t mythreads[NUM_THREADS];
//   int mythread_id[NUM_THREADS];
  
//   for (int i=0; i<NUM_THREADS; i++) {
//     zem_init(&z[i],0); // initialise all zemaphores to 0
//   } 
//   zem_init(&z[NUM_THREADS - 1],1); // because we want the first thread to actually print :)
//   for(int i =0; i < NUM_THREADS; i++)
//     {
//       mythread_id[i] = i;
//       pthread_create(&mythreads[i], NULL, justprint, (void *)&mythread_id[i]);
//     }
  
//   for(int i =0; i < NUM_THREADS; i++)
//     {
//       pthread_join(mythreads[i], NULL);
//     }
  
//   return 0;
// }