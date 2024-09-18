#ifndef FLAG_HANDLERS_H
#define FLAG_HANDLERS_H

#include <string.h>

#include "ls_options.h"

/* Returns 0 if file should be excluded; -1 otherwise */
int handle_hidden_files_a_A(char *filename, ls_options *ls_opts);

#endif
