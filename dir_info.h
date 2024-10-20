#ifndef DIR_INFO_H
#define DIR_INFO_H

typedef struct dir_info {
	/* Exclusive to long format */
	long total_blocks;
	int max_links_width;
	int max_owner_width;
	int max_group_width;
	int max_size_width;
	int max_day_width;
	int max_date_time_width;

	/* Short or long format */
	int max_inode_width;
	int max_block_size_width;
	long blocksizep;
} dir_info;

#endif