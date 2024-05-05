#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include<fcntl.h>
#include <sys/un.h>

#define LISTEN_BACKLOG 50

void error(char *e){
    perror(e);
    exit(1);
}
int main(){
    int sockfd = socket(AF_UNIX,SOCK_STREAM,0);
    struct sockaddr_un server_addr;

    char* path = "os";
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, path);

    connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    send(sockfd, "Hello", strlen("Hello"),0);
    char p[50];
    recv(sockfd, p, 10,0);
    printf("%s\n",p);
}