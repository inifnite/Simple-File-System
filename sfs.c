// Gaurav Kumar Meena
// 18CS30019

#include "disk.h"
#include "sfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SetBit(A,k)     ( A[(k/32)] |= (1 << (k%32)) )
#define ClearBit(A,k)   ( A[(k/32)] &= ~(1 << (k%32)) )
#define TestBit(A,k)    ( A[(k/32)] & (1 << (k%32)) )


int ceiling(double x) {
	if((int)(x) != x) return (int)x + 1;
	else return (int)x;
}


disk* mounted_diskptr ;

void print_super_block(super_block block_super) {
	printf("\n\n------------------------------\n");
	printf("block_super magic number ---> %d\n", block_super.magic_number);
	printf("block_super blocks ---> %d\n", block_super.blocks);
	printf("block_super inode_blocks ---> %d\n", block_super.inode_blocks);
	printf("block_super inodes ---> %d\n", block_super.inodes);
	printf("block_super inode_bitmap_block_id ---> %d\n", block_super.inode_bitmap_block_idx);
	printf("block_super inode_block_idx ---> %d\n", block_super.inode_block_idx);
	printf("block_super data_block_bitmap_idx ---> %d\n", block_super.data_block_bitmap_idx);
	printf("block_super data_block_idx ---> %d\n", block_super.data_block_idx);
	printf("block_super data_blocks ---> %d\n", block_super.data_blocks);
	printf("------------------------------\n\n");
}



void print_inode(inode inode_) {
	printf("\n\n------------------------------\n");
	printf("inode valid ---> %d\n", inode_.valid);
	printf("inode size ---> %d\n", inode_.size);

	int indirect_data = 0;
	int data_blocks = ceiling((double)inode_.size/4096);
	if(data_blocks > 5) indirect_data = inode_.size - 4096*5;

	for(int i=0; i<data_blocks && i<5; i++) {
		printf("inode direct data_block %d : %d\n", i+1, inode_.direct[i]);
	}

	printf("inode data_blocks ---> %d\n", data_blocks);
	printf("inode direct pointers ---> %d\n", data_blocks > 5 ? 5 : data_blocks);
	printf("inode indirect pointers ---> %d\n", data_blocks > 5 ? 1 : 0);

	int indirect_data_block_arr[1024];

	if(indirect_data > 0) {
		if(read_block(mounted_diskptr, inode_.indirect, indirect_data_block_arr) < 0) {
			printf("indirect_data block read failed\n");
		}

		printf("inode indirect block number : %d\n", inode_.indirect);
		for(int i=0; i<1024 && i< ceiling((double)indirect_data/4096); i++) {
			printf("inode indirect block ###  data block %d : %d\n", i+1, indirect_data_block_arr[i]);
		}
	}

	printf("------------------------------\n\n");
}


void print_dir(dir dir_lst[]) {
	printf("\n\n------------------------------\n");
	for(int i=0; i<64; i++) {
		if(dir_lst[i].valid == 1) {
			printf("file name --> %s\n", dir_lst[i].name);
			printf("file type --> %d\n", dir_lst[i].type);
			printf("file inumber --> %d\n", dir_lst[i].inumber);
		}
	}
	printf("------------------------------\n\n");
}



int format(disk *diskptr) {
	super_block block_super;

	int M = diskptr->blocks - 1;
	int I = 0.1 * M;
	int numInodes = I * 128;
	int IB = ceiling((double)numInodes / (8 * 4096));

	int R = M - I - IB;
	int DBB = ceiling((double)R / (8 * 4096));
	int DB = R - DBB;


	block_super.magic_number = MAGIC;
	block_super.blocks = M;
	block_super.inode_blocks = I;
	block_super.inodes = numInodes;
	block_super.inode_bitmap_block_idx = 1;
	block_super.inode_block_idx = 1 + IB + DBB;
	block_super.data_block_bitmap_idx = 1 + IB;
	block_super.data_block_idx = 1 + IB + DBB + I;
	block_super.data_blocks = DB;

	if(write_block(diskptr, 0, &block_super) < 0) {
		printf("superblock write failed\n");
		return -1;
	}

	int bitmap[1024];
	
	for (int i=0; i<1024; i++)
		bitmap[i] = 0;

	for(int i=block_super.inode_bitmap_block_idx; i<block_super.data_block_bitmap_idx; i++)
		if(write_block(diskptr, i, bitmap) < 0){
			printf("inode bitmap initialization failed");
			return -1;
		}

	for(int i=block_super.data_block_bitmap_idx; i<block_super.data_block_idx; i++)
		if(write_block(diskptr, i, bitmap) < 0){
			printf("data bitmap initialization failed");
			return -1;
		}

	inode inode_arr[128];
	for(int i=0; i<128; i++)
		inode_arr[i].valid = 0;


	for(int i=block_super.inode_block_idx; i<block_super.inode_blocks; i++)
		if(write_block(diskptr, i, inode_arr) < 0){
			printf("data bitmap initialization failed");
			return -1;
		}

	return 0;
}



int mount(disk *diskptr) {
	mounted_diskptr = NULL;

	char* buff = (char*)malloc(4096);
	
	super_block block_super;
	if(read_block(diskptr, 0, buff) < 0) {
		printf("couldn't read super block\n");
		return -1;
	}
	else {
		block_super = (*(super_block*)buff);
		print_super_block(block_super);
	}

	if (block_super.magic_number != MAGIC) {
		printf("Invalid SFS\n");
		return -1;
	}
	mounted_diskptr = diskptr;

	free(buff);
	return 0;
}



