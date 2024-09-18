#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>
#include <errno.h>
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
	printf("current_dir: %d\n", ls_opts->current_dir);
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
	struct stat sb;
	int i;
	int optind = parse_commandline_args(argc, argv, &ls_opts);

	if (argc <= 2) {
		ls_opts.single_dir = 1;
	}

	/* Files are accessible in argv from [optind, argc) */
	if (!ls_opts.current_dir) {
		for (i = optind; i < argc; i++) {
			if (stat(argv[i], &sb) < 0) {
				fprintf(stderr, "%s: %s: %s\n", getprogname(), argv[i],
				        strerror(errno));
				if (errno != ENOENT) {
					exit(EXIT_FAILURE);
				}
			}
			if (S_ISDIR(sb.st_mode)) {
				if (process_directory(argv[i], &ls_opts) < 0) {
					fprintf(stderr, "'%s: %s\n", getprogname(),
					        strerror(errno));
					exit(EXIT_FAILURE);
				}
			}
		}
	} else {
		if (process_directory(".", &ls_opts) < 0) {
			fprintf(stderr, "'%s: %s\n", getprogname(), strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	/*
	print_opts(&ls_opts);
	*/
	return EXIT_SUCCESS;
}