#include "file_processing.h"

int
process_directory(const char *path, ls_options *ls_opts)
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
	return 0;
}