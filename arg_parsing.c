#include "arg_parsing.h"

void
usage()
{
	fprintf(stderr, "Usage: %s [-AacdFfhiklnqRrSstuw] [file ...]\n",
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
			if (ls_opts->o_use_access_time) {
				ls_opts->o_use_access_time = 0;
			}
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
			if (ls_opts->o_report_kb) {
				ls_opts->o_report_kb = 0;
			}
			ls_opts->o_human_readable_size = 1;
			break;
		case 'i':
			ls_opts->o_print_inode = 1;
			break;
		case 'k':
			if (ls_opts->o_human_readable_size) {
				ls_opts->o_human_readable_size = 0;
			}
			ls_opts->o_report_kb = 1;
			break;
		case 'l':
			if (ls_opts->o_numeric_ids) {
				ls_opts->o_numeric_ids = 0;
			}
			ls_opts->o_long_format = 1;
			break;
		case 'n':
			if (ls_opts->o_long_format) {
				ls_opts->o_long_format = 0;
			}
			ls_opts->o_numeric_ids = 1;
			break;
		case 'q':
			if (ls_opts->o_raw_print_non_printable) {
				ls_opts->o_raw_print_non_printable = 0;
			}
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
			if (ls_opts->o_use_status_time) {
				ls_opts->o_use_status_time = 0;
			}
			ls_opts->o_use_access_time = 1;
			break;
		case 'w':
			if (ls_opts->o_f_non_printable) {
				ls_opts->o_f_non_printable = 0;
			}
			ls_opts->o_raw_print_non_printable = 1;
			break;
		case '?':
		default:
			usage();
		}
	}

	/* Enable current_dir if no file args are passed */
	if (optind >= argc) {
		ls_opts->current_dir = 1;
	}

	return optind;
}
