#include "simplefs-ops.h"
extern struct filehandle_t file_handle_array[MAX_OPEN_FILES]; // Array for storing opened files

int simplefs_create(char *filename){
    /*
	    Create file with name `filename` from disk
	*/
	struct superblock_t *superblock = (struct superblock_t *)malloc(sizeof(struct superblock_t));
    simplefs_readSuperBlock(superblock);
	struct inode_t* inode_ptr= (struct inode_t *)malloc(sizeof(struct inode_t));
	for(int i=0; i< NUM_INODES; i++){
		if(superblock->inode_freelist[i] == INODE_IN_USE){
			simplefs_readInode(i, inode_ptr);
			if(strcmp(inode_ptr->name, filename)==0){
				free(inode_ptr);
				free(superblock);
				return -1;
			}
		}
	}
	free(superblock);
	int inode_number = simplefs_allocInode();
	if(inode_number == -1) return -1;
	
	simplefs_readInode(inode_number, inode_ptr);
	memcpy(inode_ptr->name, filename, MAX_NAME_STRLEN);
	inode_ptr->status = INODE_IN_USE;
	simplefs_writeInode(inode_number, inode_ptr);
	return inode_number;
}


void simplefs_delete(char *filename){
    /*
	    delete file with name `filename` from disk
	*/
	struct superblock_t* superblock = (struct superblock_t*) malloc(sizeof(struct superblock_t));
	struct inode_t* inode_ptr = (struct inode_t*) malloc(sizeof(struct inode_t));
	simplefs_readSuperBlock(superblock);
	for(int i=0;i<NUM_INODES; i++){
		if(superblock->inode_freelist[i] == INODE_IN_USE){
			simplefs_readInode(i, inode_ptr);
			if(strcmp(filename, inode_ptr->name)==0){
				for(int j=0;j<MAX_FILE_SIZE; j++){
					if(inode_ptr->direct_blocks[j]!=-1){
						simplefs_freeDataBlock(inode_ptr->direct_blocks[j]);
					}
				}
				simplefs_freeInode(i);
				free(superblock);
				free(inode_ptr);
				return ;
			}
		}
	}
	free(superblock);
	free(inode_ptr);
}

int simplefs_open(char *filename){
    /*
	    open file with name `filename`
	*/
	struct superblock_t* superblock = (struct superblock_t*) malloc(sizeof(struct superblock_t));
	struct inode_t* inode_ptr = (struct inode_t*) malloc(sizeof(struct inode_t));
	simplefs_readSuperBlock(superblock);
	int flag = -1;
	for(int i=0;i<NUM_INODES; i++){
		if(superblock->inode_freelist[i]==INODE_IN_USE){
			simplefs_readInode(i,inode_ptr);
			if(strcmp(inode_ptr->name, filename)==0){
				flag = i;
				break;
			}
		}
	}
	free(superblock);
	free(inode_ptr);
	if(flag == -1) return -1;
	for(int i=0;i<MAX_OPEN_FILES;i++){
		if(file_handle_array[i].inode_number < 0){
			file_handle_array[i].inode_number = flag;
			file_handle_array[i].offset = 0;
			return i;
		}
	}
	return -1;
}

void simplefs_close(int file_handle){
    /*
	    close file pointed by `file_handle`
	*/
	file_handle_array[file_handle].offset = 0;
	file_handle_array[file_handle].inode_number = -1;
}

int simplefs_read(int file_handle, char *buf, int nbytes){
    /*
	    read `nbytes` of data into `buf` from file pointed by `file_handle` starting at current offset
	*/
	int inode_number = file_handle_array[file_handle].inode_number, current_offset = file_handle_array[file_handle].offset;
	struct inode_t* inode_ptr = (struct inode_t*)malloc(sizeof(struct inode_t));
	simplefs_readInode(inode_number, inode_ptr);
	if(current_offset+nbytes > inode_ptr->file_size) return -1;

	int bytes_read = 0,  block = 0;
	for(int i=0; i<MAX_FILE_SIZE; i++){
		if(bytes_read == nbytes)return 0;
		if(inode_ptr->direct_blocks[i]!=-1)
		{
			char temBuf[BLOCKSIZE];
			simplefs_readDataBlock(inode_ptr->direct_blocks[i], temBuf);
			if(current_offset/BLOCKSIZE == block){
				if(current_offset%BLOCKSIZE + nbytes < BLOCKSIZE){
					memcpy(buf+bytes_read, temBuf+(current_offset%BLOCKSIZE), nbytes);
					bytes_read+=nbytes;
				}
				else{
					memcpy(buf+bytes_read, temBuf+(current_offset%BLOCKSIZE), BLOCKSIZE-current_offset%BLOCKSIZE);
					bytes_read+=BLOCKSIZE- current_offset%BLOCKSIZE;
				}
			}
			else if(current_offset/BLOCKSIZE < block){
				if(nbytes - bytes_read < BLOCKSIZE){
					memcpy(buf+bytes_read, temBuf, nbytes-bytes_read);
					bytes_read+=nbytes-bytes_read;
				}
				else{
					memcpy(buf+bytes_read, temBuf, BLOCKSIZE);
					bytes_read+=BLOCKSIZE;
				}
			}
			block++;
		}
	}
	if(bytes_read == nbytes) return 0;
	return -1;
}