int create_file() {
	if(mounted_diskptr == NULL) {
		printf("no valid diskptr mounted\n");
		return -1;
	}

	char* buff = (char*)malloc(4096);
	
	read_block(mounted_diskptr, 0, buff);
	super_block block_super = *((super_block*)buff);

	int bitmap[1024]; /// bitmap of (4096*8)/32 ==> 1024
	int free_inode = -1; /// free inode for new file
	int inode_bitmap_block = -1; /// Disk Block of bitmap of free_inode


	// first free inode in inode bitmap
	for(int i=block_super.inode_bitmap_block_idx; i<block_super.data_block_bitmap_idx; i++) {
		if(read_block(mounted_diskptr, i, bitmap) < 0){
			printf("inode bitmap read failed");
			return -1;
		}
		else {
			for (int j=0; j<1024*32; j++) {
				if(!TestBit(bitmap, j)) {
					free_inode = j;
					inode_bitmap_block = i;
					break;
				}
			}
		}
		if(free_inode != -1) break;
	}

	if(free_inode == -1) {
		printf("no inodes are left\n");
		return -1;
	}

	//printf("inode_bitmap_block : %d\n", inode_bitmap_block);
	//printf("inode num in block : %d\n", free_inode);

	SetBit(bitmap, free_inode);

	if(write_block(mounted_diskptr, inode_bitmap_block, bitmap) < 0) {
		printf("inode bitmap update failed\n");
		return -1;
	}

	// innode number from starting 
	int start_free_inode = 1024*32*(inode_bitmap_block - block_super.inode_bitmap_block_idx) + free_inode;


	inode inode_arr[128]; // Single Block ==> 128 inodes

	if(read_block(mounted_diskptr, block_super.inode_block_idx + start_free_inode/128, inode_arr) < 0){
		printf("inode_arr read failed");
		return -1;
	}

	inode_arr[start_free_inode % 128].valid = 1;
	inode_arr[start_free_inode % 128].size = 0;

	if(write_block(mounted_diskptr, block_super.inode_block_idx + start_free_inode/128, inode_arr) < 0) {
		printf("inode_arr write failed\n");
		return -1;
	}

	free(buff);

	return start_free_inode + block_super.inode_block_idx;
}



int remove_file(int inumber) {
	if(mounted_diskptr == NULL) {
		printf("no valid diskptr mounted\n");
		return -1;
	}

	char* buff = (char*)malloc(4096);
	
	read_block(mounted_diskptr, 0, buff);
	super_block block_super = *((super_block*)buff);

	inumber -= block_super.inode_block_idx;

	inode inode_arr[128];

	if(read_block(mounted_diskptr, block_super.inode_block_idx + inumber/128, inode_arr) < 0){
		printf("inode read failed");
		return -1;
	}


	// inode data for inode inumber 
	inode file_inode = inode_arr[inumber%128];


	int indirect_data = 0;
	int data_blocks = ceiling((double)file_inode.size/4096);
	if(data_blocks > 5) indirect_data = file_inode.size - 4096*5;
	int indirect_data_blocks = ceiling((double)indirect_data / 4096);

	int indirect_data_block_arr[1024]; // stores data block number for indirect block

	//print_inode(file_inode);

	if(indirect_data > 0) {
		if(read_block(mounted_diskptr, file_inode.indirect, indirect_data_block_arr) < 0) {
			printf("indirect_data block read failed\n");
			return -1;
		}

		for(int i=0; i<1024 && i<indirect_data_blocks; i++) {
			int toFree_data_block = indirect_data_block_arr[i];
			int toFree_data_block_bitmap_block = (toFree_data_block - block_super.data_block_idx) / (8*4096); // getting block of bitmap of toFree_data_block

			int bitmap[1024];
			if(read_block(mounted_diskptr, block_super.data_block_bitmap_idx + toFree_data_block_bitmap_block, bitmap) < 0) {
				printf("indirect data block bitmap read failed\n");
				return -1;
			}

			ClearBit(bitmap, (toFree_data_block - block_super.data_block_idx) % (8*4096));

			if(write_block(mounted_diskptr, block_super.data_block_bitmap_idx + toFree_data_block_bitmap_block, bitmap) < 0) {
				printf("indirect data block bitmap write failed\n");
				return -1;
			}		
		}
		int toFree_data_block = file_inode.indirect;
		int toFree_data_block_bitmap_block = (toFree_data_block - block_super.data_block_idx) / (8*4096); // getting block of bitmap of toFree_data_block

		int bitmap[1024];
		if(read_block(mounted_diskptr, block_super.data_block_bitmap_idx + toFree_data_block_bitmap_block, bitmap) < 0) {
			printf("indirect data block bitmap read failed\n");
			return -1;
		}
		ClearBit(bitmap, (toFree_data_block - block_super.data_block_idx) % (8*4096));

		if(write_block(mounted_diskptr, block_super.data_block_bitmap_idx + toFree_data_block_bitmap_block, bitmap) < 0) {
			printf("indirect data block bitmap write failed\n");
			return -1;
		}
	}

	for(int i=0; i<data_blocks && i<5; i++) {
		int toFree_data_block = file_inode.direct[i];
		int toFree_data_block_bitmap_block = (toFree_data_block - block_super.data_block_idx) / (8*4096);

		int bitmap[1024];
		if(read_block(mounted_diskptr, block_super.data_block_bitmap_idx + toFree_data_block_bitmap_block, bitmap) < 0) {
			printf("data block bitmap read failed\n");
			return -1;
		}

		ClearBit(bitmap, (toFree_data_block - block_super.data_block_idx) % (8*4096));

		if(write_block(mounted_diskptr, block_super.data_block_bitmap_idx + toFree_data_block_bitmap_block, bitmap) < 0) {
			printf("data block bitmap write failed\n");
			return -1;
		}
	}

	inode_arr[inumber % 128].valid = 0;
	inode_arr[inumber % 128].size = 0;
	inode_arr[inumber % 128].indirect = 0;

	if(write_block(mounted_diskptr, block_super.inode_block_idx + inumber/128, inode_arr) < 0) {
		printf("inode_arr write failed\n");
		return -1;
	}

	int inode_bitmap_block = (inumber / 128) / (8 * 4096);
	int bitmap[1024];

	if(read_block(mounted_diskptr, inumber / (1024 * 32) + block_super.inode_bitmap_block_idx, bitmap) < 0) {
		printf("inode bitmap read failed\n");
		return -1;
	}

	ClearBit(bitmap, inumber % (1024 * 32));

	if(write_block(mounted_diskptr, inumber / (1024 * 32) + block_super.inode_bitmap_block_idx, bitmap) < 0) {
		printf("inode bitmap updation failed\n");
		return -1;
	}

	free(buff);

	return 0;
}




