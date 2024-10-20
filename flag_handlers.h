#ifndef FLAG_HANDLERS_H
#define FLAG_HANDLERS_H

#include <sys/stat.h>
#include <sys/types.h>

#include <fts.h>

#include "ls_options.h"


/* Returns 0 if file should be excluded; -1 otherwise */
int handle_hidden_files_a_A(char *filename, ls_options *ls_opts);

void handle_dirs_as_files_d(char **paths, ls_options *ls_opts,
                            int (*process_entry)(FTSENT *entry, char *filename,
                                                 ls_options *ls_opts));

/* Returns type indicator to append to filename, based on info */
char type_indicate_F(mode_t st_mode);

#endif
