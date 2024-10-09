#ifndef FILE_PROCESSING_H
#define FILE_PROCESSING_H

#include <sys/types.h>

#include <fts.h>

#include "ls_options.h"

int process_paths(char **paths, ls_options *ls_opts);

int process_entry(FTSENT *entry, ls_options *ls_opts);

/* Prints non-dir file in short format */
void print_entry(FTSENT *entry, ls_options *ls_opts);

/* Prints non-dir file in long format */
void print_entry_long_format(FTSENT *entry, ls_options *ls_opts);

#endif