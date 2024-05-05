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
    const char* name = "OS", *path1 = "/tmp/producer_to_consumer", *path2= "/tmp/consumer_to_producer", *path3 = "/tmp/confirm", *freee = "freeeee", *os = "OSisFun"; 
    mkfifo(path1, 0666);
    mkfifo(path2, 0666);
    mkfifo(path3, 0666);
    int p2c = open(path1, O_WRONLY, 0666);
    int c2p = open(path2, O_RDONLY, 0666);
    int confirm = open(path3, O_RDONLY,0666);
    int shm_fd = shm_open(name, O_CREAT|O_RDWR, 0666);
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
    




    for(int i = 0; i< SIZE/8 ; i++){
        memcpy(ptr+i*8, freee, strlen(freee)+1);
    }   
    for(int i=0; i<SIZE/8; i++){
        memcpy(ptr+i*8, os, strlen(os)+1);
        write(p2c, &i, sizeof(int));
        printf("[%d] PRODUCER has written at offset %d\n",i,i);
    }

    char *str =(char *)(malloc(sizeof(char)*8));
    for(int i=0; i<50- SIZE/8;i++){
        int j;
        read(c2p, &j, sizeof(int));
        memcpy(str, ptr+j*8, 8);
        memcpy(ptr+j*8, os, strlen(os)+1);
        write(p2c, &j, sizeof(int));
        printf("[%d] PRODUCER has written %s instead of %s at offset %d\n",i+SIZE/8 ,os,str ,j);
    }

    char c;
    read(confirm, &c, 1);

    free(str);
    close(confirm);
    close(p2c);
    close(c2p);


}