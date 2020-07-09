//NAME: Shrenik Manoj Kankaria,Dhruv Singhania
//EMAIL: shrenik0123@gmail.com,singhania_dhruv@yahoo.com
//ID: 005176857,105125631

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>
#include <stdint.h>
#include "ext2_fs.h"

int image_fd = -1; //image file descriptor
const int SB_OFFSET = 1024; //superblock offset
const int SB_SIZE = 1024; //superblock size
struct ext2_super_block sb;
__u32 block_size;
int num_groups = 1;
struct ext2_group_desc* group_desc_arr;

#define EXT2_S_IFDIR 0x4000
#define EXT2_S_IFLNK 0xA000
#define EXT2_S_IFREG 0x8000

void handle_error(char* msg, int code) { //error handling helper function
	fprintf(stderr, "%s\n", msg);
	exit(code);
}

void handle_exit() { //closing function called at exit
	free(group_desc_arr);
	close(image_fd);
}

void print_superblock() //prints the superblock
{
	fprintf(stdout, "%s,%u,%u,%u,%u,%u,%u,%u\n", "SUPERBLOCK",
		sb.s_blocks_count, //total number of blocks
		sb.s_inodes_count, //total number of inodes
		block_size, //block size
		sb.s_inode_size, //inode size
		sb.s_blocks_per_group, //blocks per group
		sb.s_inodes_per_group, //inodes per group
		sb.s_first_ino); //first non-reserves inode
}


void print_group(int group_num, int total_groups) //prints the group summary
{
	__u32 offset = SB_OFFSET + block_size + group_num * sizeof(struct ext2_group_desc); //calculates group offset
	pread(image_fd, &group_desc_arr[group_num], sizeof(struct ext2_group_desc), offset);
	__u32 num_blocks_in_group = sb.s_blocks_per_group; //calculates number of blocks in group
	if (group_num == total_groups - 1) {
		num_blocks_in_group = sb.s_blocks_count - (sb.s_blocks_per_group * (total_groups - 1));
	}

	__u32 num_inodes_in_group = sb.s_inodes_per_group; //calculates number of inodes in group
	if (group_num == total_groups - 1) {
		num_inodes_in_group = sb.s_inodes_count - (sb.s_inodes_per_group * (total_groups - 1));
	}
	fprintf(stdout, "%s,%d,%u,%u,%u,%u,%u,%u,%u\n", "GROUP",
		group_num, //group number
		num_blocks_in_group, //total number of blocks in this group
		num_inodes_in_group, //total number of inodes in this group
		group_desc_arr[group_num].bg_free_blocks_count, //number of free blocks
		group_desc_arr[group_num].bg_free_inodes_count, //number of free inodes
		group_desc_arr[group_num].bg_block_bitmap, //block number of free block bitmap for this group
		group_desc_arr[group_num].bg_inode_bitmap, //block number of free inode bitmap for this group
		group_desc_arr[group_num].bg_inode_table); //block number of first block of inodes in this group
}


void print_free_block_entries(int group_num) //prints free block entries
{
	__u32 num_bitmap = group_desc_arr[group_num].bg_block_bitmap; //block number of free block bitmap
	char* bitmap = malloc(block_size * sizeof(char));
	if (bitmap == NULL) {
		free(bitmap);
		handle_error("Error allocating memory to print the free block entries", 2);
	}
	if (pread(image_fd, bitmap, block_size, num_bitmap * (int)block_size) < 0) {
		free(bitmap);
		handle_error("Error reading bitmap", 2);
	}

	int check = 1;
	unsigned i = 0;
	int j = 0;
	for (i = 0; i < block_size; ++i) { //traverses block bitmap
		char byte = bitmap[i];
		for (j = 0; j < 8; ++j) {
			int bit = byte & (check << j);
			int block_num = group_num * sb.s_blocks_per_group + i * 8 + 1 + j;
			if (bit == 0) //if free
				fprintf(stdout, "%s,%u\n", "BFREE", block_num); //number of free block
		}
	}
	free(bitmap);
}


