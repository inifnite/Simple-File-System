// Gaurav Kumar Meena
// 18CS30019

#include<stdint.h>

const static uint32_t MAGIC = 12345;
#define MAX_LENGTH 48

typedef struct inode {
	uint32_t valid; // 0 if invalid
	uint32_t size; // logical size of the file
	uint32_t direct[5]; // direct data block pointer
	uint32_t indirect; // indirect pointer
} inode;


typedef struct super_block {
	uint32_t magic_number;	// File system magic number
	uint32_t blocks;	// Number of blocks in file system (except super block)

	uint32_t inode_blocks;	// Number of blocks reserved for inodes == 10% of Blocks
	uint32_t inodes;	// Number of inodes in file system == length of inode bit map
	uint32_t inode_bitmap_block_idx;  // Block Number of the first inode bit map block
	uint32_t inode_block_idx;	// Block Number of the first inode block

	uint32_t data_block_bitmap_idx;	// Block number of the first data bitmap block
	uint32_t data_block_idx;	// Block number of the first data block
	uint32_t data_blocks;  // Number of blocks reserved as data blocks
} super_block;


typedef struct dir{
	uint32_t valid;  // 0 if invalid
	uint32_t type;  // 0 if file and 1 if dir
	char name[MAX_LENGTH];  // name of the file/dir
	uint32_t name_len;  // length of the name
	uint32_t inumber;  // inumber of the file/dir
} dir;

int format(disk *diskptr);

int mount(disk *diskptr);

int create_file();

int remove_file(int inumber);

int stat(int inumber);

int read_i(int inumber, char *data, int length, int offset);

int write_i(int inumber, char *data, int length, int offset);

int fit_to_size(int inumber, int size);

int read_file(char *filepath, char *data, int length, int offset);
int write_file(char *filepath, char *data, int length, int offset);
int create_dir(char *dirpath);
int remove_dir(char *dirpath);


int create_root_dir(); //function to create root dir
int print_all_dir(int inumber, int tb); //helper function to print all directory with inode number inumber,   tb is for printing tabs