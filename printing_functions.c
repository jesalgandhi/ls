#include "printing_functions.h"

#include <sys/statvfs.h>

#include <ctype.h>
#include <errno.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "flag_handlers.h"

void
switch_k_to_K(char *file_size_str)
{
	int i = FILESIZE_STR_SIZE;
	while (i > 0) {
		if (file_size_str[i] == 'k') {
			file_size_str[i] = 'K';
			break;
		}
		i--;
	}
}

void
sanitize_filename(const char *input, char *output, size_t size)
{
	size_t i = 0, j = 0;
	while (i < size - 1 && input[i] != '\0') {
		if (isprint((unsigned char)input[i])) {
			output[j++] = input[i];
		} else {
			output[j++] = '?';
		}
		i++;
	}
	output[j] = '\0';
}

void
sanitize_filename_malloc(const char *input, char **output)
{
	size_t i;
	size_t len = strlen(input);
	char *sanitized = (char *)malloc(len + 1);

	if (sanitized == NULL) {
		fprintf(stderr, "%s: %s\n", getprogname(), strerror(errno));
		exit(EXIT_FAILURE);
	}

	for (i = 0; i <= len; i++) {
		if (isprint((unsigned char)input[i]) || input[i] == '\0') {
			sanitized[i] = input[i];
		} else {
			sanitized[i] = '?';
		}
	}

	*output = sanitized;
}

