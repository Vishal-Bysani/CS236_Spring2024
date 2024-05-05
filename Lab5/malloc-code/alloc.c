#include "alloc.h"
int free_list[512];
int size_list[512];
char *ptr;

int init_alloc()
{
    //mmap to get page
    ptr = (char *)mmap(0, PAGESIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(ptr == MAP_FAILED)
        return 1;	//mmap failed
    
    //initialize the memory
    memset(ptr, 0, PAGESIZE);
    for(int i=0;i<512;i++) {free_list[i] = 0; size_list[i] = 0;}//initialize free_list and size_list
    return 0;
}

int cleanup(){
  if(munmap(ptr, PAGESIZE) == -1)
  return 1;
  for(int i=0;i<512;i++){
    free_list[i] = 0;
    size_list[i] = 0;
  }
  return 0;
}

char * alloc(int size){
    if(size%8) return NULL;
    for(int i=0;i<512;i++){
        int flag = 0;
        if(free_list[i]==0){
            for(int j=0;j<size/8;j++){
                if(i+j >= 512 || free_list[i+j]!=0) {flag =1; break;}
            }
            if(!flag){
                size_list[i] = size;
                for(int j=0;j<size/8;j++)
                free_list[i+j]=1;
                return (ptr+8*i);
            }
        }
    }
    return NULL;
}
void dealloc(char *p){
    int index = (p-ptr)/8;
    for(int i=0;i<size_list[index]/8;i++){
        free_list[index+i] = 0;
    }
    size_list[index] = 0;
}