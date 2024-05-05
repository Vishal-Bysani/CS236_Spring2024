#include <stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
int main() {
    printf("Hello, world!\n");
    char * addr = mmap(0, 4096*1024, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1, 0);
    // char *addr = (char *)(malloc(1024*50));
    for(int i=0;i<4096*1024;i++)
    {
        addr[i] = 'a';
    }
    int i;
    scanf("Please enter: %d\n", &i);
    return 0;
}
