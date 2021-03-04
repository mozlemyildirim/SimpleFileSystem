#include <stdio.h>
#include "structs.h"
#include <string.h>
#include <stdbool.h>

int ino_num = 0;
int dir_num = 0;
int tab = 0; 
int currentDir = 0;
int previousDir = 0;
char name[28];
char currentDirName[28] = "~";
char previousDirName[28] = "~";

void get_inode_struct(FILE *sfs, struct inode_st *inostr, int inode_num) //to get inode structures in sfs using for ls,mkdir and cd functions
{    
    fseek(sfs, sizeof(struct super_block), SEEK_SET); //fseek determines the location of the file
    fseek(sfs, sizeof(struct inode_st)*inode_num, SEEK_CUR);
    
    fread(inostr, sizeof(struct inode_st), 1, sfs); //read inode structure
}

void get_dir_entry(FILE *sfs, struct dir_entry *de, int db_num, int dir_entry_num) //to get direction entiry in sfs using for ls,mkdir and cd functions
{
    fseek(sfs, sizeof(struct super_block) + sizeof(struct inode_st)*32+ 512* db_num + dir_entry_num*sizeof(struct dir_entry), SEEK_SET);//fseek determines the location of the file
    fread(de, sizeof(struct dir_entry), 1, sfs); //read inode structure
}

void jump_inode(FILE *sfs, struct inode_st *inostr, int inode_num) //using for jump of inodes
{
	fseek(sfs, sizeof(struct super_block) + sizeof(struct inode_st) * inode_num, SEEK_SET);
}

void jump_dir_entry(FILE *sfs, struct dir_entry *de, int db_num, int dir_entry_num)  //using for jump of entries
{
	fseek(sfs, sizeof(struct super_block) + sizeof(struct inode_st) * 32 + 512 * db_num + dir_entry_num * sizeof(struct dir_entry), SEEK_SET);
}

int getBit(int bitnum, int n) { return (n >> bitnum) & 1; } //using for get bit in mkdir function

int setBit(int bitnum, int bitmap) { return bitmap ^ (1 << bitnum); } //using for set bit in mkdir function

int get(int bitmap) //using for get bitmap
{ 
	printf("Get bitmap:"); 
    int count = 0;
    int i=0;
    for ( i = 31; i >= 0; i--) { 
        count++;
        int k = bitmap >> i; 
        if (k & 1) 
            printf("1"); 
        else
        {
            printf("0"); 
        } 
    }
    printf("\n");
} 


void ls(FILE *sfs, int ino_num, int tab) {
	/* LS KOMUTUMUZU BURADA CALISTIRIYORUZ: BULUNDUGUMUZ DIZINDE LISTELEMEYE YARAR*/
    struct inode_st inostr;
	struct dir_entry de;
	get_inode_struct(sfs,&inostr,ino_num);
    tab++;
	int i;
	for(i = 0; i < inostr.size/32; i++){
        get_dir_entry(sfs,&de,inostr.data_block_indices[0],i); //get directory entries

        if(strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0) 
        {
            if(inostr.type == DIRECTORY) {
                // directory olmayana kadar recursive olarak tekrar tekrar ekrana dosyalarý bastýrýr.
                printf("%*s%s", tab * 5, "", de.name);
                
                printf("\t\t%d\n", ino_num);
                ls(sfs, de.inode_num, tab);
            }
        }
	}
    tab--;
}

void cd(FILE *sfs, char name[28], int ino_num) {
	/*CD KOMUTU CHANGE DIRECTORY YANI DOSYAYA GIRIS YAPAR */
	char str1[28];
    char str2[28];
    int ret;
    bool flag = false;
    
    struct inode_st inostr;
	struct dir_entry de;
	get_inode_struct(sfs,&inostr,ino_num);//get inode structure in sfs

	int i;
	for(i = 0; i < inostr.size/32; i++){ 
        get_dir_entry(sfs,&de,inostr.data_block_indices[0],i);//get directory entry in sfs
        
        if(inostr.type == 1) {
            strcpy(str1, de.name); //write str1 in de.name
            strcpy(str2, name);
            ret = strcmp(str1, str2); // If str1 and str2 same,return 0
            if(ret == 0) {
                strcpy(previousDirName, currentDirName); 
                strcat(currentDirName, "/");  //currentDirin sonuna / ekler
                strcat(currentDirName, de.name); 
                previousDir = currentDir;
                currentDir = de.inode_num;
                flag = true;
            }

            if(strcmp(name, "..") == 0) {
                get_inode_struct(sfs, &inostr, currentDir); //get inode structures
                get_dir_entry(sfs, &de, inostr.data_block_indices[0], 1); //get dir entries
                previousDir = de.inode_num;
                currentDir = de.inode_num;
                if(currentDir == 0) {
                    strcpy(currentDirName, "˜ ");
                }
                else {
                    char character = '/'; 
                    char* ptr = strrchr(currentDirName, character);
                    *ptr = '\0';
                }
                return;
            }
            ino_num++; 
        }
	}
    if(!flag)
        printf("-bash: cd: %s: No such file or directory\n", name);
}



