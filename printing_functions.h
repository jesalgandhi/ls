#ifndef PRINTING_FUNCTIONS_H
#define PRINTING_FUNCTIONS_H

/* Max numbers of digits in u64/64 bit ints */
#define MAX_UINT64_DIGITS 20
#define MAX_INT64_DIGITS 19

/* Size of symbolic string returned by strmode(3)
"This stored string is eleven characters in length
  plus a trailing nul byte.""
 */
#define SYMBOLIC_STRING_SIZE 12

/* Month is always abbreviated to 3 chars + \0 */
#define ABBREVIATED_MONTH_SIZE 4

/* Assuming date + time||year <= 32 bytes */
#define DATE_STRING_SIZE 32

/* Add 1 for \0 */
#define MAX_INODE_STR_SIZE (MAX_UINT64_DIGITS + 1)
#define MAX_BLOCKS_STR_SIZE (MAX_INT64_DIGITS + 1)


/* 5 bytes because we want shortest meaningful positive value
 * humanize_number(3):
 * "To generate the shortest meaningful value, a buffer length (len) that is
 * 6 greater the length of the suffix along with HN_AUTOSCALE will ensure
 * the highest meaningful scale is used.  Allow one extra byte for the sign
 * if the number is negative, and one less if the HN_NOSPACE flag is used."
 */
#define FILESIZE_STR_SIZE 5

#include <sys/stat.h>
#include <sys/types.h>

#include <fts.h>

#include "dir_info.h"
#include "ls_options.h"

void switch_k_to_K(char *file_size_str);

/* Populates output with a sanitized filename */
void sanitize_filename(const char *input, char *output, size_t size);

/* Allocates memory in output for a sanitized filename (for fts_pointer) */
void sanitize_filename_malloc(const char *input, char **output);

/* Prints all entries of children in non-standard (short/long) format, depending
 * on options */
void print_children(FTSENT *children, ls_options *ls_opts, dir_info *di);

#endif