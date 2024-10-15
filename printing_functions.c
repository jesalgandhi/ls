#include "printing_functions.h"

#include <ctype.h>
#include <errno.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void
sanitize_filename(const char *input, char *output, size_t size)
{
	size_t i = 0, j = 0;
	while (i < size - 1 && input[i] != '\0') {
		if (isprint((unsigned char)input[i])) {
			output[j++] = input[i];
		} else {
			output[j++] = '?';
		}
		i++;
	}
	output[j] = '\0';
}

void
sanitize_filename_malloc(const char *input, char **output)
{
	size_t i;
	size_t len = strlen(input);
	char *sanitized = (char *)malloc(len + 1);

	if (sanitized == NULL) {
		fprintf(stderr, "%s: %s\n", getprogname(), strerror(errno));
		exit(EXIT_FAILURE);
	}

	for (i = 0; i <= len; i++) {
		if (isprint((unsigned char)input[i]) || input[i] == '\0') {
			sanitized[i] = input[i];
		} else {
			sanitized[i] = '?';
		}
	}

	*output = sanitized;
}

void
print_entry_short(FTSENT *entry, char *filename, ls_options *ls_opts)
{
	char sanitized_name[PATH_MAX];
	if (!ls_opts->o_raw_print_non_printable) {
		sanitize_filename(filename, sanitized_name, sizeof(sanitized_name));
		printf("%s", sanitized_name);
	} else {
		printf("%s", filename);
	}

	/* TODO print more here maybe ... */
	(void)entry;
	printf("\n");
}

void
print_entry_long(FTSENT *entry, char *filename, ls_options *ls_opts)
{
	struct stat *sb = entry->fts_statp;
	char sym_str[SYMBOLIC_STRING_SIZE];
	char date_str[DATE_STRING_SIZE];
	char month_str[ABBREVIATED_MONTH_SIZE];
	char link_target[PATH_MAX + 1];
	int curr_yr;
	int file_yr;
	struct passwd *pw;
	struct group *gr;

	/* Get current and file time info */
	time_t t = time(NULL);
	struct tm *tm_ptr;
	struct tm curr_tm_info;
	struct tm file_tm_info;
	tm_ptr = localtime(&t);
	curr_tm_info = *tm_ptr;
	tm_ptr = localtime(&sb->st_mtime);
	file_tm_info = *tm_ptr;
	curr_yr = curr_tm_info.tm_year + 1900;
	file_yr = file_tm_info.tm_year + 1900;

	pw = getpwuid(sb->st_uid);
	gr = getgrgid(sb->st_gid);

	(void)ls_opts;
	strmode(sb->st_mode, sym_str);
	strftime(month_str, sizeof(month_str), "%b", &file_tm_info);

	/* Provide year if file year differs from current year */
	if (curr_yr != file_yr) {
		snprintf(date_str, sizeof(date_str), "%s %2d %d", month_str,
		         file_tm_info.tm_mday, file_yr);
	} else {
		snprintf(date_str, sizeof(date_str), "%s %2d %02d:%02d", month_str,
		         file_tm_info.tm_mday, file_tm_info.tm_hour,
		         file_tm_info.tm_min);
	}


	printf("%-11s %-1ld %-7s %-8s %4ld %s %s", sym_str, (long)sb->st_nlink,
	       pw ? pw->pw_name : "", gr ? gr->gr_name : "", (long)sb->st_size,
	       date_str, filename);

	/* Follow through symbolic links */
	if (S_ISLNK(sb->st_mode)) {
		ssize_t len =
			readlink(entry->fts_accpath, link_target, sizeof(link_target) - 1);
		if (len != -1) {
			link_target[len] = '\0';
			printf(" -> %s", link_target);
		}
	}

	printf("\n");
}