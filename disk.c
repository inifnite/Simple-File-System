#include "disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

disk* create_disk(int nbytes) {
	int numBlocks = (nbytes - 24 )/BLOCKSIZE;

	disk* diskptr = (disk*)malloc(sizeof(disk));

	if(diskptr == NULL) return NULL;

	diskptr->block_arr = malloc(sizeof(char*)*numBlocks);

	for(int i=0; i<numBlocks; i++) {
		diskptr->block_arr[i] = malloc(BLOCKSIZE);
	}

	if(diskptr->block_arr == NULL) return NULL;

	diskptr->size = nbytes;
	diskptr->blocks = numBlocks;
	diskptr->reads = 0;
	diskptr->writes = 0;

	return diskptr;
}

int create_disk_func(disk *diskptr, int nbytes) {
	diskptr = create_disk(nbytes);

	//printf("disk.c    ###### create_disk   ---->  %d\n", diskptr == NULL);
	//printf("disk.c    ###### create_disk   ---->  blocks   --->  %d\n", diskptr->blocks);

	if(diskptr == NULL) return -1;
	else return 0;
}

int read_block(disk *diskptr, int blocknr, void *block_data) {
	if (blocknr < 0 || blocknr > diskptr->blocks) {
		printf("write_block --> blocknr is invalid\n");
		return -1;
	}

	//block_data = NULL;
	//printf("size of block_data in read is %ld\n", sizeof(block_data));

	memcpy(block_data, diskptr->block_arr[blocknr], BLOCKSIZE);

	if(block_data == NULL) return -1;

	diskptr->reads ++;

	return 0;
}

int write_block(disk *diskptr, int blocknr, void* block_data) {
	if (blocknr < 0 || blocknr > diskptr->blocks) {
		printf("write_block --> blocknr is invalid\n");
		return -1;
	}

	if (block_data == NULL) return -1;

	memcpy(diskptr->block_arr[blocknr], block_data, BLOCKSIZE);

	//if(diskptr->block_arr[blocknr] == NULL) return -1;
	
	diskptr->writes ++;

	return 0;
}

int free_disk(disk* diskptr) {
	if(diskptr == NULL) {
		//printf("disc.c   #### free_disk  ----> diskptr is NULL\n");
		return -1;
	}
	for(int i=0; i< diskptr->blocks; i++) {
		free(diskptr->block_arr[i]);
	}
	free(diskptr->block_arr);
	free(diskptr);
	return 0;
}