void mkdir(FILE* sfs, int currentDir, char name[28]) {
    struct super_block sblock;
    struct inode_st inostr;
    struct dir_entry de;
    int ino_num = currentDir;
    fseek(sfs, 0, SEEK_SET);
    fread(&sblock, sizeof(struct super_block), 1, sfs);

    //region sblock
    bool flag = false;
    int db_array = 0;
    int db_num = 0;
    int db_bit;
    get(sblock.data_bitmap[0]);
    printf("Data bitmap: %d\n", sblock.data_bitmap[0]);

    for(db_array = 0; db_array < 10; db_array++) {
        while(db_num < 32) {
            if(getBit(db_num, sblock.data_bitmap[db_array]) == 0) {
                flag = true; // if datablock is empty break the loop so that we store the number
                db_bit = setBit(db_num, sblock.data_bitmap[db_array]); // setBit and return the bitmap to db_bit for ex; [1][1][0][1] -> [1][1][1][1] | so it should be stored in -> sblock.datablocks[] but needs override
                break;
                // data str da bos olaný bulup onu dolduruyoruz.
            }
            db_num++;
        }
        if(flag) break;
    }

    printf("Db indices: %d\n", db_array);
    printf("Db number: %d\n", db_num);
    printf("Inode bitmap: %d\n", sblock.inode_bitmap);
    printf("New inodebitmap\n",get(sblock.inode_bitmap));
    printf("\n");
    
    //region inodestr
    int inode_bit;
    int inode = 0;

    for(inode = 0; inode < 32; inode++) {
        printf("\tinodes: %d", inode);
        if(getBit(inode, sblock.inode_bitmap) == 0) 
        {
            inode_bit = setBit(inode, sblock.inode_bitmap); // setBit and return the bitmap to inode_bit for ex; [1][1][0][1] -> [1][1][1][1] |so it should be stored in -> sblock.inode_bitmap but needs override
            break;
        }
        // ayný islemi metadata için yaptýk
    }
    printf("Inode: %d\n", inode);

    //region new_inode
    struct inode_st dirInode;
	dirInode.type = DIRECTORY;
	dirInode.size = DESIZE * 2;
	dirInode.data_block_indices[db_array] = db_num;

    jump_inode(sfs, &inostr, inode);
	fwrite(&dirInode, sizeof(dirInode), 1, sfs);
    

    //region new_dir_entry
    struct dir_entry dot;
	strcpy(dot.name, ".");
	dot.inode_num = inode;

	struct dir_entry dotdot;
	strcpy(dotdot.name, "..");
	dotdot.inode_num = currentDir;
    	jump_dir_entry(sfs, &de, db_array * db_num + db_num, 0);
	fwrite(&dot, sizeof(dot), 1, sfs);
  	fwrite(&dotdot, sizeof(dotdot), 1, sfs);

    //region parent_dir_entry
    struct dir_entry dir;
    get_inode_struct(sfs, &inostr, currentDir);
    printf("Name: %s\n", name);
	strcpy(dir.name, name);
	printf("Dirname: %s\n", dir.name);
	dir.inode_num = inode;
  	printf("Inode Structures Size: %d\n", inostr.size);
    jump_dir_entry(sfs, &de, inostr.data_block_indices[0], inostr.size/32);
	fwrite(&dir, sizeof(dir), 1, sfs);
	get_dir_entry(sfs, &de, inostr.data_block_indices[0], 64);
	//printf("Dir entry name: %s\n", de.name);
	
    //region parent_inode
    get_inode_struct(sfs,&inostr,currentDir);
    
    inostr.size += DESIZE;
    jump_inode(sfs, &inostr, currentDir);
	fwrite(&inostr, sizeof(inostr), 1, sfs);

    //region override_superblock
    sblock.inode_bitmap = inode_bit;
    sblock.data_bitmap[db_array] = db_bit;
    fseek (sfs, 0, SEEK_SET);
	fwrite(&sblock, sizeof(sblock), 1, sfs);
	
    printf("Databitmap:\n");
    get(sblock.data_bitmap[0]);
    printf("Inodebitmap:\n");
    get(sblock.inode_bitmap);
    printf("\n");
}
int main() {
    
    FILE *sfs = fopen("sfs.bin", "r+");
	struct dir_entry de;
	//struct 
    char command[32];
    while(1) {

        printf("\033[1;32m\03user@AlpOzlem:\033[1;34m\03%s\033[0m\03$ ", currentDirName);
        //ekrana þuan bulunduðumuz directory name bastýrdýk
		scanf("%s",command);//read input
        if(strcmp(command, "ls") == 0) { 
            printf("/\n");
            ls(sfs, 0, tab); //if command equals ls,called ls function
        }
        else if (strcmp(command,"mkdir") == 0) {
            //mkdir(sfs, name);
            scanf("%s", name);
            mkdir(sfs, currentDir, name); //if command equals mkdir,called mkdir function
        }
        else if (strcmp(command,"cd") == 0) {
            //mkdir(sfs, name);
            scanf("%s", name);
            cd(sfs, name, currentDir); //if command equals cd,called cd function
        }
		else {
            printf("%s: command not found\n", command); //not found
        }

    }
}



































