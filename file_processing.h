#ifndef FILE_PROCESSING_H
#define FILE_PROCESSING_H

/* FTS_ROOTLEVEL taken from gnulib fts_.h
 * https://github.com/coreutils/gnulib/blob/dd07b74893bc2fee44743f775a887930d04e9a85/lib/fts_.h#L222
 */
#define FTS_ROOTLEVEL 0

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