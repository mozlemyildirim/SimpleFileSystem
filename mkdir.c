#include "structs.h"
#include <stdio.h>
#include <string.h>

int main() {
	FILE *sfs = fopen("sfs.bin", "w"); //open file
	struct super_block sb; //create super block
	sb.inode_bitmap = 1; // inode bitmap set 1 in super block
	
	int i=0;// just for loop
	for( i = 0; i < 10; i++)
		sb.data_bitmap[i] = 0; //all data bitmap 0 
	sb.data_bitmap[0] = 1; // just 0.index set 1
	

	fwrite(&sb, sizeof(sb), 1, sfs);//write super block in sfs

	//INODE STRUCTURES
	//creating inode structure root
	struct inode_st root;
	root.type = DIRECTORY;
	root.size = DESIZE * 2;
	root.data_block_indices[0] = 0;
	fwrite(&root, sizeof(root), 1, sfs);//write inode structure in sfs

	//creating inode structure alpI
	struct inode_st alpI;
	alpI.type = DIRECTORY;
	alpI.size = DESIZE * 3;
	alpI.data_block_indices[0] = 1;
	fwrite(&alpI, sizeof(alpI), 1, sfs);//write inode structure in sfs

	
	//creating inode structure ozlemI
	struct inode_st ozlemI;
	ozlemI.type = DIRECTORY;
	ozlemI.size = DESIZE * 2;
	ozlemI.data_block_indices[0] = 2;
	fwrite(&ozlemI, sizeof(ozlemI), 1, sfs);//write inode structure in sfs

	//DIRENTRIES

	//1 .
	struct dir_entry dot;
	strcpy(dot.name, ".");
	dot.inode_num = 0;

	//2 ..
	struct dir_entry dotdot;
	strcpy(dotdot.name, "..");
	dotdot.inode_num = 0;

	//3 alpEntry
	struct dir_entry alpE;
	strcpy(alpE.name, "alp");
	alpE.inode_num = 1;


	fseek(sfs, sizeof(sb) + INODENUMBER * sizeof(struct inode_st) + 512 * 0, SEEK_SET); //fseek determines the location of the file
	fwrite(&dot, sizeof(dot), 1, sfs); //write dot entry in sfs
	fwrite(&dotdot, sizeof(dotdot), 1, sfs);//write dotdot entry in sfs
	fwrite(&alpE, sizeof(alpE), 1, sfs);//write alpE entry in sfs


	strcpy(dot.name, "."); //write . in dot.name
	dot.inode_num = 1;

	strcpy(dotdot.name, ".."); //write .. in dotdot.name
	dotdot.inode_num = 0;

	//4 ozlemEntry
	struct dir_entry ozlemE;
	strcpy(ozlemE.name, "ozlem");
	ozlemE.inode_num = 3;

	fseek(sfs, sizeof(sb) + INODENUMBER * sizeof(struct inode_st) + 512 * 1, SEEK_SET); //fseek determines the location of the file
	fwrite(&dot, sizeof(dot), 1, sfs);//write dot entry in sfs
	fwrite(&dotdot, sizeof(dotdot), 1, sfs);//write dotdot entry in sfs
	fwrite(&ozlemE, sizeof(ozlemE), 1, sfs);//write ozlemE entry in sfs
	
	//created inodes and entries

	fflush(sfs); //used to clean the buffer memory

	return 0;
}



















