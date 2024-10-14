#ifndef FILE_PROCESSING_H
#define FILE_PROCESSING_H

/* FTS_ROOTLEVEL taken from gnulib fts_.h
 * https://github.com/coreutils/gnulib/blob/dd07b74893bc2fee44743f775a887930d04e9a85/lib/fts_.h#L222
 */
#define FTS_ROOTLEVEL 0

/* Size of symbolic string returned by strmode(3)
"This stored string is eleven characters in length
  plus a trailing nul byte.""
 */
#define SYMBOLIC_STRING_SIZE 12

#define ABBREVIATED_MONTH_SIZE 4

#define DATE_STRING_SIZE 32

#include <sys/types.h>

#include <fts.h>

#include "ls_options.h"

int process_paths(char **paths, ls_options *ls_opts);

int process_entry(FTSENT *entry, ls_options *ls_opts);

/* Prints file in short format */
void print_entry(FTSENT *entry, ls_options *ls_opts);

/* Prints file in long format */
void print_entry_long_format(FTSENT *entry, ls_options *ls_opts);

#endif