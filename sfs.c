#include "structs.h"
#include <stdio.h>
#include <string.h>

int main(){
	FILE *sfs = fopen("sfs.bin", "w");
	struct super_block sb; //create super block
	sb.inode_bitmap = 1; //write 1 in inode bitmap of sb
	int i=0; //just for loop
	for( i = 0; i < 10; i++) //create data bitmap
		sb.data_bitmap[i] = 0;
	sb.data_bitmap[0] = 1; //first value change 1

	fwrite(&sb, sizeof(sb),1,sfs); //writing super block in sfs.bin

	struct inode_st root; //create inode structure
	root.type = DIRECTORY; //DIRECTORY=1  root type equals 1
	root.size = DESIZE * 2; //. and .. DIRENTRYSIZE=32 root size equals direntsize power of 2
	root.data_block_indices[0] = 0; //write 0 in root.datablockindices[0]

	fwrite(&root,sizeof(root),1,sfs); //writing inode structure in sfs.bin

	struct dir_entry dot;  //create dir entry dot
	strcpy(dot.name, "."); //write . in dot.name
	dot.inode_num = 0; //write 0 in dot.inode_num

	struct dir_entry dotdot; //create dir entry dotdot
	strcpy(dotdot.name, ".."); //write .. in dotdot.name
	dotdot.inode_num = 0; //write 0 in dotdot.inode_num
	
	fseek(sfs,sizeof(sb)+INODENUMBER*sizeof(struct inode_st) ,SEEK_SET); // INODENUMBER=32 IN STRUCT fseek determines the location of the file
	
	fwrite(&dot,sizeof(dot),1,sfs); //write dot in sfs.bin
	fwrite(&dotdot, sizeof(dotdot),1,sfs);	 //write dotdot in sfs.bin
	
	fflush(sfs);//used to clean the buffer memory
	
	//we created simplefilesystem in sfs.bin

	return 0;
}