int stat(int inumber) {
	if(mounted_diskptr == NULL) {
		printf("no valid diskptr mounted\n");
		return -1;
	}

	char* buff = (char*)malloc(4096);
	
	read_block(mounted_diskptr, 0, buff);
	super_block block_super = *((super_block*)buff);

	inumber -= block_super.inode_block_idx;

	int inode_bitmap_block = inumber/(1024*32);

	inode inode_arr[128];

	if(read_block(mounted_diskptr, block_super.inode_block_idx + inumber/128, inode_arr) < 0){
		printf("inode read failed");
		return -1;
	}

	print_inode(inode_arr[inumber]);

	free(buff);

	return 0;
}



int read_i(int inumber, char *data, int length, int offset) {
	if(mounted_diskptr == NULL) {
		printf("no valid diskptr mounted\n");
		return -1;
	}

	char* buff = (char*)malloc(4096);
	
	read_block(mounted_diskptr, 0, buff);
	super_block block_super = *((super_block*)buff);


	if(inumber < block_super.inode_block_idx || inumber >= block_super.inodes) {
		printf("Invalid inode number\n");
		return -1;
	}

	inumber -= block_super.inode_block_idx;

	int inode_bitmap_block = inumber/(1024*32);

	inode inode_arr[128];

	if(read_block(mounted_diskptr, block_super.inode_block_idx + inumber/128, inode_arr) < 0){
		printf("inode read failed");
		return -1;
	}

	inode file_inode = inode_arr[inumber%128];

	if(offset < 0 || offset >= file_inode.size) {
		printf("Invalid offset\n");
		return -1;
	}

	if(length < 0) {
		printf("Invalid length\n");
		return -1;
	}

	int indirect_data = 0;
	int data_blocks = ceiling((double)file_inode.size/4096);
	if(data_blocks > 5) indirect_data = file_inode.size - 4096*5;
	int indirect_data_blocks = ceiling((double)indirect_data / 4096);


	int curr_data_block = offset/ 4096;
	int num_data_block = ceiling((double)offset/ 4096);

	if(num_data_block > 5) {
		int indirect_data_block_arr[1024];
		curr_data_block -= 5;
		
		if(read_block(mounted_diskptr, file_inode.indirect, indirect_data_block_arr) < 0) {
			printf("indirect_data block read failed\n");
			return -1;
		}

		int len = 0;

		if(read_block(mounted_diskptr, indirect_data_block_arr[curr_data_block], buff) < 0) {
			printf("indirect data block read failed\n");
			return -1;
		}

		int remain = 4096 - (offset - 4096*5)%4096;

		memcpy(data, buff + (4096- remain), remain<length ? remain : length);
		len += remain<length ? remain : length;
		curr_data_block ++ ;

		while(len < length && len < file_inode.size - offset && curr_data_block < indirect_data_blocks) {
			if(read_block(mounted_diskptr, indirect_data_block_arr[curr_data_block], buff) < 0) {
				printf("indirect data block read failed\n");
				return -1;
			}

			memcpy(data+len, buff, (length - len)<4096 ? (length - len):4096);
			len += (length - len)<4096 ? (length - len):4096;
			curr_data_block ++;
		}
		free(buff);

		return len;
	}
	else {
		int len = 0;

		if(read_block(mounted_diskptr, file_inode.direct[curr_data_block], buff) < 0) {
			printf("direct data block read failed\n");
			return -1;
		}

		int remain = 4096 - offset % 4096;

		//printf("buff ---> %s\n", buff);
		//printf("data ---> %s\n", data);

		memcpy(data, buff + (4096- remain), remain<length ? remain : length);
		len += remain<length ? remain : length;
		curr_data_block ++ ;

		//printf("buff ---> %s\n", buff);
		//printf("data ---> %s\n", data);


		//printf("\n\n------------------------\ncurrent block : %d\nremain : %d\nlength : %d\nlen : %d\noffset : %d\n-------------------------\n\n", curr_data_block, remain, length, len, offset);

		while(len < length && len < file_inode.size - offset && curr_data_block < 5) {
			if(read_block(mounted_diskptr, file_inode.direct[curr_data_block], buff) < 0) {
				printf("indirect data block read failed\n");
				return -1;
			}

			//printf("reading direct data block : %d\n", curr_data_block);

			memcpy(data+len, buff, (length - len)<4096 ? (length - len):4096);
			len += (length - len)<4096 ? (length - len):4096 ;
			curr_data_block++;
		}

		int indirect_data_block_arr[1024];
		curr_data_block = 0;

		while(len < length && len < file_inode.size - offset && curr_data_block < indirect_data_blocks) {
			if(read_block(mounted_diskptr, indirect_data_block_arr[curr_data_block], buff) < 0) {
				printf("indirect data block read failed\n");
				return -1;
			}
			//printf("reading indirect data block : %d\n", curr_data_block);

			memcpy(data+len, buff, (length - len)<4096 ? (length - len):4096);
			len += (length - len)<4096 ? (length - len):4096;
			curr_data_block++;
		}

		free(buff);

		return len;	
	}

}




