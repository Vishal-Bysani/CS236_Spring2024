#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<fcntl.h>


int main(int argc, char * argv[])
{
    char path[]="/tmp/fifo", rd_str[10];
    mkfifo(path,0666);
    int fd = open(path, O_RDONLY);
    read(fd,rd_str,10);
    close(fd);

    printf("%s",rd_str);
    unlink(path);
    return 0;
}