#ifndef PRINTING_FUNCTIONS_H
#define PRINTING_FUNCTIONS_H

/* Size of symbolic string returned by strmode(3)
"This stored string is eleven characters in length
  plus a trailing nul byte.""
 */
#define SYMBOLIC_STRING_SIZE 12

#define ABBREVIATED_MONTH_SIZE 4

#define DATE_STRING_SIZE 32

#include <sys/stat.h>
#include <sys/types.h>

#include <fts.h>

#include "ls_options.h"

/* Populates output with a sanitized filename */
void sanitize_filename(const char *input, char *output, size_t size);

/* Allocates memory in output for a sanitized filename (for fts_pointer) */
void sanitize_filename_malloc(const char *input, char **output);

/* Prints an entry in short format */
void print_entry_short(FTSENT *entry, char *filename, ls_options *ls_opts);

/* Prints an entry in long format */
void print_entry_long(FTSENT *entry, char *filename, ls_options *ls_opts);

#endif