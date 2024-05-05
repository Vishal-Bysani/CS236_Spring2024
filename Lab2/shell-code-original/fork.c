#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main(){
    int stat;
    close(STDOUT_FILENO);
    int fd=open("foo.txt",O_CREAT|O_RDWR );
    pid_t ret= fork();
    if(ret<0){
        printf("Error in forking\n");
    }
    else if(ret==0){
        printf("I am a child with pid %d\n",getpid());
    }
    else{
        int ret2 = wait(NULL);
        printf("I am a parent and I ahve reaped child with pid %d\n", ret2);
    }
    // int i, stat;
    // pid_t pid[5];
    // for(int i=0;i<5;i++){
    //     pid_t ret= fork();
    //     if(ret==0) exit(100+i);
    //     else pid[i]=ret;
    // }
    // for(int i=0;i<5;i++){
    //     pid_t ret2= waitpid(pid[i], &stat, 0);
    //     printf("%d\n",stat);
    //     if(WIFEXITED(stat)){
    //         printf("Child with pid %d terminated with exit code %d\n", ret2, WEXITSTATUS(stat));
    //     }
    // }
}