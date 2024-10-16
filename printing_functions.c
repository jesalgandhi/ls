#include "printing_functions.h"

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
print_entry_short(FTSENT *entry, char *filename, ls_options *ls_opts)
{
	char sanitized_name[PATH_MAX];
	if (!ls_opts->o_raw_print_non_printable) {
		sanitize_filename(filename, sanitized_name, sizeof(sanitized_name));
		printf("%s", sanitized_name);
	} else {
		printf("%s", filename);
	}

	/* TODO print more here maybe ... */
	(void)entry;
	printf("\n");
}

void
print_children(FTSENT *children, ls_options *ls_opts, dir_info *di)
{
	FTSENT *child;
	char *filename;
	struct stat *sb;

	char sym_str[SYMBOLIC_STRING_SIZE];
	char date_str[DATE_STRING_SIZE];
	char month_str[ABBREVIATED_MONTH_SIZE];
	char fullpath[PATH_MAX + 1];
	char link_target[PATH_MAX + 1];
	char inode_str[MAX_INODE_STR_SIZE];
	char blocks_str[MAX_BLOCKS_STR_SIZE];

	time_t t;
	struct tm curr_tm_info;
	struct tm file_tm_info;
	int curr_yr;
	int file_yr;

	struct passwd *pw;
	struct group *gr;
	ssize_t len;
	char *sanitized_link_target;

	t = time(NULL);
	localtime_r(&t, &curr_tm_info);
	curr_yr = curr_tm_info.tm_year + 1900;

	if (ls_opts->o_long_format && di->total_blocks >= 0) {
		printf("total %ld\n", di->total_blocks);
	}

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
			         di->max_block_size_width, (long)sb->st_blocks);
		}

		if (ls_opts->o_long_format) {
			pw = getpwuid(sb->st_uid);
			gr = getgrgid(sb->st_gid);

			printf("%s%s%s %*ld %-*s %-*s %*ld %s %s", inode_str, blocks_str,
			       sym_str, di->max_links_width, (long)sb->st_nlink,
			       di->max_owner_width, pw ? pw->pw_name : "",
			       di->max_group_width, gr ? gr->gr_name : "",
			       di->max_size_width, (long)sb->st_size, date_str, filename);

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
			printf("%s%s%s\n", inode_str, blocks_str, filename);
		}

		/* Free the sanitized filename only if it was malloc'ed */
		if (!ls_opts->o_raw_print_non_printable) {
			free(filename);
		}
	}
}