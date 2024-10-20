#include "arg_parsing.h"

void
usage()
{
	fprintf(stderr, "usage: %s [-AacdFfhiklnqRrSstuw] [file ...]\n",
	        getprogname());
	exit(EXIT_FAILURE);
}

int
parse_commandline_args(int argc, char **argv, ls_options *ls_opts)

{
	int option;
	extern int optind;

	while ((option = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1) {
		switch (option) {
		case 'A':
			ls_opts->o_list_all_except_dot = 1;
			break;
		case 'a':
			ls_opts->o_include_dot_entries = 1;
			break;
		case 'c':
			ls_opts->o_use_access_time = 0;
			ls_opts->o_use_status_time = 1;
			break;
		case 'd':
			ls_opts->o_list_directories_as_files = 1;
			break;
		case 'F':
			ls_opts->o_type_indicate = 1;
			break;
		case 'f':
			ls_opts->o_no_sorting = 1;
			break;
		case 'h':
			ls_opts->o_report_kb = 0;
			ls_opts->o_human_readable_size = 1;
			break;
		case 'i':
			ls_opts->o_print_inode = 1;
			break;
		case 'k':
			ls_opts->o_human_readable_size = 0;
			ls_opts->o_report_kb = 1;
			break;
		case 'l':
			ls_opts->o_numeric_ids = 0;
			ls_opts->o_long_format = 1;
			break;
		case 'n':
			ls_opts->o_long_format = 0;
			ls_opts->o_numeric_ids = 1;
			break;
		case 'q':
			ls_opts->o_raw_print_non_printable = 0;
			ls_opts->o_f_non_printable = 1;
			break;
		case 'R':
			ls_opts->o_recursive = 1;
			break;
		case 'r':
			ls_opts->o_reverse_sort = 1;
			break;
		case 'S':
			ls_opts->o_sort_by_size = 1;
			break;
		case 's':
			ls_opts->o_display_block_usage = 1;
			break;
		case 't':
			ls_opts->o_sort_by_mod_time = 1;
			break;
		case 'u':
			ls_opts->o_use_status_time = 0;
			ls_opts->o_use_access_time = 1;
			break;
		case 'w':
			ls_opts->o_f_non_printable = 0;
			ls_opts->o_raw_print_non_printable = 1;
			break;
		case '?':
		default:
			usage();
		}
	}

	return optind;
}
