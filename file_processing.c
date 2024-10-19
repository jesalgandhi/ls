#include "file_processing.h"

#include <sys/stat.h>
#include <sys/types.h>

#include <ctype.h>
#include <errno.h>
#include <fts.h>
#include <grp.h>
#include <limits.h>
#include <math.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "dir_info.h"
#include "flag_handlers.h"
#include "ls_options.h"
#include "printing_functions.h"
#include "sorting_functions.h"

int
process_entries(FTSENT *children, ls_options *ls_opts)
{

	FTSENT *child;
	struct stat *sb;
	struct passwd *pw;
	struct group *gr;
	char *sanitized_name;

	time_t t = time(NULL);
	struct tm *tm_ptr;
	struct tm curr_tm_info;
	struct tm file_tm_info;
	int curr_yr;
	int file_yr;

	/* Total block size always init to 0; we initialize the others to -1 to
	 * check whether they were populated in print to avoid unnecessary
	 * processing */
	dir_info di = {0, -1, -1, -1, -1, -1, -1, -1, -1};

	int size_len;
	int links_len;
	int owner_len;
	int group_len;
	int day_len;
	int date_time_len;
	int inode_len;
	int block_size_len;

	tm_ptr = localtime(&t);
	curr_tm_info = *tm_ptr;
	curr_yr = curr_tm_info.tm_year + 1900;

	/* Find widths of props of children files */
	for (child = children; child != NULL; child = child->fts_link) {
		if ((child->fts_name[0] == '.') &&
		    (handle_hidden_files_a_A(child->fts_name, ls_opts) == 0)) {
			continue;
		}

		/* Only free in print IF !o_raw_print_non_printable */
		if (!ls_opts->o_raw_print_non_printable) {
			sanitize_filename_malloc(child->fts_name, &sanitized_name);
			child->fts_pointer = sanitized_name;
		} else {
			child->fts_pointer = child->fts_name;
		}

		sb = child->fts_statp;

		/* Below two options apply to either short/long format */
		if (ls_opts->o_display_block_usage) {
			/* Check len of str representation of type as long (repeated below)
			 */
			block_size_len = snprintf(NULL, 0, "%ld", (long)sb->st_blocks);
			if (block_size_len > di.max_block_size_width) {
				di.max_block_size_width = block_size_len;
			}
		}
		if (ls_opts->o_print_inode) {
			inode_len = snprintf(NULL, 0, "%ld", (long)sb->st_ino);
			if (inode_len > di.max_inode_width) {
				di.max_inode_width = inode_len;
			}
		}

		/* Below options always apply to long format */
		if (ls_opts->o_long_format) {
			di.total_blocks += sb->st_blocks;

			links_len = snprintf(NULL, 0, "%ld", (long)sb->st_nlink);
			if (links_len > di.max_links_width) {
				di.max_links_width = links_len;
			}

			pw = getpwuid(sb->st_uid);
			owner_len = pw ? strlen(pw->pw_name) : 0;
			if (owner_len > di.max_owner_width) {
				di.max_owner_width = owner_len;
			}

			gr = getgrgid(sb->st_gid);
			group_len = gr ? strlen(gr->gr_name) : 0;
			if (group_len > di.max_group_width) {
				di.max_group_width = group_len;
			}

			size_len = snprintf(NULL, 0, "%ld", (long)sb->st_size);
			if (size_len > di.max_size_width) {
				di.max_size_width = size_len;
			}

			tm_ptr = localtime(&sb->st_mtime);
			file_tm_info = *tm_ptr;
			file_yr = file_tm_info.tm_year + 1900;

			day_len = snprintf(NULL, 0, "%d", file_tm_info.tm_mday);
			if (day_len > di.max_day_width) {
				di.max_day_width = day_len;
			}

			if (curr_yr != file_yr) {
				day_len = snprintf(NULL, 0, "%d", file_yr);
			} else {
				/* Format is guaranteed to be in HH:MM, len = 6 - \0 = 5 */
				date_time_len = 5;
			}
			di.max_date_time_width = date_time_len;
		}
	}

	print_children(children, ls_opts, &di);
	return 0;
}

