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
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <vis.h>

#include "flag_handlers.h"
#include "ls_options.h"
#include "sorting_functions.h"

void
print_entry(FTSENT *entry, char *filename, ls_options *ls_opts)
{
	char visbuf[PATH_MAX * 4 + 1];
	struct stat *sb = entry->fts_statp;

	strvis(visbuf, filename, VIS_GLOB);

	printf("%s", visbuf);
	/* TODO print more here maybe ... */
	(void)ls_opts;
	(void)sb;
	printf("\n");
}

void
print_entry_long_format(FTSENT *entry, char *filename, ls_options *ls_opts)
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
	       date_str, filename);

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
process_entry(FTSENT *entry, char *filename, ls_options *ls_opts)
{
	struct stat *sb = entry->fts_statp;

	(void)sb;
	if (ls_opts->o_long_format) {
		print_entry_long_format(entry, filename, ls_opts);
	} else {
		print_entry(entry, filename, ls_opts);
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
	int (*sort_fn)(const FTSENT **, const FTSENT **);

	sort_fn = &lexicographical_sort;

	/* Do not use fts for -d at all, just lstat */
	if (ls_opts->o_list_directories_as_files) {
		handle_dirs_as_files_d(paths, ls_opts, process_entry);
		return 0;
	}

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
			fprintf(stderr, "%s: %s\n", entry->fts_path,
			        strerror(entry->fts_errno));
			continue;
		}
		if (entry->fts_info == FTS_DC) {
			fprintf(stderr, "directory %s: causes a cycle\n", entry->fts_path);
			continue;
		}

		/* Prevent duplicate processing of root-level post-order dirs */
		if (entry->fts_info == FTS_DP && entry->fts_level == FTS_ROOTLEVEL) {
			continue;
		}

		/* Prevent recursive traversal of fts if !-R */
		/* Depth > root -> nested folder -> skip if not -R */
		if ((entry->fts_info == FTS_D || entry->fts_info == FTS_DP) &&
		    !ls_opts->o_recursive && entry->fts_level > FTS_ROOTLEVEL) {
			fts_set(ftsp, entry, FTS_SKIP);
			continue;
		}

		/* Exclude hidden files/skip hidden post-order dirs if triggered with
		 * -aA */
		if ((entry->fts_name[0] == '.') &&
		    (handle_hidden_files_a_A(entry->fts_name, ls_opts) == 0)) {
			if (entry->fts_info == FTS_D && entry->fts_level > FTS_ROOTLEVEL) {
				fts_set(ftsp, entry, FTS_SKIP);
			}
			continue;
		}

		if (entry->fts_info == FTS_D) {
			if (!(entry->fts_level == FTS_ROOTLEVEL && ls_opts->single_dir)) {
				if (!first_entry) {
					printf("\n");
				}
				printf("%s:\n", entry->fts_path);
				first_entry = 0;
			}
			continue;
		}

		process_entry(entry, entry->fts_name, ls_opts);
		first_entry = 0;
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
