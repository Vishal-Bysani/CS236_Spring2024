#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include<sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include<string.h>

int main()
{
	const char *name = "OS";
	const int SIZE = 4096;
    const char *message0 = "freeee";
    char message[8];

    
    //pipe into which producer writes and consumer reads
    const char path1[]="/tmp/fifowrite";
    mkfifo(path1,0666);
    int fifowrite = open(path1, O_RDONLY);
    //pipe into which consumer writes and producer reads
    const char path2[]="/tmp/fiforead";
    mkfifo(path2,0666);
    int fiforead = open(path2, O_WRONLY);

    //shared memory segment 
    int shm_fd = shm_open(name, O_RDWR, 0666);
    ftruncate(shm_fd,SIZE);
    void* ptr = mmap(0, SIZE, PROT_READ| PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED){
        printf("Map failed\n");
        return -1;
    }


    //reading shared memory and informing the producer on consuming anything
	for(int i=0;i<10;i++){
        int j;
        read(fifowrite, &j, 4);                     //get the file offset
        memcpy(message, (char *)(ptr+j), 8);              //copy the message
        memcpy(ptr+j, message0, 8);
        printf("%d [CONSUMER] has read and freed the message %s from the shared memory with offset %d\n. Now the new message is %s\n\n",i+1, message, (j), (char* )(ptr+j));
        write(fiforead, &j, 4);          //place the address into the buffer
        sleep(1);
    }


    close(fiforead);
    close(fifowrite);

    char path3[]="/tmp/confirm";
    mkfifo(path3,0666);
    int confirm = open(path3, O_WRONLY);
    int k = 10;
    write(confirm, &k, 4);

    close(confirm);
	return 0;
}