int write_i(int inumber, char *data, int length, int offset) {
	if(mounted_diskptr == NULL) {
		printf("no valid diskptr mounted\n");
		return -1;
	}

	char* buff = (char*)malloc(4096);
	
	read_block(mounted_diskptr, 0, buff);
	super_block block_super = *((super_block*)buff);

	if(inumber < block_super.inode_block_idx || inumber >= block_super.inodes) {
		printf("Invalid inode number\n");
		return -1;
	}

	inumber -= block_super.inode_block_idx;

	int inode_bitmap_block = inumber/(1024*32);

	inode inode_arr[128];

	if(read_block(mounted_diskptr, block_super.inode_block_idx + inumber/128, inode_arr) < 0){
		printf("inode read failed");
		return -1;
	}

	inode* file_inode = &inode_arr[inumber%128];

	if(offset < 0 || offset > file_inode->size) {
		printf("Invalid offset\n");
		return -1;
	}

	if(length < 0) {
		printf("Invalid length\n");
		return -1;
	}

	int indirect_data = 0;
	int data_blocks = ceiling((double)file_inode->size/4096);
	if(data_blocks > 5) indirect_data = file_inode->size - 4096*5;
	int indirect_data_blocks = ceiling((double)indirect_data / 4096);


	int curr_data_block = offset/ 4096;
	int num_data_block = ceiling((double)offset/ 4096);

	int len = 0;

	if(num_data_block > 5) {
		int indirect_data_block_arr[1024];
		curr_data_block -= 5;
		
		if(read_block(mounted_diskptr, file_inode->indirect, indirect_data_block_arr) < 0) {
			printf("indirect_data block read failed\n");
			return -1;
		}

		int remain = 4096 - (offset - 4096*5)%4096;

		if((offset % 4096) != 0) {
			//printf("overwriting inbetween indirect data block %d : %d\n", curr_data_block+1, indirect_data_block_arr[curr_data_block]);
			if(read_block(mounted_diskptr, indirect_data_block_arr[curr_data_block], buff) < 0) {
				printf("indirect data block read failed\n");
				return -1;
			}

			memcpy(buff + offset % 4096, data, remain<length ? remain : length);

			if(write_block(mounted_diskptr, indirect_data_block_arr[curr_data_block], buff) < 0) {
				printf("diskptr has %d\n", mounted_diskptr->blocks);
				return -1;
			}
			len += remain<length ? remain : length;
			curr_data_block ++;
		}

		// overwriting
		while(len < length && len < file_inode->size - offset && curr_data_block < indirect_data_blocks) {
			//printf("overwriting indirect data block %d : %d\n", curr_data_block, indirect_data_block_arr[curr_data_block]);
			memcpy(buff, data+len, (length - len)<4096 ? (length - len):4096);

			if(write_block(mounted_diskptr, indirect_data_block_arr[curr_data_block], buff) < 0) {
				printf("indirect data block write failed\n");
				file_inode->size += len ;
				return -1;
			}

			len += (length - len)<4096 ? (length - len):4096 ;
			curr_data_block ++;
		}

		// new datablocks 
		while(len < length && curr_data_block < 1024) {
			int free_data_block_from_start = -1;
			for (int i = block_super.data_block_bitmap_idx; i < block_super.inode_block_idx; i++) {
				int bitmap[1024];
				if(read_block(mounted_diskptr, i, bitmap) < 0) {
					printf("indirect new data block bitmap access failed\n");
					file_inode->size += len;
					return -1;
				}
				for (int j = 0; j < 1024*32; j++) {
					if(!TestBit(bitmap, j)) {
						free_data_block_from_start = (1024*32)*(i - block_super.data_block_bitmap_idx) + j;
						//printf("bitmap is in block : %d\ndata block number : %d\nfree_data_block_from_start : %d\n", i, j, free_data_block_from_start);

						memcpy(buff, data+len, (length - len)<4096 ? (length - len):4096);

						if(write_block(mounted_diskptr, block_super.data_block_idx + free_data_block_from_start, buff) < 0) {
							printf("indirect data block write failed\n");
							file_inode->size += len ;
							return -1;
						}

						SetBit(bitmap,j);

						if(write_block(mounted_diskptr, i, bitmap) < 0) {
							printf("indirect data block bitmap update failed\n");
							return -1;
						}					

						len += (length - len)<4096 ? (length - len):4096;

						indirect_data_block_arr[curr_data_block ++] = block_super.data_block_idx + free_data_block_from_start;
						break;
					}
				}
				if(free_data_block_from_start != -1) break;
			}
		}

		if(write_block(mounted_diskptr, file_inode->indirect, indirect_data_block_arr) < 0) {
			printf("indirect_data block updation failed\n");
			return -1;
		}

	}


	else {
		int remain = 4096 - offset % 4096;

		if((offset % 4096) != 0) {
			if(read_block(mounted_diskptr, file_inode->direct[curr_data_block], buff) < 0) {
				printf("direct data block read failed\n");
				return -1;
			}

			memcpy(buff + offset % 4096, data, remain<length ? remain : length);

			if(write_block(mounted_diskptr, file_inode->direct[curr_data_block], buff) < 0) {
				printf("direct data block write failed\n");
				return -1;
			}
			len += remain<length ? remain : length;
			curr_data_block ++;
		}

		// overwriting
		while(len < length && len < file_inode->size - offset && curr_data_block < 5) {
			//printf("overwriting data block\n");
			memcpy(buff, data+len, (length - len)<4096 ? (length - len):4096);
			
			if(write_block(mounted_diskptr, file_inode->direct[curr_data_block], buff) < 0) {
				printf("direct data block write failed\n");
				file_inode->size += len ;
				return -1;
			}

			len += (length - len)<4096 ? (length - len):4096 ;
			curr_data_block ++;
		}

		// new direct blocks
		while(len < length && curr_data_block < 5) {
			int free_data_block_from_start = -1;
			for (int i = block_super.data_block_bitmap_idx; i < block_super.inode_block_idx; i++) {
				int bitmap[1024];
				if(read_block(mounted_diskptr, i, bitmap) < 0) {
					printf("direct new data block bitmap access failed\n");
					//file_inode->size += len;
					return -1;
				}
				//print_super_block(block_super);
				for (int j = 0; j < 1024*32; j++) {
					if(!TestBit(bitmap, j)) {
						free_data_block_from_start = (1024*32)*(i - block_super.data_block_bitmap_idx) + j;
						//printf("\nbitmap is in block : %d\ndata block number : %d\nfree_data_block_from_start : %d\n\n", i, j, free_data_block_from_start);

						memcpy(buff, data+len, (length - len)<4096 ? (length - len):4096);

						//print_super_block(block_super);

						//printf("data block number from starting : %d\n", block_super.data_block_idx + free_data_block_from_start);

						if(write_block(mounted_diskptr, block_super.data_block_idx + free_data_block_from_start, buff) < 0) {
							printf("direct data block write failed\n");
							file_inode->size += len ;
							return -1;
						}

						SetBit(bitmap,j);

						if(write_block(mounted_diskptr, i, bitmap) < 0) {
							printf("direct data block bitmap update failed\n");
							return -1;
						}


						len += (length - len)<4096 ? (length - len):4096;

						file_inode->direct[curr_data_block ++] = block_super.data_block_idx + free_data_block_from_start;
						break;
					}
				}
				if(free_data_block_from_start != -1) break;
			}
		}

		curr_data_block = 0;
		int indirect_data_block_arr[1024];

		if(len < length && len >= file_inode->size - offset ) {
			//printf("I am here\n");
			int free_data_block_from_start = -1;
			for (int i = block_super.data_block_bitmap_idx; i < block_super.inode_block_idx; i++) {
				int bitmap[1024];
				if(read_block(mounted_diskptr, i, bitmap) < 0) {
					printf("direct new data block bitmap access failed\n");
					file_inode->size += len;
					return -1;
				}
				for (int j = 0; j < 1024*32; j++) {
					if(!TestBit(bitmap, j)) {
						free_data_block_from_start = (1024*32)*(i - block_super.data_block_bitmap_idx) + j;

						if(write_block(mounted_diskptr, block_super.data_block_idx + free_data_block_from_start, indirect_data_block_arr) < 0) {
							printf("direct data block write failed\n");
							return -1;
						}

						SetBit(bitmap,j);

						if(write_block(mounted_diskptr, i, bitmap) < 0) {
							printf("indirect data block bitmap initialzation failed\n");
							return -1;
						}

						file_inode->indirect = block_super.data_block_idx + free_data_block_from_start;
						break;
					}
				}
				if(free_data_block_from_start != -1) break;
			}
		}

		if(len < length && len < file_inode->size - offset) {
			if(read_block(mounted_diskptr, file_inode->indirect, indirect_data_block_arr) < 0) {
				printf("indirect_data block read failed\n");
				return -1;
			}
		}

		// overwriting
		while(len < length && len < file_inode->size - offset && curr_data_block < indirect_data_blocks) {
			//printf("overwriting indirect data block %d : %d\n", curr_data_block + 1, indirect_data_block_arr[curr_data_block]);
			memcpy(buff, data+len, (length - len)<4096 ? (length - len):4096);

			if(write_block(mounted_diskptr, indirect_data_block_arr[curr_data_block], buff) < 0) {
				printf("indirect data block write failed\n");
				file_inode->size += len ;
				return -1;
			}

			len += (length - len)<4096 ? (length - len):4096 ;
			curr_data_block ++;
		}


		// new datablocks

		while(len < length && curr_data_block < 1024) {
			int free_data_block_from_start = -1;
			for (int i = block_super.data_block_bitmap_idx; i < block_super.inode_block_idx; i++) {
				int bitmap[1024];
				if(read_block(mounted_diskptr, i, bitmap) < 0) {
					printf("indirect new data block bitmap access failed\n");
					file_inode->size += len;
					return -1;
				}
				for (int j = 0; j < 1024*32; j++) {
					if(!TestBit(bitmap, j)) {
						free_data_block_from_start = (1024*32)*(i - block_super.data_block_bitmap_idx) + j;

						if(write_block(mounted_diskptr, block_super.data_block_idx + free_data_block_from_start, buff) < 0) {
							printf("indirect data block write failed\n");
							file_inode->size += len ;
							return -1;
						}

						SetBit(bitmap,j);

						if(write_block(mounted_diskptr, i, bitmap) < 0) {
							printf("indirect data block bitmap update failed\n");
							return -1;
						}

						len += (length - len)<4096 ? (length - len):4096;

						indirect_data_block_arr[curr_data_block ++] = block_super.data_block_idx +free_data_block_from_start;
						break;
					}
				}
				if(free_data_block_from_start != -1) break;
			}
		}

		if(curr_data_block > 0 && write_block(mounted_diskptr, file_inode->indirect, indirect_data_block_arr) < 0) {
			printf("indirect_data block bitmap updation failed\n");
			return -1;
		}
	}

	file_inode->size = (offset + len) > (file_inode->size) ? (offset + len) : (file_inode->size) ;  /// offset + len => new size

	if(write_block(mounted_diskptr, block_super.inode_block_idx + inumber/128, inode_arr) < 0){
		printf("inode read failed");
		return -1;
	}
	
	free(buff);

	return len;
}





