#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"




int
main(int argc, char *argv[]) {
    
    char buf[20];
    int fd = open("README", O_RDONLY);
    
    read(fd, buf, 5);
    trace();
    read(fd, buf, 5);
    read(fd, buf, 5);

    exit();
}
