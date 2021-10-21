#include "disk.h"
#include "sfs.h"
#include <stdio.h>
#include <stdlib.h>


/*
void print_super_block(super_block block_super) {
	printf("block_super magic number ---> %d\n",block_super.magic_number);
	printf("block_super blocks ---> %d\n",block_super.blocks);
	printf("block_super inode_blocks ---> %d\n",block_super.inode_blocks);
	printf("block_super inodes ---> %d\n",block_super.inodes);
	printf("block_super inode_bitmap_block_id ---> %d\n",block_super.inode_bitmap_block_idx);
	printf("block_super inode_block_idx ---> %d\n",block_super.inode_block_idx);
	printf("block_super data_block_bitmap_idx ---> %d\n",block_super.data_block_bitmap_idx);
	printf("block_super data_block_idx ---> %d\n",block_super.data_block_idx);
	printf("block_super data_blocks ---> %d\n",block_super.data_blocks);
}
*/


int main() {
	char* buff = (char*)malloc(4096);
	
	disk* diskptr = create_disk(4096*100+24);
	
	if(diskptr == NULL) {
		printf("create_disk   ---->  couldn't create_disk disk\n");
		return 0;
	}

	/*
	//printf("size of char %d\n",sizeof(char));
	for(int i=0; i<4096; i++) {
		data[i] = '1'; 
	}

	if(write_block(diskptr, 3, data) < 0) {
		printf("write_block ---> not able to write\n");
	}
	else {
		printf("write_block ---> write successfully\n");

		if(read_block(diskptr, 3, buff) < 0) {
			printf("read_block ---> not able to read\n");		
		}
		else {
			printf("read_block ---> read successfully\n");
			//printf("%s\n", buff);
		}
	}
	*/

	if(format(diskptr) < 0) {
		printf("couldn't format the diskptr\n");
	}

	/*

	super_block* block_super;
	if(read_block(diskptr, 0, buff) < 0) {
		printf("couldn't read super block\n");
	}
	else {
		block_super = (super_block*)buff;
		//printf("%s\n",buff);
		printf("successfully read super block\n");
		print_super_block(*block_super);
	}

	int bitmap[1024];
	for(int i=block_super->inode_bitmap_block_idx; i<block_super->data_block_bitmap_idx; i++)
		if(read_block(diskptr, i, bitmap) < 0){
			printf("inode bitmap read failed");
			return -1;
		}

	for(int i=block_super->data_block_bitmap_idx; i<block_super->data_block_idx; i++)
		if(read_block(diskptr, i + block_super->data_block_bitmap_idx, bitmap) < 0){
			printf("data bitmap read failed");
			return -1;
		}

	inode inode_arr[128];


	for(int i=block_super->inode_block_idx; i<block_super->inode_blocks; i++)
		if(read_block(diskptr, i, inode_arr) < 0){
			printf("data bitmap initialization failed");
			return -1;
		}
	*/

	if(mount(diskptr) < 0) {
		printf("couldn't mount the diskptr\n");
	}
	else {
		printf("mounting successful\n");
	}

	int file_inode = create_file();
	if(file_inode < 0) {
		printf("file creation failed\n");
	}
	else {
		printf("\n--------------------------\nfile creation successful\n");
		printf("file inode is %d\n--------------------------\n\n", file_inode);

		//char data[] = "I am Gaurav and You r a fool. Hahahhahhah";

		char data[] = "Goals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over again";

		int wr = write_i(file_inode, data, sizeof(data), 0); 
		wr = write_i(file_inode, data, sizeof(data), sizeof(data));
		if( wr < 0) {
			printf("Sorry bro!!! , nice try though!!\n");
		}
		else {
			printf("Yahooooo   wrote %d bytes\n", wr);
			char* data1 = (char*)malloc(4096*5);

			int rd = read_i(file_inode, data1, 4096*5, 10);
			if(rd < 0) {
				printf("fix the read!!\n");
			}
			else {
				printf("read_i successful\nread %d bytes\n%s\n", rd, data1);
			}
		}

		stat(file_inode);

		/*

		if(remove_file(file_inode) < 0) {
			printf("failed to remove file\n");
		}
		else {
			printf("file remove successful\n");
			stat(file_inode);
		}
		*/
	}


	file_inode = create_file();
	if(file_inode < 0) {
		printf("file creation failed\n");
	}
	else {
		printf("\n--------------------------\nfile creation successful\n");
		printf("file inode is %d\n--------------------------\n\n", file_inode);

		char data[] = "I am Gaurav and You r a fool. Hahahhahhah";

		//char data[] = "Goals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over againGoals: Solve cp problems as much as possible Ethical Hacking  Academic Courses me machana h Learn whatever the heck you want finance, law and psychology Machine learning bhi shayad Don’t fucking waste time, that’s all PS: It’s good to feel adrenaline in your blood. Wanna relive that feeling over and over again";

		int wr = write_i(file_inode, data, sizeof(data), 0); 
		//wr = write_i(file_inode, data, sizeof(data), sizeof(data));
		if( wr < 0) {
			printf("Sorry bro!!! , nice try though!!\n");
		}
		else {
			printf("Yahooooo   wrote %d bytes\n", wr);
			char* data1 = (char*)malloc(4096*5);

			int rd = read_i(file_inode, data1, 4096*5, 10);
			if(rd < 0) {
				printf("fix the read!!\n");
			}
			else {
				printf("read_i successful\nread %d bytes\n%s\n", rd, data1);
			}
		}

		stat(file_inode);

		if(remove_file(file_inode) < 0) {
			printf("failed to remove file\n");
		}
		else {
			printf("file remove successful\n");
			stat(file_inode);
		}
	}









	free_disk(diskptr);
	//printf("main.c     ####   free disk ---> %d\n", free_disk(diskptr));

	return 0;
}