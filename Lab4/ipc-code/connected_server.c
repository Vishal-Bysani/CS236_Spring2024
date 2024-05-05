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
    struct sockaddr_un server_addr, client_addr;

    char* path = "os";
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, path);

    if(bind(sockfd,(struct sockaddr*)&server_addr, sizeof(server_addr))==-1) error("Error in binding\n");
    if(listen(sockfd, LISTEN_BACKLOG)==-1) error("Error in listening\n");

    int client_size = sizeof(client_addr);
    int sockfd1 = accept(sockfd,(struct sockaddr*)&client_addr,&client_size);

    printf("Server ready\n");
    char p[50];
    bzero(p,strlen(p));
    recv(sockfd1,p,100,0);
    printf("%s\n",p);

    send(sockfd1, "OkBye", strlen("OkBye"), 0);
    unlink(path);
}