int simplefs_write(int file_handle, char *buf, int nbytes){
    /*
	    write `nbytes` of data from `buf` to file pointed by `file_handle` starting at current offset
	*/
	int free_data_blocks=0,bytes_written =0 ,block = 0, last = 0,current_offset = file_handle_array[file_handle].offset, inode_number = file_handle_array[file_handle].inode_number;
	if(current_offset+nbytes > MAX_FILE_SIZE*BLOCKSIZE) return -1;
	struct superblock_t* superblock = (struct superblock_t*) malloc(sizeof(struct superblock_t));
	struct inode_t* inode_ptr = (struct inode_t*) malloc(sizeof(struct inode_t));
	simplefs_readSuperBlock(superblock);
	simplefs_readInode(inode_number, inode_ptr);
	for(int i=0;i<NUM_DATA_BLOCKS;i++){
		if(superblock->datablock_freelist[i]==DATA_BLOCK_FREE) free_data_blocks++;
	}
	free(superblock);
	if(inode_ptr->file_size % BLOCKSIZE==0) last = BLOCKSIZE;
	else last = BLOCKSIZE- inode_ptr->file_size % BLOCKSIZE;
	if(((current_offset+nbytes)-(inode_ptr->file_size)) > free_data_blocks*BLOCKSIZE+(BLOCKSIZE - last)){
		free(inode_ptr);
		return -1;
	}
	for(int i=0;i<MAX_FILE_SIZE;i++){
		if(bytes_written==nbytes){
			break;
		}
		if(inode_ptr->direct_blocks[i]!=-1){
			char tempBuf[BLOCKSIZE];
			simplefs_readDataBlock(inode_ptr->direct_blocks[i], tempBuf);
			if(current_offset/BLOCKSIZE == block){
				if(current_offset%BLOCKSIZE + nbytes < BLOCKSIZE){
					memcpy(tempBuf+current_offset%BLOCKSIZE, buf+bytes_written, nbytes);
					simplefs_writeDataBlock(inode_ptr->direct_blocks[i], tempBuf);
					if(inode_ptr->file_size < current_offset+nbytes){
						inode_ptr->file_size=(current_offset+nbytes);
					}
					bytes_written+=nbytes;	
					simplefs_writeInode(inode_number, inode_ptr);
				}
				else{
					memcpy(tempBuf+current_offset%BLOCKSIZE, buf+bytes_written, BLOCKSIZE-current_offset%BLOCKSIZE);
					simplefs_writeDataBlock(inode_ptr->direct_blocks[i], tempBuf);
					if(inode_ptr->file_size < current_offset+ BLOCKSIZE-current_offset%BLOCKSIZE){
						inode_ptr->file_size = (current_offset+BLOCKSIZE-current_offset%BLOCKSIZE);
					}
					bytes_written+=BLOCKSIZE- current_offset%BLOCKSIZE;
					simplefs_writeInode(inode_number,inode_ptr);
				}
			}
			else if(current_offset/BLOCKSIZE < block){
				if(nbytes - bytes_written < BLOCKSIZE){
					memcpy(tempBuf, buf+bytes_written, nbytes-bytes_written);
					simplefs_writeDataBlock(inode_ptr->direct_blocks[i], tempBuf);
					bytes_written+=nbytes-bytes_written;
				}
				else{
					simplefs_writeDataBlock(inode_ptr->direct_blocks[i], buf+bytes_written);
					bytes_written+=BLOCKSIZE;
				}
			}
			block++;
		}
		else{
			int dblock=simplefs_allocDataBlock();
			inode_ptr->direct_blocks[i]= dblock;
			if(nbytes - bytes_written<BLOCKSIZE){
				char tempBuf[BLOCKSIZE];
				memset(tempBuf, 0 , BLOCKSIZE);
				memcpy(tempBuf, buf+bytes_written, nbytes-bytes_written);
				simplefs_writeDataBlock(inode_ptr->direct_blocks[i], tempBuf);
				inode_ptr->file_size+= nbytes - bytes_written;
				bytes_written+=nbytes-bytes_written;
				simplefs_writeInode(inode_number,inode_ptr);
			}
			else{
				simplefs_writeDataBlock(inode_ptr->direct_blocks[i], buf+bytes_written);
				bytes_written+=BLOCKSIZE;
				inode_ptr->file_size+=BLOCKSIZE;
				simplefs_writeInode(inode_number,inode_ptr);

			}
		}
	}
	free(inode_ptr);
	if(bytes_written == nbytes){ return 0;}
	return -1;
}


int simplefs_seek(int file_handle, int nseek){
    /*
	   increase `file_handle` offset by `nseek`
	*/
	int current_offset = file_handle_array[file_handle].offset;
	int inode_number  = file_handle_array[file_handle].inode_number;
	if(inode_number <0) return -1;
	struct inode_t* inode_ptr = (struct inode_t*)malloc(sizeof(struct inode_t));
	simplefs_readInode(inode_number, inode_ptr);
	if(inode_ptr->status == INODE_FREE) return -1;
	if(nseek + current_offset > inode_ptr->file_size || nseek + current_offset  < 0) return -1;
	file_handle_array[file_handle].offset+=nseek;
    return 0;
}