void
print_children(FTSENT *children, ls_options *ls_opts, dir_info *di)
{
	FTSENT *child;
	char *filename;
	struct stat *sb;

	char type_indicator;
	char sym_str[SYMBOLIC_STRING_SIZE];
	char date_str[DATE_STRING_SIZE];
	char month_str[ABBREVIATED_MONTH_SIZE];
	char fullpath[PATH_MAX + 1];
	char link_target[PATH_MAX + 1];
	char inode_str[MAX_INODE_STR_SIZE];
	char blocks_str[MAX_BLOCKS_STR_SIZE];
	char file_size_str[FILESIZE_STR_SIZE];
	char pw_name_buf[20];
	char gr_name_buf[20];

	time_t t;
	struct tm curr_tm_info;
	struct tm file_tm_info;
	int curr_yr;
	int file_yr;

	struct passwd *pw;
	struct group *gr;
	ssize_t len;
	char *sanitized_link_target;
	long blocksizep;

	t = time(NULL);
	localtime_r(&t, &curr_tm_info);
	curr_yr = curr_tm_info.tm_year + 1900;

	getbsize(NULL, &blocksizep);

	if ((ls_opts->o_long_format || ls_opts->o_long_numeric_ids) &&
	    di->total_blocks >= 0) {
		/* blocks are in units of 512 bytes, so divide then * by block size
		 */
		if (ls_opts->o_human_readable_size) {
			if (humanize_number(
					file_size_str, sizeof(file_size_str),
					(di->total_blocks * 512) / blocksizep, NULL, HN_AUTOSCALE,
					HN_NOSPACE | HN_B | HN_DIVISOR_1000 | HN_DECIMAL) != -1) {

				switch_k_to_K(file_size_str);
				printf("total %s\n", file_size_str);
			} else {
				fprintf(stderr, "%s: %s", getprogname(), strerror(errno));
			}
		} else {
			printf("total %ld\n", (di->total_blocks * 512) / blocksizep);
		}
	}

	pw_name_buf[0] = '\0';
	gr_name_buf[0] = '\0';

	for (child = children; child != NULL; child = child->fts_link) {
		if ((child->fts_name[0] == '.') &&
		    (handle_hidden_files_a_A(child->fts_name, ls_opts) == 0)) {
			continue;
		}

		/* Contains sanitized filename or regular filename */
		filename = (char *)child->fts_pointer;

		inode_str[0] = '\0';
		blocks_str[0] = '\0';

		sb = child->fts_statp;

		localtime_r(&sb->st_mtime, &file_tm_info);
		file_yr = file_tm_info.tm_year + 1900;
		strmode(sb->st_mode, sym_str);
		strftime(month_str, sizeof(month_str), "%b", &file_tm_info);

		if (curr_yr != file_yr) {
			/* Year is different, display year instead of time */
			snprintf(date_str, sizeof(date_str), "%s %*d %*d", month_str,
			         di->max_day_width, file_tm_info.tm_mday,
			         di->max_date_time_width, file_yr);
		} else {
			/* Same year, display time */
			snprintf(date_str, sizeof(date_str), "%s %*d %02d:%02d", month_str,
			         di->max_day_width, file_tm_info.tm_mday,
			         file_tm_info.tm_hour, file_tm_info.tm_min);
		}

		/* Prepare inode and block size strings if needed */
		if (ls_opts->o_print_inode) {
			snprintf(inode_str, sizeof(inode_str), "%*ld ", di->max_inode_width,
			         (long)sb->st_ino);
		}

		if (ls_opts->o_display_block_usage) {
			snprintf(blocks_str, sizeof(blocks_str), "%*ld ",
			         di->max_block_size_width,
			         (long)(sb->st_blocks * 512) / blocksizep);
		}

		if (ls_opts->o_long_format || ls_opts->o_long_numeric_ids) {
			pw = getpwuid(sb->st_uid);
			gr = getgrgid(sb->st_gid);

			if (ls_opts->o_long_numeric_ids) {
				sprintf(pw_name_buf, "%ld", (long)pw->pw_uid);
				sprintf(gr_name_buf, "%ld", (long)gr->gr_gid);
			}

			if (ls_opts->o_human_readable_size) {
				if (humanize_number(file_size_str, sizeof(file_size_str),
				                    sb->st_size, NULL, HN_AUTOSCALE,
				                    HN_NOSPACE | HN_B | HN_DECIMAL) != -1) {
					switch_k_to_K(file_size_str);
					printf("%s%s%s %*ld %-*s  %-*s  %*s %s %s", inode_str,
					       blocks_str, sym_str, di->max_links_width,
					       (long)sb->st_nlink, di->max_owner_width,
					       pw_name_buf[0] != '\0' ? pw_name_buf : pw->pw_name,
					       di->max_group_width,
					       gr_name_buf[0] != '\0' ? gr_name_buf : gr->gr_name,
					       di->max_size_width, file_size_str, date_str,
					       filename);
				} else {
					fprintf(stderr, "%s: %s", getprogname(), strerror(errno));
				}
			} else {
				printf("%s%s%s %*ld %-*s  %-*s  %*ld %s %s", inode_str,
				       blocks_str, sym_str, di->max_links_width,
				       (long)sb->st_nlink, di->max_owner_width,
				       pw_name_buf[0] != '\0' ? pw_name_buf : pw->pw_name,
				       di->max_group_width,
				       gr_name_buf[0] != '\0' ? gr_name_buf : gr->gr_name,
				       di->max_size_width, (long)sb->st_size, date_str,
				       filename);
			}

			pw_name_buf[0] = '\0';
			gr_name_buf[0] = '\0';


			if (ls_opts->o_type_indicate &&
			    (type_indicator = type_indicate_F(sb->st_mode)) != ' ') {
				printf("%c", type_indicator);
			}

			/* Check for symlinks */
			if (S_ISLNK(sb->st_mode)) {

				/* Path needs to be constructed for symlink because we are
				 * traversing children as a dir */
				strlcpy(fullpath, child->fts_path, sizeof(fullpath));
				len = strlen(fullpath);
				if (len > 0 && fullpath[len - 1] != '/') {
					strlcat(fullpath, "/", sizeof(fullpath));
				}
				strlcat(fullpath, child->fts_name, sizeof(fullpath));

				len = readlink(fullpath, link_target, sizeof(link_target) - 1);
				if (len != -1) {
					link_target[len] = '\0';
					/* Sanitize link target if required */
					if (!ls_opts->o_raw_print_non_printable) {
						sanitize_filename_malloc(link_target,
						                         &sanitized_link_target);
					} else {
						sanitized_link_target = link_target;
					}
					printf(" -> %s", sanitized_link_target);

					/* Free if it was sanitized */
					if (!ls_opts->o_raw_print_non_printable) {
						free(sanitized_link_target);
					}
				}
			}

			printf("\n");

		} else {
			/* short format */
			printf("%s%s%s", inode_str, blocks_str, filename);
			if (ls_opts->o_type_indicate &&
			    (type_indicator = type_indicate_F(sb->st_mode)) != ' ') {
				printf("%c", type_indicator);
			}

			printf("\n");
		}

		/* Free the sanitized filename only if it was malloc'ed */
		if (!ls_opts->o_raw_print_non_printable) {
			free(filename);
		}
	}
}