#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/wait.h>
#include <errno.h>
#include <signal.h>

int main(){
    int stat;
    pid_t ret= fork();
    if(ret<0){
        printf("Error in forking\n");
    }
    else if(ret==0){
        char* args[3]; args[0]=strdup("sleep"); args[1]=strdup("1000"); args[2]=NULL;
        printf("I am a child with pid %d\n",getpid());
        execvp("sleep",args);
    }
    else{
        kill(ret,SIGKILL);
        int ret2=waitpid(ret, &stat,0);
        printf("%d\n",ret);
        printf("Child with pid %d has been killed and reaped\n",ret2);
        printf("I am a parent with pid %d\n", getpid());
    }
}