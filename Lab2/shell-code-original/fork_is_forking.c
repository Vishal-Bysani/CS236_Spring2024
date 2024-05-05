#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/wait.h>
#include <errno.h>
int count=0;
int main(){
    int n=3;
    for(int i=0;i<n;i++){
        pid_t pid= fork();
        if(pid!=0){ wait(NULL); count++; printf("Child with pid %d is made and reaped\n",pid);}
        else printf("You are in child with pid %d\n",getpid());
    }

    printf("\n\n%d\n\n",count);
}