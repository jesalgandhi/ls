#include "file_processing.h"

int
process_directory(const char *path, struct stat *sb, ls_options *ls_opts)
{
	DIR *dp;
	struct dirent *dirp;
	if ((dp = opendir(path)) == NULL) {
		return -1;
	}

	while ((dirp = readdir(dp)) != NULL) {
		if ((dirp->d_name[0] == '.') &&
		    (handle_hidden_files_a_A(dirp->d_name, ls_opts) == 0)) {
			continue;
		}

		printf("%s\n", dirp->d_name);
	}
	closedir(dp);

	(void)sb;

	return 0;
}

void
print_long_format(const char *file, const struct stat *sb, ls_options *ls_opts)
{
	(void)file;
	(void)sb;
	(void)ls_opts;
}

void
display_file_info(const char *file, const struct stat *sb, ls_options *ls_opts)
{
	/*
	if (ls_opts->o_long_format) {
	    print_long_format(file, sb, ls_opts);
	} else {
	    printf("%s\n", file);
	}
	*/

	(void)file;
	(void)sb;
	(void)ls_opts;
	printf("%s\n", file);
}

int
process_non_dir_file(const char *file, struct stat *sb, ls_options *ls_opts)
{
	mode_t mode = sb->st_mode;
	if (S_ISREG(mode)) {
		process_regular_file(file, sb, ls_opts);
	}

	return 0;
}

int
process_regular_file(const char *file, struct stat *sb, ls_options *ls_opts)
{
	/* Regular file specific flags here TODO */

	display_file_info(file, sb, ls_opts);

	(void)file;
	(void)sb;
	(void)ls_opts;
	return 0;
}