int fit_to_size(int inumber, int size) {
	if(mounted_diskptr == NULL) {
		printf("no valid diskptr mounted\n");
		return -1;
	}

	char* buff = (char*)malloc(4096);
	
	read_block(mounted_diskptr, 0, buff);
	super_block block_super = *((super_block*)buff);

	if(inumber < block_super.inode_block_idx || inumber >= block_super.inodes) {
		printf("Invalid inode number\n");
		return -1;
	}

	inumber -= block_super.inode_block_idx;

	int inode_bitmap_block = inumber/(1024*32);

	inode inode_arr[128];

	if(read_block(mounted_diskptr, block_super.inode_block_idx + inumber/128, inode_arr) < 0){
		printf("inode read failed");
		return -1;
	}

	inode* file_inode = &inode_arr[inumber%128];

	if(size >= file_inode->size) {
		return 0;
	}


	int indirect_data = 0;
	int data_blocks = ceiling((double)file_inode->size/4096);
	if(data_blocks > 5) indirect_data = file_inode->size - 4096*5;
	int indirect_data_blocks = ceiling((double)indirect_data / 4096);


	int offset = size;
	int curr_data_block = offset/ 4096;
	int num_data_block = ceiling((double)offset/ 4096);
	//printf("curr_data_block : %d\n", curr_data_block);

	if(num_data_block > 5) {
		int indirect_data_block_arr[1024];
		curr_data_block -= 5;
		
		if(read_block(mounted_diskptr, file_inode->indirect, indirect_data_block_arr) < 0) {
			printf("indirect_data block read failed\n");
			return -1;
		}

		if((offset % 4096) != 0) curr_data_block ++;

		for(int i=curr_data_block; i<1024 && i<indirect_data_blocks; i++) {
			int toFree_data_block = indirect_data_block_arr[i];
			int toFree_data_block_bitmap_block = (toFree_data_block - block_super.data_block_idx) / (8*4096); // getting block of bitmap of toFree_data_block

			int bitmap[1024];
			//printf("curr_data_block : %d\n toFree_data_block : %d\n toFree_data_block_bitmap_block : %d\n", curr_data_block, toFree_data_block, toFree_data_block_bitmap_block);
			if(read_block(mounted_diskptr, block_super.data_block_bitmap_idx + toFree_data_block_bitmap_block, bitmap) < 0) {
				printf("indirect data block bitmap read failed\n");
				return -1;
			}

			ClearBit(bitmap, (toFree_data_block - block_super.data_block_idx) % (8*4096));

			if(write_block(mounted_diskptr, block_super.data_block_bitmap_idx + toFree_data_block_bitmap_block, bitmap) < 0) {
				printf("indirect data block bitmap write failed\n");
				return -1;
			}
		}
	}
	else {
		if((offset % 4096) != 0) curr_data_block ++;

		for(int i=curr_data_block; i<data_blocks && i<5; i++) {
			int toFree_data_block = file_inode->direct[i];
			int toFree_data_block_bitmap_block = (toFree_data_block - block_super.data_block_idx) / (8*4096);

			int bitmap[1024];
			if(read_block(mounted_diskptr, block_super.data_block_bitmap_idx + toFree_data_block_bitmap_block, bitmap) < 0) {
				printf("data block bitmap read failed\n");
				return -1;
			}

			ClearBit(bitmap, (toFree_data_block - block_super.data_block_idx) % (8*4096));

			if(write_block(mounted_diskptr, block_super.data_block_bitmap_idx + toFree_data_block_bitmap_block, bitmap) < 0) {
				printf("data block bitmap write failed\n");
				return -1;
			}
		}

		int indirect_data_block_arr[1024];

		if(read_block(mounted_diskptr, file_inode->indirect, indirect_data_block_arr) < 0) {
			printf("indirect_data block read failed\n");
			return -1;
		}

		for(int i=0; i<1024 && i<indirect_data_blocks; i++) {
			int toFree_data_block = indirect_data_block_arr[i];
			int toFree_data_block_bitmap_block = (toFree_data_block - block_super.data_block_idx) / (8*4096); // getting block of bitmap of toFree_data_block

			int bitmap[1024];
			//printf("I am here \n");
			//printf("current block : %d\n", indirect_data_block_arr[i]);
			//printf("%d     %d : %d\n", indirect_data_blocks, i, block_super.data_block_bitmap_idx + toFree_data_block_bitmap_block);
			if(read_block(mounted_diskptr, block_super.data_block_bitmap_idx + toFree_data_block_bitmap_block, bitmap) < 0) {
				printf("indirect data block bitmap read failed\n");
				return -1;
			}

			ClearBit(bitmap, (toFree_data_block - block_super.data_block_idx) % (8*4096));

			if(write_block(mounted_diskptr, block_super.data_block_bitmap_idx + toFree_data_block_bitmap_block, bitmap) < 0) {
				printf("indirect data block bitmap write failed\n");
				return -1;
			}
		}

		int bitmap[1024];

		if(read_block(mounted_diskptr, block_super.data_block_bitmap_idx + (file_inode->indirect - block_super.data_block_idx) / (8*4096), bitmap) < 0) {
			printf("indirect data block bitmap read failed\n");
			return -1;
		}

		ClearBit(bitmap, (file_inode->indirect - block_super.data_block_idx) % (8*4096));

		if(write_block(mounted_diskptr, block_super.data_block_bitmap_idx + (file_inode->indirect - block_super.data_block_idx) / (8*4096), bitmap) < 0) {
			printf("indirect data block bitmap write failed\n");
			return -1;
		}
		file_inode->indirect = 0;
	}


	file_inode->size = size;

	if(write_block(mounted_diskptr, block_super.inode_block_idx + inumber/128, inode_arr) < 0) {
		printf("inode_arr write failed\n");
		return -1;
	}

	free(buff);

	return 0;

}


