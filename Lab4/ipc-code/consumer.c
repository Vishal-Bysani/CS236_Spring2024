#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/shm.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<string.h>
#define SIZE 100
int main(){
    const char* name = "OS", *path1 = "/tmp/producer_to_consumer", *path2= "/tmp/consumer_to_producer", *path3 = "/tmp/confirm", *freee = "freeeee"; 
    mkfifo(path1, 0666);
    mkfifo(path2, 0666);
    mkfifo(path3, 0666);
    int p2c = open(path1, O_RDONLY, 0666);
    int c2p = open(path2, O_WRONLY, 0666);
    int confirm = open(path3, O_WRONLY,0666);

    int shm_fd = shm_open(name, O_RDWR, 0666);
    if(shm_fd == -1) {
        perror("Error in shm_open\n");
        exit(-1);
    }
    if(ftruncate(shm_fd, SIZE)==-1) 
    {
        perror("Error in ftruncate\n");
        exit(-1);
    }
    void* ptr = mmap(NULL, SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED)
    {
        perror("Error in mmap\n");
        exit(-1);
    } 
    

    char* str = (char*)(malloc(sizeof(char)*8));
    
    for(int i=0;i<50;i++){
        int j;
        read(p2c, &j, sizeof(int));
        memcpy(str, ptr+8*j, 8);
        memcpy(ptr+8*j, freee, strlen(freee));
        write(c2p, &j, sizeof(int));
        printf("[%d] CONSUMER has read message %s and replaced it with %s at offset %d\n", i, str, ptr+8*j, j);
        sleep(1);
    }

    write(confirm, "c", 1);
    free(str);
    close(p2c);
    close(c2p);
    close(confirm);
    shm_unlink(name);
    
}