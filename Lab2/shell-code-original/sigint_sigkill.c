#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/wait.h>
#include <errno.h>
#include<signal.h>

sig_atomic_t sigflag = 0;
void sig_handler(int sig){
    if(sig==SIGINT)
    printf("I will run forever\n");
}
int main(){
    if(signal(SIGINT, sig_handler)==SIG_ERR) 
    {
        perror("Error: ");
    }
    while(1){
        // if (sigflag != 0) { printf("Signal!\n"); sigflag = 0; }
    }
}