#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/un.h>

#define SOCK_PATH "/tmp/unix_socket_example"

int main(int argc, char *argv[]){
    
    int sockfd, ret;
    struct sockaddr_un serveraddr, clientaddr;
    // Socket creation
    sockfd = socket(AF_UNIX,SOCK_DGRAM, 0);
    if(sockfd <0)
    printf("Error in creating socket\n");

    //obtain server address
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sun_family = AF_UNIX;
    strcpy(serveraddr.sun_path, SOCK_PATH);

    //bind server address
    if(bind(sockfd,(struct sockaddr*) &serveraddr, sizeof(serveraddr))<0)
    perror("Error in binding\n");

    printf("Server ready\n");
    char buffer[256];

    while (1)
    {
        printf("Reading data...\n");
        int len = sizeof(clientaddr);
        int ret = recvfrom(sockfd, buffer, 255, 0,(struct sockaddr*) &clientaddr, &len);
        printf("%d\n\n\n",ret);
        if(ret <= 0)
        {
            printf("Finished reading\n");
            break;
        }
        printf("%s\n", buffer);
        printf("Data read\n\n\n");

        sleep(2);
    }
    
    unlink(SOCK_PATH);
    return 0;
}