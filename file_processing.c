#include "file_processing.h"

#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>

#include "flag_handlers.h"
#include "ls_options.h"

void
print_entry(FTSENT *entry, ls_options *ls_opts)
{
	struct stat *sb = entry->fts_statp;
	printf("%s: %d", entry->fts_name, entry->fts_level);
	if (S_ISDIR(sb->st_mode)) {
		printf(" dir ");
	}
	/* TODO print more here maybe ... */
	(void)ls_opts;
	printf("\n");
}

void
print_entry_long_format(FTSENT *entry, ls_options *ls_opts)
{
	(void)entry;
	(void)ls_opts;
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
	/* TODO: Declare fn ptr for compare param of fts_open */

	if (ls_opts->o_list_directories_as_files) {
		fts_opts = FTS_LOGICAL;
	} else {
		fts_opts = FTS_PHYSICAL;
	}
	if (ls_opts->o_include_dot_entries) {
		fts_opts |= FTS_SEEDOT;
	}

	/* fts_opts |= FTS_NOCHDIR; */

	if ((ftsp = fts_open(paths, fts_opts, NULL)) == NULL) {
		return -1;
	}

	while ((entry = fts_read(ftsp)) != NULL) {
		/* Show dir name for non-recursive when mulitple paths given */
		if (!ls_opts->o_recursive && entry->fts_level == FTS_ROOTLEVEL &&
		    (entry->fts_info == FTS_D || entry->fts_info == FTS_DP)) {

			/* Only show dir name for pre-order dirs */
			if (!ls_opts->single_dir && entry->fts_info == FTS_D) {
				printf("\n%s:\n", entry->fts_path);
			}
			continue;
		}

		/* Prevent recursive traversal of fts if -d or !-R */
		if (entry->fts_info == FTS_D) {
			if (ls_opts->o_list_directories_as_files) {
				fts_set(ftsp, entry, FTS_SKIP);
				/* Process once so dir is printed as regular file */
				process_entry(entry, ls_opts);
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
