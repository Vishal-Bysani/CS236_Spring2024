#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/wait.h>
#include <errno.h>

int main(int argc, char* argv[]){
    int stat;
    pid_t ret= fork();
    if(ret<0){
        printf("Error in forking\n");
    }
    else if(ret==0){
        printf("I am a child with pid %d\n",getpid());
        fflush(stdout);
        char *args[4];
        args[0]="ls";
        args[1]="-l";
        // args[2]="fork.c";
        args[2]=NULL;
        printf("Hi Bye\n");
        // fflush(stdout);
        printf("%d\n",execvp(args[0],args));
        int x=errno;
        printf( "%d\n",x);
        printf("This line is not printed\n");
    }
    else{
        int ret2=waitpid(ret, &stat,0);
        printf("Child with pid %d has terminated\n",ret2);
        printf("I am a parent\n");
    }
}