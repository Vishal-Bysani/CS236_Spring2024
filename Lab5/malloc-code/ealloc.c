#include "ealloc.h"

int page[4][16];
char * pages[4];

void init_alloc(void){
    for(int i=0;i<4;i++){
        for(int j=0;j<16;j++)
            page[i][j] = 0;
        pages[i]=NULL;
    }
}

void cleanup(void){
    for(int i=0;i<4;i++){
        for(int j=0;j<16;j++)
            page[i][j] = 0;
        pages[i]=NULL;
    }
}

char* alloc (int size){
    if(size%256 || size>PAGESIZE) return NULL;
    for(int i=0;i<4;i++){
        if(pages[i]){
            for(int j=0;j<16;j++){
                int flag=0;
                if(!page[i][j])
                {
                    for(int k=0;k<size/MINALLOC;k++){
                        if(page[i][j+k]) flag=1;
                    }
                    if(!flag){
                        for(int k=0;k<size/MINALLOC;k++){
                            page[i][j+k]=1;;
                        }
                        page[i][j]=size/MINALLOC;
                        return (pages[i]+j*MINALLOC);
                    } 
                }
            }
        }
    }
    for(int i=0;i<4;i++){
        if(!pages[i]){
            pages[i] = mmap(0,PAGESIZE,PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1,0);
            for(int j=0;j<size/MINALLOC;j++)  page[i][j]=1;
            page[i][0]=size/MINALLOC;
            return (pages[i]);
        }
    }
    return NULL;
}
void dealloc(char *p){
    for(int i=0;i<4;i++){
        if(p - pages[i] < PAGESIZE && p-pages[i]>=0){
            int start= (p-pages[i])/MINALLOC;
            int no = page[i][start];
            for(int j=0;j<no;j++){
                page[i][start+j] = 0;
            }            
        }
    }
}