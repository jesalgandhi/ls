#include "file_processing.h"

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

		/* Skip entry if recursive flag not set */
		if (!ls_opts->o_recursive && entry->fts_level > 0) {
			fts_set(ftsp, entry, FTS_SKIP);
			continue;
		}

		/* Handle hidden files/flags accordingly */
		if ((entry->fts_name[0] == '.') &&
		    (handle_hidden_files_a_A(entry->fts_name, ls_opts) == 0)) {
			continue;
		}

		process_entry(entry, ls_opts);
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
print_long_format(const char *file, const struct stat *sb, ls_options *ls_opts)
{
	(void)file;
	(void)sb;
	(void)ls_opts;
}