#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>
#include <errno.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "arg_parsing.h"
#include "file_processing.h"
#include "ls_options.h"

/* for testing -- delete later */
void
print_opts(ls_options *ls_opts)
{
	printf("single_dir: %d\n", ls_opts->single_dir);
	printf("o_list_all_except_dot: %d\n", ls_opts->o_list_all_except_dot);
	printf("o_include_dot_entries: %d\n", ls_opts->o_include_dot_entries);
	printf("o_use_status_time: %d\n", ls_opts->o_use_status_time);
	printf("o_list_directories_as_files: %d\n",
	       ls_opts->o_list_directories_as_files);
	printf("o_type_indicate: %d\n", ls_opts->o_type_indicate);
	printf("o_no_sorting: %d\n", ls_opts->o_no_sorting);
	printf("o_human_readable_size: %d\n", ls_opts->o_human_readable_size);
	printf("o_print_inode: %d\n", ls_opts->o_print_inode);
	printf("o_report_kb: %d\n", ls_opts->o_report_kb);
	printf("o_long_format: %d\n", ls_opts->o_long_format);
	printf("o_numeric_ids: %d\n", ls_opts->o_numeric_ids);
	printf("o_f_non_printable: %d\n", ls_opts->o_f_non_printable);
	printf("o_recursive: %d\n", ls_opts->o_recursive);
	printf("o_reverse_sort: %d\n", ls_opts->o_reverse_sort);
	printf("o_sort_by_size: %d\n", ls_opts->o_sort_by_size);
	printf("o_display_block_usage: %d\n", ls_opts->o_display_block_usage);
	printf("o_sort_by_mod_time: %d\n", ls_opts->o_sort_by_mod_time);
	printf("o_use_access_time: %d\n", ls_opts->o_use_access_time);
	printf("o_raw_print_non_printable: %d\n",
	       ls_opts->o_raw_print_non_printable);
}

int
main(int argc, char **argv)
{
	ls_options ls_opts = {0};
	int i;
	int num_paths;
	char **paths;
	int optind = parse_commandline_args(argc, argv, &ls_opts);

	num_paths = argc - optind;
	if (num_paths <= 1) {
		ls_opts.single_dir = 1;
	}

	/* Files are accessible in argv from [optind, argc) */
	if (!ls_opts.current_dir) {
		num_paths = argc - optind;

		/* +1 because paths must be NULL-terminated for fts(3) */
		paths = malloc((num_paths + 1) * sizeof(char *));
		if (paths == NULL) {
			fprintf(stderr, "%s: %s", getprogname(), strerror(errno));
			exit(EXIT_FAILURE);
		}
		for (i = 0; i < num_paths; i++) {
			paths[i] = argv[optind + i];
		}
		paths[num_paths] = NULL;
	} else {
		paths = malloc(2 * sizeof(char *));
		if (paths == NULL) {
			fprintf(stderr, "%s: %s", getprogname(), strerror(errno));
			exit(EXIT_FAILURE);
		}
		paths[0] = ".";
		paths[1] = NULL;
	}

	if (process_paths(paths, &ls_opts) < 0) {
		fprintf(stderr, "%s: %s\n", getprogname(), strerror(errno));
		if (!ls_opts.current_dir) {
			free(paths);
		}
		exit(EXIT_FAILURE);
	}
	if (!ls_opts.current_dir) {
		free(paths);
	}

	/*
	print_opts(&ls_opts);
	*/
	return EXIT_SUCCESS;
}