#include "file_processing.h"

#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <fts.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "flag_handlers.h"
#include "ls_options.h"

void
print_entry(FTSENT *entry, ls_options *ls_opts)
{
	struct stat *sb = entry->fts_statp;
	printf("%s", entry->fts_name);
	/* TODO print more here maybe ... */
	(void)ls_opts;
	(void)sb;
	printf("\n");
}

/*
void
print_entry_long_format(FTSENT *entry, ls_options *ls_opts)
{
    char sym_str[SYMBOLIC_STRING_SIZE];
    struct stat *sb = entry->fts_statp;

    (void)ls_opts;
    strmode(sb->st_mode, sym_str);
    printf("%s  ", sym_str);
    printf("%d  ", sb->st_nlink);
    printf("%s", entry->fts_name);
    printf("\n");
}
*/

void
print_entry_long_format(FTSENT *entry, ls_options *ls_opts)
{
	struct stat *sb = entry->fts_statp;
	char sym_str[SYMBOLIC_STRING_SIZE];
	char date_str[DATE_STRING_SIZE];
	char month_str[ABBREVIATED_MONTH_SIZE];
	char link_target[PATH_MAX + 1];
	int curr_yr;
	int file_yr;
	struct passwd *pw;
	struct group *gr;

	/* Get current and file time info */
	time_t t = time(NULL);
	struct tm *tm_ptr;
	struct tm curr_tm_info;
	struct tm file_tm_info;
	tm_ptr = localtime(&t);
	curr_tm_info = *tm_ptr;
	tm_ptr = localtime(&sb->st_mtime);
	file_tm_info = *tm_ptr;
	curr_yr = curr_tm_info.tm_year + 1900;
	file_yr = file_tm_info.tm_year + 1900;

	pw = getpwuid(sb->st_uid);
	gr = getgrgid(sb->st_gid);

	(void)ls_opts;
	strmode(sb->st_mode, sym_str);
	strftime(month_str, sizeof(month_str), "%b", &file_tm_info);

	/* Provide year if file year differs from current year */
	if (curr_yr != file_yr) {
		snprintf(date_str, sizeof(date_str), "%s %2d  %d", month_str,
		         file_tm_info.tm_mday, file_yr);
	} else {
		snprintf(date_str, sizeof(date_str), "%s %2d %02d:%02d", month_str,
		         file_tm_info.tm_mday, file_tm_info.tm_hour,
		         file_tm_info.tm_min);
	}


	printf("%-11s %-1ld %-7s %-8s %4ld %s %s", sym_str, (long)sb->st_nlink,
	       pw ? pw->pw_name : "", gr ? gr->gr_name : "", (long)sb->st_size,
	       date_str, entry->fts_name);

	/* Follow through symbolic links */
	if (S_ISLNK(sb->st_mode)) {
		ssize_t len =
			readlink(entry->fts_accpath, link_target, sizeof(link_target) - 1);
		if (len != -1) {
			link_target[len] = '\0';
			printf(" -> %s", link_target);
		}
	}

	printf("\n");
}

int
process_entry(FTSENT *entry, ls_options *ls_opts)
{
	struct stat *sb = entry->fts_statp;

	(void)sb;
	if (ls_opts->o_long_format) {
		print_entry_long_format(entry, ls_opts);
	} else {
		print_entry(entry, ls_opts);
	}
	return 0;
}

int
process_paths(char **paths, ls_options *ls_opts)
{
	FTS *ftsp;
	FTSENT *entry;
	int fts_opts;
	int first_entry = 1;
	/* TODO: Declare fn ptr for compare param of fts_open */

	if (ls_opts->o_list_directories_as_files) {
		fts_opts = FTS_LOGICAL;
	} else {
		fts_opts = FTS_PHYSICAL;
	}
	if (ls_opts->o_include_dot_entries) {
		fts_opts |= FTS_SEEDOT;
	}

	fts_opts |= FTS_NOCHDIR;

	if ((ftsp = fts_open(paths, fts_opts, NULL)) == NULL) {
		return -1;
	}

	while ((entry = fts_read(ftsp)) != NULL) {
		/* Show dir name for non-recursive when mulitple paths given */
		if (!ls_opts->o_recursive && entry->fts_level == FTS_ROOTLEVEL &&
		    (entry->fts_info == FTS_D || entry->fts_info == FTS_DP)) {

			/* Only show dir name for pre-order dirs */
			if (!ls_opts->single_dir && entry->fts_info == FTS_D) {
				if (!first_entry) {
					printf("\n");
				}
				printf("%s:\n", entry->fts_path);
				first_entry = 0;
			}
			continue;
		}

		/* Prevent recursive traversal of fts if -d or !-R */
		if (entry->fts_info == FTS_D) {
			if (ls_opts->o_list_directories_as_files) {
				fts_set(ftsp, entry, FTS_SKIP);
				/* Process once so dir is printed as regular file */
				process_entry(entry, ls_opts);
				first_entry = 0;
				continue;
				/* Depth > root -> nested folder -> skip if not -R */
			} else if (!ls_opts->o_recursive) {
				if (entry->fts_level > FTS_ROOTLEVEL) {
					fts_set(ftsp, entry, FTS_SKIP);
					continue;
				}
			}
		}

		/* Process non-hidden files, unless flagged otherwise */
		if ((entry->fts_name[0] == '.') &&
		    (handle_hidden_files_a_A(entry->fts_name, ls_opts) == 0)) {
			continue;
		} else {
			process_entry(entry, ls_opts);
			first_entry = 0;
		}
	}

	if (errno != 0) {
		fts_close(ftsp);
		return -1;
	}
	if (fts_close(ftsp) < 0) {
		return -1;
	}
	return 0;
}
