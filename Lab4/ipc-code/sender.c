#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<fcntl.h>


int main(int argc, char * argv[])
{
    char path[]="/tmp/fifo";
    mkfifo(path,0666);

    int fd = open(path, O_WRONLY);
    write(fd, "Hello\n", 6);
    close(fd);
    return 0;
}