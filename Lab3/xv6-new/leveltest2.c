#include "types.h"
#include "stat.h"
#include "user.h"



int
main(int argc, char *argv[]) {
    int level = getLevel(getpid());
    printf(2, "parent level- %d with pid %d\n", level, getpid());

    int c1 = fork();
    if (c1 == 0)
    {
        level = getLevel(getpid());
        printf(2, "child level - %d with pid %d\n", level, getpid());
        int c2 = fork();
        if (c2 == 0)
        {
            level = getLevel(getpid());
            printf(2, "grand child level before orphan - %d with pid %d\n", level, getpid());

            sleep(3);
            level = getLevel(getpid());
            printf(2, "grand child level after orphan - %d with pid %d\n", level, getpid());
            exit();
        }
        else
            sleep(1);
        exit();
        
    }
    else
        wait();

    sleep(4);
    exit();
}
