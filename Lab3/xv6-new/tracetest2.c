#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

int main(int argc, char *argv[])
{

    char buf[100];
    int fd = open("README", O_RDONLY);

    read(fd, buf, 6);
    trace();
    read(fd, buf, 7);
    int p = fork();
    if (p == 0)
    {
        read(fd, buf, 8);
        int fd1 = open("cs333.txt", O_RDONLY);
        read(fd1, buf, 100);
        exit();
    }
    else
        wait();

    exit();
}