void print_free_inode_entries(int group_num) //prints free inode entries
{
	__u32 num_bitmap = group_desc_arr[group_num].bg_inode_bitmap; //block number of free block bitmap
	char* bitmap = malloc(block_size * sizeof(char));
	if (bitmap == NULL) {
		free(bitmap);
		handle_error("Error allocating memory to print the free inode entries", 2);
	}
	if (pread(image_fd, bitmap, block_size, num_bitmap * (int)block_size) < 0) {
		free(bitmap);
		handle_error("Error reading bitmap", 2);
	}

	int check = 1;
	unsigned i = 0;
	int j = 0;
	for (i = 0; i < block_size; ++i) { //traverses inode bitmap
		char byte = bitmap[i];
		for (j = 0; j < 8; ++j) {
			int bit = byte & (check << j);
			int block_num = group_num * sb.s_inodes_per_group + i * 8 + 1 + j;
			if (bit == 0) //if free
				fprintf(stdout, "%s,%u\n", "IFREE", block_num); //number of free inode
		}
	}
	free(bitmap);
}


void print_block_references(uint32_t block_num, int inode_num, int block_level, uint32_t block_offset) //prints indirect block references
{
	__u32 block_val;
	if (block_level == 1) { //single indirect block
		for (__u32 i = 0; i < block_size / 4; i++) {
			if (pread(image_fd, &block_val, sizeof(__u32), (block_num * block_size + i * 4)) < 0) {
				handle_error("Error with pread()", 2);
			}
			if (block_val != 0) {
				fprintf(stdout, "INDIRECT,%u,1,%u,%u,%u\n",
					inode_num, //inode number of owning file
					block_offset + i, //logical block offset represented by referenced block
					block_num, //block number of indirect block being scanned
					block_val); //block number of the referenced block
			}
		}
	}
	else if (block_level == 2) { //double indirect block
		for (__u32 i = 0; i < block_size / 4; i++) {
			if (pread(image_fd, &block_val, sizeof(__u32), (block_num * block_size + i * 4)) < 0) {
				handle_error("Error with pread()", 2);
			}
			if (block_val != 0) {
				fprintf(stdout, "INDIRECT,%u,2,%u,%u,%u\n",
					inode_num,
					block_offset + i * (block_size / 4),
					block_num,
					block_val);
				print_block_references(block_val, inode_num, block_level - 1, block_offset + i * (block_size / 4)); //recursively calls itself
			}
		}
	}
	else if (block_level == 3) { //triple indirect block
		for (__u32 i = 0; i < block_size / 4; i++) {
			if (pread(image_fd, &block_val, sizeof(__u32), (block_num * block_size + i * 4)) < 0) {
				handle_error("Error with pread()", 2);
			}
			if (block_val != 0) {
				fprintf(stdout, "INDIRECT,%u,3,%u,%u,%u\n",
					inode_num,
					block_offset + i * (block_size * block_size / 8),
					block_num,
					block_val);
				print_block_references(block_val, inode_num, block_level - 1, block_offset + i * (block_size * block_size / 8)); //recursively calls itself
			}
		}
	}
}


void get_time(char* str, __u32 time_u32) //stores GMT time in str
{
	struct tm* time;
	time_t temp = time_u32;
	time = gmtime(&temp);
	if (time == NULL) {
		handle_error("Error getting time in GMT", 2);
	}
	sprintf(str, "%02d/%02d/%02d %02d:%02d:%02d",
		time->tm_mon + 1, //month
		time->tm_mday, //day
		(time->tm_year) % 100, //year
		time->tm_hour, //hour
		time->tm_min, //minute
		time->tm_sec); //second
}


