#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/un.h>
void error(char *e){
    perror(e);
    exit(0);
}
int main()
{
    int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(sockfd == -1) error("Error in creating socket\n"); 

    struct sockaddr_un server_addr, client_addr;
    char *path = "unix_socket";
    
    bzero((char*)(&server_addr), sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, path);

    if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr))==-1) error("Error in binding\n");

    char buffer[256];
    int len = sizeof(client_addr);
    printf("Server is starting to recieve .....\n\n\n");

    int file_size, bytes_recieved=0;
    recvfrom(sockfd, &file_size, sizeof(file_size), 0, (struct sockaddr*)&client_addr, &len);
    while(bytes_recieved<file_size){
        int n=recvfrom(sockfd, buffer, 256, 0,(struct sockaddr *)&client_addr, &len);
        printf("SERVER recieved: \n\n\n%s\n\n\n",buffer);
        bytes_recieved += n;
        bzero(buffer, 256);
    }
    printf("\n\n\nServer finished recieving\n");
    unlink(path);
}