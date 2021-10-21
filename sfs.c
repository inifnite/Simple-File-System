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
	printf("\n\n----------------\n");
	printf("block_super magic number ---> %d\n", block_super.magic_number);
	printf("block_super blocks ---> %d\n", block_super.blocks);
	printf("block_super inode_blocks ---> %d\n", block_super.inode_blocks);
	printf("block_super inodes ---> %d\n", block_super.inodes);
	printf("block_super inode_bitmap_block_id ---> %d\n", block_super.inode_bitmap_block_idx);
	printf("block_super inode_block_idx ---> %d\n", block_super.inode_block_idx);
	printf("block_super data_block_bitmap_idx ---> %d\n", block_super.data_block_bitmap_idx);
	printf("block_super data_block_idx ---> %d\n", block_super.data_block_idx);
	printf("block_super data_blocks ---> %d\n", block_super.data_blocks);
	printf("-----------------\n\n");
}



void print_inode(inode inode_) {
	printf("\n\n-----------------\n");
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

	if(read_block(mounted_diskptr, inode_.indirect, indirect_data_block_arr) < 0) {
		printf("indirect_data block read failed\n");
	}

	printf("inode indirect block number : %d\n", inode_.indirect);
	for(int i=0; i<1024 && i< ceiling((double)indirect_data/4096); i++) {
		printf("inode indirect block ###  data block %d : %d\n", i+1, indirect_data_block_arr[i]);
	}

	printf("-----------------\n\n");
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
		printf("successfully read super block\n");
		print_super_block(block_super);
	}

	if (block_super.magic_number != MAGIC) {
		printf("Invalid SFS\n");
		return -1;
	}
	mounted_diskptr = diskptr;
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

	print_inode(file_inode);

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

	if(num_data_block >= 5) {
		int indirect_data_block_arr[1024];
		curr_data_block =- 4;
		
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

		while(len < length && len < file_inode.size - offset && curr_data_block < indirect_data_blocks) {
			if(read_block(mounted_diskptr, indirect_data_block_arr[curr_data_block], buff) < 0) {
				printf("indirect data block read failed\n");
				return -1;
			}

			memcpy(data+len, buff, (length - len)<4096 ? (length - len):4096);
			len += (length - len)<4096 ? (length - len):4096;
			curr_data_block ++;
		}
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
		}

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

	if(num_data_block >= 5) {
		int indirect_data_block_arr[1024];
		curr_data_block =- 4;
		
		if(read_block(mounted_diskptr, file_inode->indirect, indirect_data_block_arr) < 0) {
			printf("indirect_data block read failed\n");
			return -1;
		}

		int remain = 4096 - (offset - 4096*5)%4096;

		if(offset != 0) {
			if(read_block(mounted_diskptr, indirect_data_block_arr[curr_data_block], buff) < 0) {
				printf("indirect data block read failed\n");
				return -1;
			}

			memcpy(buff + offset % 4096, data, remain<length ? remain : length);

			if(write_block(mounted_diskptr, indirect_data_block_arr[curr_data_block], buff) < 0) {
				printf("indirect data block write failed\n");
				return -1;
			}
			len += remain<length ? remain : length;
			curr_data_block ++;
		}

		// overwriting
		while(len < length && len < file_inode->size - offset && curr_data_block < indirect_data_blocks) {
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

						indirect_data_block_arr[curr_data_block ++] = free_data_block_from_start;
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

		if(offset != 0) {
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

		if(len < length) {
			printf("I am here\n");
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

						indirect_data_block_arr[curr_data_block ++] = block_super.data_block_idx + free_data_block_from_start;
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

	file_inode->size += len;

	if(write_block(mounted_diskptr, block_super.inode_block_idx + inumber/128, inode_arr) < 0){
		printf("inode read failed");
		return -1;
	}
	return len;
}


int read_file(char *filepath, char *data, int length, int offset);
int write_file(char *filepath, char *data, int length, int offset);
int create_dir(char *dirpath);
int remove_dir(char *dirpath);
