#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/un.h>

#define SOCK_PATH "/tmp/unix_socket_example"

int main(int argc, char *argv[]){
    if(argc < 2){
        printf("Usage: %s <file>\n", argv[0]);
        exit(1);
    }
    int sockfd, ret;
    // Socket creation
    // AFX_UNIX for local communications
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    
    if (sockfd < 0){
        printf("Error in socket\n");
        exit(1);
    }

    //obtain server address
    struct sockaddr_un serv_addr;
    bzero( &serv_addr, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, SOCK_PATH);

    int fd = open(argv[1], O_RDONLY);
    while(1){
        char buffer[256];
        ret = read(fd, buffer, 255);
        if(ret <= 0){
            break;
        }
        printf("%d\n\n\n",ret);
        printf("Sending data...\n");
        sendto(sockfd,buffer,ret,0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        printf("%s\n\n\nData sent\n\n\n", buffer);
        sleep(2);
    }
    printf("Finished sending\n\n");
    close(sockfd);
    return 0;
}