int get_inumber(char *filepath, int* par, int* file_flag) {
	if(mounted_diskptr == NULL) {
		printf("no valid diskptr mounted\n");
		return -1;
	}

	char* buff = (char*)malloc(4096);
	
	read_block(mounted_diskptr, 0, buff);
	super_block block_super = *((super_block*)buff);

	free(buff);

	char file[MAX_LENGTH+1];

	int i=0, n=strlen(filepath);
	int curr_inode = block_super.inode_block_idx;

	int slashes = 0;
	for(int j=0;j<n;j++)
		if(*(filepath + j) == '/') slashes ++;
	
	int found = 0;

	while(i < n) {
		int k = 0;
		if(*(filepath+i) == '/') i++, slashes--;
		while(i<n && *(filepath + i) != '/') {
			file[k++] = *(filepath + i);
			i++;
		}
		file[k]='\0';

		char* dir_lst = (char*)malloc(4096);

		found = 0;

		int l = 0;

		int inumber = curr_inode - block_super.inode_block_idx;

		inode inode_arr[128];

		if(read_block(mounted_diskptr, block_super.inode_block_idx + inumber/128, inode_arr) < 0){
			printf("inode read failed");
			return -1;
		}

		inode* file_inode = &inode_arr[inumber%128];

		while(l*4096 < file_inode->size) {
			if(read_i(curr_inode, dir_lst, 4096, l*4096) < 0) {
				printf("directory list read failed\n");
				return -1;
			}

			dir* dir_lst_new = (dir*)dir_lst;

			for(int j=0;j<64;j++) {
				//if(dir_lst_new[j].valid == 1) printf("%s   %s %d\n", dir_lst_new[j].name, file, (strcmp(dir_lst_new[j].name, file) == 0));
				if((dir_lst_new[j].valid == 1) && (strcmp(dir_lst_new[j].name, file) == 0)) {
					*par = curr_inode;
					curr_inode = dir_lst_new[j].inumber;
					//printf("here I am \n%s\n", filepath);
					found = 1;
					break;
				}
			}
			if(found == 1) break;
			l++;
		}
		if((slashes == 1) && (i == n-1)) {
			*file_flag = 0;
			break;
		}
		free(dir_lst);
	}

	if(found == 0) {
		printf("No such file exists\n");
		return -1;
	}
	return curr_inode;
}


