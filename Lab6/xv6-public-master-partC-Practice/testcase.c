#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(void)
{
  printf(1, "Number of free pages : %d\n", getNumFreePages());
  char * x= sbrk(4096);
  x[0]=1;
  printf(1, "Number of free pages : %d\n", getNumFreePages());
  if(fork()==0){
    printf(1,"Child process pid: %d, x: %d\n", getpid(), x[0]);
    printf(1, "Number of free pages : %d\n", getNumFreePages());
    x[0]=5;
    printf(1,"Child process pid: %d, x: %d\n", getpid(), x[0]);
    printf(1, "Number of free pages : %d\n", getNumFreePages());
    exit();
  }
  else{
    wait();
    printf(1,"Parent process pid: %d, x: %d\n", getpid(), x[0]);
    printf(1, "Number of free pages : %d\n", getNumFreePages());
    exit();
  }
  exit();
}