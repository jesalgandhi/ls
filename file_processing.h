#ifndef FILE_PROCESSING_H
#define FILE_PROCESSING_H

#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>

#include "flag_handlers.h"
#include "ls_options.h"

int process_paths(const char **paths, ls_options *ls_opts);

int process_entry(FTSENT *entry, ls_options *ls_opts);

int print_entry(FTSENT *entry, ls_options *ls_opts);

void print_long_format(const char *file, const struct stat *sb,
                       ls_options *ls_opts);

#endif