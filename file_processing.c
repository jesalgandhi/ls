#include "file_processing.h"

#include <sys/stat.h>
#include <sys/types.h>

#include <ctype.h>
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

#include "flag_handlers.h"
#include "ls_options.h"
#include "printing_functions.h"
#include "sorting_functions.h"

int
process_entries(FTSENT *children, ls_options *ls_opts)
{

	FTSENT *child = children;
	char sanitized_name[PATH_MAX];
	sanitize_filename(child->fts_name, sanitized_name, sizeof(sanitized_name));

	/* First find widths of props of all files */
	for (child = children; child != NULL; child = child->fts_link) {
		if ((child->fts_name[0] == '.') &&
		    (handle_hidden_files_a_A(child->fts_name, ls_opts) == 0)) {
			continue;
		}
	}

	/* Then print each file using the found widths */

	(void)ls_opts;
	(void)children;

	return 0;
}

int
process_entry(FTSENT *entry, char *filename, ls_options *ls_opts)
{

	char sanitized_name[PATH_MAX];
	sanitize_filename(filename, sanitized_name, sizeof(sanitized_name));

	if (ls_opts->o_long_format) {
		print_entry_long_format(entry, sanitized_name, ls_opts);
	} else {
		print_entry(entry, sanitized_name, ls_opts);
	}
	return 0;
}

int
process_paths(char **paths, ls_options *ls_opts)
{
	FTS *ftsp;
	FTSENT *entry, *children, *child;
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

			/* Long format requires processing all entries then printing */
			if (ls_opts->o_long_format) {
				process_entries(children, ls_opts);
				continue;
			}

			/* For short format, we can just print directly */
			for (child = children; child != NULL; child = child->fts_link) {
				if ((child->fts_name[0] == '.') &&
				    (handle_hidden_files_a_A(child->fts_name, ls_opts) == 0)) {
					continue;
				}
				process_entry(child, child->fts_name, ls_opts);
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
	return 0;
}
