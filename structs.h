#define REGFILE 0
#define DIRECTORY 1
#define F 2
#define DBSIZE 512
#define DESIZE 32
#define INODENUMBER 32

struct super_block{
	int inode_bitmap;
	int data_bitmap[10];
};

struct inode_st{
	int type;
	int size;
	int data_block_indices[10];
};

struct dir_entry{
	char name[28];
	int inode_num;
};