int read_file(char *filepath, char *data, int length, int offset) {
	int par = 0;
	int file_flag = 0;
	int curr_inode = get_inumber(filepath, &par, &file_flag);

	if(curr_inode == -1) {
		return -1;
	}

	return read_i(curr_inode, data, length, offset);
}


int write_file(char *filepath, char *data, int length, int offset) {
	int par = 0;
	int file_flag = 0;
	int curr_inode = get_inumber(filepath, &par, &file_flag);

	if(curr_inode == -1) {
		return -1;
	}

	return write_i(curr_inode, data, length, offset);
}


int create_root_dir() {
	if(mounted_diskptr == NULL) {
		printf("no valid diskptr mounted\n");
		return -1;
	}

	char* buff = (char*)malloc(4096);
	
	read_block(mounted_diskptr, 0, buff);
	super_block block_super = *((super_block*)buff);

	free(buff);

	dir dir_lst[64];
	for(int i=0; i<64; i++) {
		dir_lst[i].valid = 0;
	}

	int dir_inode = create_file();
	if(dir_inode < 0) {
		printf("couldn't create root dir\n");
		return -1;
	}

	dir_inode -= block_super.inode_block_idx;

	inode inode_arr[128];

	if(read_block(mounted_diskptr, block_super.inode_block_idx + dir_inode/128, inode_arr) < 0){
		printf("inode read failed");
		return -1;
	}

	inode* file_inode = &inode_arr[dir_inode%128];

	//printf("size of dir list : %d\n", sizeof(dir_lst));
			
	if(write_i(dir_inode + block_super.inode_block_idx, (char*)dir_lst, sizeof(dir_lst), file_inode->size) < 0) {
		printf("directory list initialzation failed\n");
		return -1;
	}

	//print_dir(dir_lst);

	return dir_inode + block_super.inode_block_idx;
}



int create_dir(char *dirpath) {
	if(mounted_diskptr == NULL) {
		printf("no valid diskptr mounted\n");
		return -1;
	}

	char* buff = (char*)malloc(4096);
	
	read_block(mounted_diskptr, 0, buff);
	super_block block_super = *((super_block*)buff);

	free(buff);

	char file[MAX_LENGTH+1];

	int i=0, n=strlen(dirpath);
	int curr_inode = block_super.inode_block_idx;

	int slashes = 0;
	for(int j=0;j<n;j++)
		if(*(dirpath + j) == '/') slashes ++;
	
	int found = 0;

	while(i < n) {
		int k = 0;
		if(*(dirpath+i) == '/') i++, slashes--;
		while(i<n && *(dirpath + i) != '/') {
			file[k++] = *(dirpath + i);
			i++;
		}
		file[k]='\0';

		char* dir_lst = (char*)malloc(4096);
		found = 0;

		int l = 0;

		int inumber = curr_inode - block_super.inode_block_idx;

		inode inode_arr[128];

		if(read_block(mounted_diskptr, block_super.inode_block_idx + inumber/128, inode_arr) < 0){
			printf("inode read failed");
			return -1;
		}

		//printf("%d\n", curr_inode);

		inode* file_inode = &inode_arr[inumber%128];

		//printf("curr_inode : %d\n", curr_inode);
		//printf("file: %s\n\n", file);
		while(l*4096 < file_inode->size) {
			if(read_i(curr_inode, dir_lst, 4096, l*4096) < 0) {
				printf("directory list initialzation failed\n");
				return -1;
			}

			dir* dir_lst_new = (dir*)dir_lst;

			for(int j=0;j<64;j++) {
				if(dir_lst_new[j].valid == 1 && strcmp(dir_lst_new[j].name, file) == 0) {
					curr_inode = dir_lst_new[j].inumber;
					found = 1;
					break;
				}
			}
			if(found == 1) break;
			l++;
		}
		if((slashes == 1) && (i == n-1)) {
			break;
		}
		free(dir_lst);
	}

	if(found == 0 && slashes == 1) {
		//printf("New directory to be created\n");
	}
	else if(found == 0 && slashes == 0) {
		//printf("New file to be created\n");
	}
	else if(found == 1 && slashes == 0) {
		printf("file already exists\n");
		return -1;
	}
	else if(found == 1 && slashes == 1){
		printf("Invalid directory\n");
		return -1;
	}

	//printf("found : %d\nfile : %s\n", found, file);

	int l = 0;

	int inumber = curr_inode - block_super.inode_block_idx;

	inode inode_arr[128];

	if(read_block(mounted_diskptr, block_super.inode_block_idx + inumber/128, inode_arr) < 0){
		printf("inode read failed");
		return -1;
	}

	inode* file_inode = &inode_arr[inumber%128];

	dir dir_lst[64];

	int done = 0;

	while(done == 0) {
		if(done == 1) break;
		while(l*4096 < file_inode->size) {
			//printf("curr_inode : %d\n", curr_inode);
			//printf("file size : %d\n", file_inode->size);
			if(read_i(curr_inode, (char*)dir_lst, 4096, l*4096) < 0){
				printf("directory list read failed\n");
				return -1;
			}

			for(int j=0; j<64; j++) {
				if(dir_lst[j].valid == 0) {
					dir_lst[j].valid = 1;
					dir_lst[j].type = 0;

					strcpy(dir_lst[j].name, file);
					//printf("slashes : %d\n", slashes);

					dir_lst[j].name_len = strlen(file);
					dir_lst[j].inumber = create_file();

					//printf("file : %s\n", dir_lst[j].name);

					if(dir_lst[j].inumber == -1) return -1;
					
					inumber = dir_lst[j].inumber;

					if(slashes == 1) dir_lst[j].type = 1;

					if(write_i(curr_inode, (char*)dir_lst, 4096, l*4096) < 0) {
						printf("directory list write failed\n");
						return -1;
					}
					done = 1;
					return inumber;
				}

			}
			//print_dir(dir_lst);
			if(done == 1) break;
			l++;
		}
		///*
		if(done == 1) break;
		for(int i=0; i<64; i++) dir_lst[i].valid = 0;

		//printf("%d\n", curr_inode);
		//printf("dir size before : %d\n", file_inode->size);
		if(write_i(curr_inode , (char*)dir_lst, sizeof(dir_lst), file_inode->size) < 0) {
			printf("directory list write failed\n");
			return -1;
		}
		if(read_block(mounted_diskptr, block_super.inode_block_idx + inumber/128, inode_arr) < 0){
			printf("inode read failed");
			return -1;
		}

		file_inode = &inode_arr[inumber%128];
		//printf("dir size after : %d\n", file_inode->size);
		//*/
	}
	return inumber + block_super.inode_block_idx;
}




