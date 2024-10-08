#include "file_processing.h"

#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>

#include "flag_handlers.h"
#include "ls_options.h"

int
process_paths(const char **paths, ls_options *ls_opts)
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

	if (ftsp = fts_open(paths, fts_opts, NULL) < 0) {
		fprintf(stderr, "%s: %s\n", getprogname(), strerror(errno));
		exit(EXIT_FAILURE);
	}

	while ((entry = fts_read(ftsp)) != NULL) {
		/* Handle hidden files/flags accordingly */
		if ((entry->fts_name[0] == '.') &&
		    (handle_hidden_files_a_A(entry->fts_name, ls_opts) == 0)) {
			continue;
		}

		/* Prevent recursive traversal of fts if -d or !-R */
		if (entry->fts_info == FTS_D) {
			if (ls_opts->o_list_directories_as_files) {
				fts_set(ftsp, entry, FTS_SKIP);
				/* Process once so dir is printed */
				process_entry(entry, ls_opts);
				continue;
			} else if (!ls_opts->o_recursive && entry->fts_level > 0) {
				fts_set(ftsp, entry, FTS_SKIP);
				continue;
			}
		}

		if (entry->fts_info != FTS_D && entry->fts_info != FTS_DP) {
			process_entry(entry, ls_opts);
		} else if (!ls_opts->single_dir) {
			printf("%s:\n", entry->fts_name);
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

int
process_entry(FTSENT *entry, ls_options *ls_opts)
{
}

int
print_entry(FTSENT *entry, ls_options *ls_opts)
{

	/* Print if not dir or if dir and -d flag */
	if (entry->fts_info != FTS_D ||
	    (entry->fts_info == FTS_D && ls_opts->o_list_directories_as_files)) {
	}
}

void
print_long_format(FTSENT *entry, ls_options *ls_opts)
{
	(void)entry;
	(void)ls_opts;
}