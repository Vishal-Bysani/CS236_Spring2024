#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"



char* arg[2] = {"wc","README"};

int
main(int argc, char *argv[]) {
    
    char buf[20];
    int fd = open("README", O_RDONLY);
    
    read(fd, buf, 6);    
    read(fd, buf, 7);
    int p = fork();
    if (p == 0)
    {
        trace();
        read(fd, buf, 8);
        int p2 = fork();
        if (p2 == 0)
        {
            // The program being exec'd might call read multiple times
            exec("wc", arg);
            exit();
        }
        else
            wait();
        
        exit();
    }
    else
        wait();
    

    exit();
}
