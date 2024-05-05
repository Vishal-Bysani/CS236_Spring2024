#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include<sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main()
{
	const int SIZE = 4096;
	const char *name = "OS";
	const char *message0= "freeeee";
    const char *message1= "OSisFUN";
    const char *message2= "Message Written\n";


    //pipe into which producer writes and consumer reads
    char path1[]="/tmp/fifowrite";
    mkfifo(path1,0666);
    int fifowrite = open(path1, O_WRONLY);
    //pipe into which consumer writes and producer reads
    char path2[]="/tmp/fiforead";
    mkfifo(path2,0666);
    int fiforead = open(path2, O_RDONLY);

    //shared memory segment 
    int shm_fd = shm_open(name, O_CREAT|O_RDWR, 0666);
    ftruncate(shm_fd,SIZE);
    void* ptr = mmap(0, SIZE, PROT_READ| PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED){
        printf("Map failed\n");
        return -1;
    }
    //producer fills the shared memory with freeeee
    for(int i=0;i<512;i++)
    {
        strcpy(ptr+8*i,message0);
    }

    //writing into shared memory and informing the consumer on writing
    for(int i=0;i<10;i++){
        int j;
        j = 8*((3*i+73)%512);
        memcpy(ptr+j, message1, 8);
        printf("%d [PRODUCER] has written the message %s into the shared memory with offset %d\n\n\n", i+1, message1, (j));
        write(fifowrite, &j , 4);                   // writes the offset address into the pipe(Please don't write the address as virtual addresses are not shared between processes)
        read(fiforead, &j, 4);                      // absolutely useless read(or maybe it waits till the consumer consumes)
        sleep(1);
    }

    close(fiforead);
    close(fifowrite);

    char path3[]="/tmp/confirm";
    mkfifo(path3,0666);
    int confirm = open(path3, O_RDONLY);
    int k ;
    read(confirm, &k, 4);

    close(confirm);
    close(fiforead);
    close(fifowrite);
	return 0;
}
