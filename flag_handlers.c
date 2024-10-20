#include "flag_handlers.h"

#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ls_options.h"

int
handle_hidden_files_a_A(char *filename, ls_options *ls_opts)
{
	if (strcmp(filename, ".") == 0 && ls_opts->o_list_directories_as_files) {
		return -1;
	}
	if (ls_opts->o_include_dot_entries) {
		return -1;
	}
	if (ls_opts->o_list_all_except_dot) {
		if ((strcmp(filename, ".") == 0) || (strcmp(filename, "..") == 0)) {
			return 0;
		}
		return -1;
	}

	return 0;
}

void
handle_dirs_as_files_d(char **paths, ls_options *ls_opts,
                       int (*process_entry)(FTSENT *entry, char *filename,
                                            ls_options *ls_opts))
{
	int i;
	struct stat sb;
	FTSENT entry_struct;

	for (i = 0; paths[i] != NULL; i++) {
		if (lstat(paths[i], &sb) == -1) {
			fprintf(stderr, "%s: cannot access '%s': %s\n", getprogname(),
			        paths[i], strerror(errno));
			continue;
		}

		/* Construct custom entry to provide process_entry */
		memset(&entry_struct, 0, sizeof(FTSENT));
		entry_struct.fts_path = paths[i];
		entry_struct.fts_accpath = paths[i];
		entry_struct.fts_statp = &sb;
		process_entry(&entry_struct, paths[i], ls_opts);
	}
}

char
type_indicate_F(mode_t st_mode)
{
	if (S_ISDIR(st_mode)) {
		return '/';
	}
	if (S_ISLNK(st_mode)) {
		return '@';
	}
	if (S_ISWHT(st_mode)) {
		return '%';
	}
	if (S_ISSOCK(st_mode)) {
		return '=';
	}
	if (S_ISFIFO(st_mode)) {
		return '|';
	}
	if ((st_mode > 0) && (S_IEXEC & st_mode)) {
		return '*';
	}
	return ' ';
}