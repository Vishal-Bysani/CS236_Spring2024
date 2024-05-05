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

    struct sockaddr_un server_addr;
    char *path = "unix_socket";
    
    bzero((char*)(&server_addr), sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, path);

    char buffer[256];
    printf("Client starting now .....\n\n\n");


    FILE *fp;
	fp = fopen("consumer.c", "r");
	if (fp == NULL) {
		printf("Error opening file\n");
		exit(1);
	}
	fseek(fp, 0, SEEK_END);
	int file_size = ftell(fp);
	fclose(fp);


    int fd = open("consumer.c",O_RDONLY,0666), flag = 1;
    if(fd == -1)error("Error in opening the file\n");

    sendto(sockfd, &file_size, sizeof(file_size), 0 , (struct sockaddr*) &server_addr, sizeof(server_addr));
    
    while((flag=read(fd, buffer, 256))>0){
        buffer[flag]='\0';
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*) &server_addr, sizeof(server_addr));

        printf("CLIENT sent:\n\n\n %s\n\n\n", buffer);
        sleep(1);
        bzero(buffer, 256);
    }

    printf("\n\n\nClient finished sending data\n");
    close(fd);
    close(sockfd);
    return 0;
}