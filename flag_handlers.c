#include "flag_handlers.h"

#include <string.h>

#include "ls_options.h"

int
handle_hidden_files_a_A(char *filename, ls_options *ls_opts)
{
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