int print_all_dir(int curr_inode, int tb) {
	if(mounted_diskptr == NULL) {
		printf("no valid diskptr mounted\n");
		return -1;
	}

	char* buff = (char*)malloc(4096);
	
	read_block(mounted_diskptr, 0, buff);
	super_block block_super = *((super_block*)buff);

	free(buff);

	int inumber = curr_inode - block_super.inode_block_idx;

	inode inode_arr[128];

	if(read_block(mounted_diskptr, block_super.inode_block_idx + inumber/128, inode_arr) < 0){
		printf("inode read failed");
		return -1;
	}

	inode* file_inode = &inode_arr[inumber%128];

	int i = 0;
	while(i*4096 < file_inode->size) {
		char* dir_lst = (char*)malloc(4096);

		if(read_i(curr_inode, dir_lst, 4096, i*4096) < 0) {
			printf("directory list read failed\n");
			return -1;
		}

		dir* dir_lst_new = (dir*)dir_lst;

		//print_dir(dir_lst_new);

		for(int k=0; k<64; k++) {
			if(dir_lst_new[k].valid == 1) {
				int tbt = tb;
				while(tbt--) printf("  ");
				printf("->");
				printf("%s\t %d\n", dir_lst_new[k].name, dir_lst_new[k].inumber);
				//stat(dir_lst_new[k].inumber);
				if(dir_lst_new[k].type == 1) print_all_dir(dir_lst_new[k].inumber, tb+1);
			}
		}
		
		i++;
		free(dir_lst);
	}
}


int remove_dir_in(int curr_inode, int par_inode, int file_flag) {
	if(mounted_diskptr == NULL) {
		printf("no valid diskptr mounted\n");
		return -1;
	}

	char* buff = (char*)malloc(4096);
	
	read_block(mounted_diskptr, 0, buff);
	super_block block_super = *((super_block*)buff);

	free(buff);

	int inumber = curr_inode - block_super.inode_block_idx;

	//printf("curr_inode : %d\n", curr_inode);

	inode inode_arr[128];

	if(read_block(mounted_diskptr, block_super.inode_block_idx + inumber/128, inode_arr) < 0){
		printf("inode read failed");
		return -1;
	}

	inode* file_inode = &inode_arr[inumber%128];

	char* dir_lst = (char*)malloc(4096);

	int i = 0;

	if(file_flag == 0) {
		while(i*4096 < file_inode->size) {
			if(read_i(curr_inode, dir_lst, 4096, i*4096) < 0) {
				printf("directory list read failed\n");
				return -1;
			}

			dir* dir_lst_new = (dir*)dir_lst;

			//print_dir(dir_lst_new);

			for(int k=0; k<64; k++) {
				if(dir_lst_new[k].valid == 1) {
					if(dir_lst_new[k].type == 1) remove_dir_in(dir_lst_new[k].inumber, curr_inode, 0);
					if(remove_file(dir_lst_new[k].inumber) < 0) {
						printf("file name : %s remove failed", dir_lst_new[k].name);
						return -1;
					}
					else {
						//printf("file remove sucess:   %s\t %d\n", dir_lst_new[k].name, dir_lst_new[k].inumber);
					}
				}
			}
			
			i++;
		}
	}

	if(remove_file(curr_inode) < 0) {
		printf("file remove failed\n");
		return -1;
	}



	i = 0;

	inumber = par_inode - block_super.inode_block_idx;

	if(read_block(mounted_diskptr, block_super.inode_block_idx + inumber/128, inode_arr) < 0){
		printf("inode read failed");
		return -1;
	}

	file_inode = &inode_arr[inumber%128];

	int done = 0;
	
	while(i*4096 < file_inode->size) {
		if(read_i(par_inode, dir_lst, 4096, i*4096) < 0) {
			printf("directory list read failed\n");
			return -1;
		}

		dir* dir_lst_new = (dir*)dir_lst;

		//print_dir(dir_lst_new);

		for(int k=0; k<64; k++) {
			if((dir_lst_new[k].valid == 1) && (dir_lst_new[k].inumber == curr_inode)) {
				dir_lst_new[k].valid = 0;
				//printf(" curr_inode : %d\n par_inode : %d\n", curr_inode, par_inode);
				if(write_i(par_inode, (char*)dir_lst_new, 4096, i*4096) < 0) {
					printf("directory list write failed\n");
					return -1;
				}
				done = 1;
				break;
			}
		}

		if(done == 1) break;
		
		i++;
	}

	free(dir_lst);

	return 0;
}

int remove_dir(char *dirpath) {
	int par_inode = 0;
	int file_flag = 1;
	int curr_inode = get_inumber(dirpath, &par_inode, &file_flag);

	//printf("par inode is %d\n", par_inode);

	return remove_dir_in(curr_inode, par_inode, file_flag);
}