int
process_paths(char **paths, ls_options *ls_opts, int is_directory)
{
	FTS *ftsp;
	FTSENT *entry, *children, *child;
	int fts_opts;
	int i;
	int first_entry = 1;
	int (*sort_fn)(const FTSENT **, const FTSENT **);

	/* For files, create a LL to pass into process_entries */
	FTSENT *file_entries = NULL;
	FTSENT **lastptr = &file_entries;


	sort_fn = &lexicographical_sort;

	fts_opts = FTS_PHYSICAL | FTS_NOCHDIR;
	if (ls_opts->o_include_dot_entries) {
		fts_opts |= FTS_SEEDOT;
	}

	if ((ftsp = fts_open(paths, fts_opts, sort_fn)) == NULL) {
		return -1;
	}


	while ((entry = fts_read(ftsp)) != NULL) {
		/* Ensure entry is not erroneous */
		if (entry->fts_info == FTS_ERR || entry->fts_info == FTS_NS ||
		    entry->fts_info == FTS_DNR) {
			fprintf(stderr, "%s: %s: %s\n", getprogname(), entry->fts_name,
			        strerror(entry->fts_errno));
			continue;
		}
		if (entry->fts_info == FTS_DC) {
			fprintf(stderr, "%s: %s: causes a cycle\n", getprogname(),
			        entry->fts_name);
			continue;
		}

		/* Prevent duplicate processing of root-level post-order dirs */
		if (entry->fts_info == FTS_DP) {
			continue;
		}

		/* Prevent recursive traversal of fts if !-R */
		/* Depth > root -> nested folder -> skip if not -R */
		if ((entry->fts_info == FTS_D) && !ls_opts->o_recursive &&
		    entry->fts_level > FTS_ROOTLEVEL) {
			fts_set(ftsp, entry, FTS_SKIP);
			continue;
		}

		/* Exclude hidden files/skip hidden post-order dirs if triggered with
		 * -aA */
		if ((entry->fts_name[0] == '.') &&
		    (handle_hidden_files_a_A(entry->fts_name, ls_opts) == 0)) {
			if (entry->fts_info == FTS_D && entry->fts_level > FTS_ROOTLEVEL) {
				fts_set(ftsp, entry, FTS_SKIP);
				continue;
			}
		}

		/* Add non-dir entry to LL and process after fts_read */
		if (!is_directory) {
			*lastptr = entry;
			lastptr = &entry->fts_link;
			entry->fts_link = NULL;
			continue;
		}

		if (entry->fts_info == FTS_D) {
			/* Preface dir with dir name */
			if (!(entry->fts_level == FTS_ROOTLEVEL && ls_opts->single_dir)) {
				if (!first_entry) {
					printf("\n");
				}
				printf("%s:\n", entry->fts_path);
				first_entry = 0;
			}

			children = fts_children(ftsp, 0);

			/* Long format/ requires processing all entries then printing */
			if (ls_opts->o_long_format || ls_opts->o_print_inode ||
			    ls_opts->o_display_block_usage) {
				process_entries(children, ls_opts);
				first_entry = 0;
				continue;
			}

			/* For short format, we can just print directly */
			for (child = children; child != NULL; child = child->fts_link) {
				if ((child->fts_name[0] == '.') &&
				    (handle_hidden_files_a_A(child->fts_name, ls_opts) == 0)) {
					continue;
				}
				print_entry_short(child, child->fts_name, ls_opts);
			}
		}
	}

	if (errno != 0) {
		fts_close(ftsp);
		return -1;
	}
	if (fts_close(ftsp) < 0) {
		return -1;
	}

	if (!is_directory && file_entries != NULL) {
		if (!(ls_opts->o_long_format || ls_opts->o_print_inode ||
		      ls_opts->o_display_block_usage)) {
			i = 0;
			while (file_entries != NULL) {
				print_entry_short(&file_entries[i], file_entries[i].fts_path,
				                  ls_opts);
				i += 1;
				file_entries = file_entries->fts_link;
			}
		} else {
			process_entries(file_entries, ls_opts);
		}
	}

	return 0;
}