void print_inode_summary(int group_num) //prints inode summary
{
	__u32 num_table = group_desc_arr[group_num].bg_inode_table; //block number of first block of inodes in this group
	size_t size = sizeof(struct ext2_inode) * sb.s_inodes_per_group; //calculates number of bytes to be read
	struct ext2_inode* total_inodes;
	total_inodes = (struct ext2_inode*) malloc(size);
	if (total_inodes == NULL) {
		free(total_inodes);
		handle_error("Error allocating memory for inode summary", 2);
	}
	if (pread(image_fd, total_inodes, size, (int)block_size * num_table) < 0) {
		free(total_inodes);
		handle_error("Error reading inodes from the image", 2);
	}

	for (unsigned int i = 0; i < sb.s_inodes_per_group; ++i) { //traverses all inodes in group
		int inode_num = group_num * sb.s_inodes_per_group + 1 + i;
		struct ext2_inode curr = total_inodes[i];
		if (curr.i_mode != 0 && curr.i_links_count != 0) { //unknown file type
			char type = '?';

			if ((curr.i_mode & S_IFMT) == S_IFREG) //file
				type = 'f';
			else if ((curr.i_mode & S_IFMT) == S_IFDIR) //directory
				type = 'd';
			else if ((curr.i_mode & S_IFMT) == S_IFLNK) //symbolic link
				type = 's';
			__u16 lower_order = 0x0FFF;
			__u16 mode = curr.i_mode & lower_order;

			char change_time[20];
			char mod_time[20];
			char last_time[20];
			get_time(change_time, curr.i_ctime);
			get_time(mod_time, curr.i_mtime);
			get_time(last_time, curr.i_atime);

			fprintf(stdout, "INODE,%u,%c,%o,%u,%u,%u,%s,%s,%s,%u,%u",
				inode_num, //inode number
				type, //file type
				mode, //mode
				curr.i_uid, //owner
				curr.i_gid, //group
				curr.i_links_count, //link count
				change_time, //time of last inode change
				mod_time, //modification time
				last_time, //time of last access
				curr.i_size, //file size
				curr.i_blocks); //number of blocks of disk space taken up by this file

			if (type == 'f' || type == 'd' || curr.i_size > 60) { //traverses block addresses for files and directories
				for (int j = 0; j < 15; ++j) {
					__u32 num_blocks = curr.i_block[j];
					fprintf(stdout, ",%u", num_blocks);
				}
			}
			fprintf(stdout, "\n");

			//printing directory entries
			int logical_byte_offset = 0;
			if (type == 'd') {
				struct ext2_dir_entry* entry = malloc(sizeof(struct ext2_dir_entry));
				for (unsigned i = 0; i < EXT2_NDIR_BLOCKS; ++i) {
					if (curr.i_block[i] != 0) {
						while ((unsigned)logical_byte_offset < block_size) {
							if (pread(image_fd, entry, sizeof(struct ext2_dir_entry), block_size * curr.i_block[i] + logical_byte_offset) < 0) {
								free(entry);
								handle_error("Error reading into directory", 2);
							}
							if (entry->inode != 0) {
								fprintf(stdout, "%s,%u,%d,%u,%u,%u,'%s'\n",
									"DIRENT",
									inode_num, //parent inode number
									logical_byte_offset, //logical byte offset of this entry within a directory
									entry->inode, //inode number of the referenced file
									entry->rec_len, //entry length
									entry->name_len, //name length
									entry->name); //name
							}
							logical_byte_offset += entry->rec_len;
						}
					}
				}
				free(entry);
			}//end of directory entries if block

			if (curr.i_block[EXT2_IND_BLOCK] != 0) { //calls print block references
				print_block_references(curr.i_block[EXT2_IND_BLOCK], inode_num, 1, 12);
			}
			if (curr.i_block[EXT2_DIND_BLOCK] != 0) {
				print_block_references(curr.i_block[EXT2_DIND_BLOCK], inode_num, 2, 12 + block_size / 4);
			}
			if (curr.i_block[EXT2_TIND_BLOCK] != 0) {
				print_block_references(curr.i_block[EXT2_TIND_BLOCK], inode_num, 3, 12 + block_size / 4 + block_size * block_size / 16);
			}
		} //end of if block
	} //end of for loop

	free(total_inodes);
}


int main(int argc, char* argv[])
{
	if (argc != 2) {
		handle_error("Incorrect number of arguments: \nUsage: ./lab3a filename", 1);
	}

	//atexit call to free all memory and close image_fd
	atexit(handle_exit);

	image_fd = open(argv[1], O_RDONLY); //opens image
	if (image_fd < 0) {
		handle_error("Error opening the file", 2);
	}

	if (pread(image_fd, &sb, sizeof(struct ext2_super_block), SB_SIZE) < 0) {
		handle_error("Could not read image", 2);
	}

	block_size = EXT2_MIN_BLOCK_SIZE << sb.s_log_block_size;
	print_superblock(); //calls respective print function

	num_groups = sb.s_blocks_count / sb.s_blocks_per_group; //calculates total number of groups
	if ((double)num_groups < (double)sb.s_blocks_count / sb.s_blocks_per_group)
		num_groups++;
	group_desc_arr = (struct ext2_group_desc*) malloc(num_groups * sizeof(struct ext2_group_desc));
	if (group_desc_arr == NULL) {
		handle_error("Error allocating memory to the array of groups", 2);
	}
	for (int i = 0; i < num_groups; ++i) {
		print_group(i, num_groups); //calls respective print function
	}
	for (int i = 0; i < num_groups; ++i) {
		print_free_block_entries(i); //calls respective print function
	}

	for (int i = 0; i < num_groups; ++i) {
		print_free_inode_entries(i); //calls respective print function
	}

	for (int i = 0; i < num_groups; ++i) {
		print_inode_summary(i); //calls respective print function
	}

	exit(0);
}
