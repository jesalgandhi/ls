#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>
#include <errno.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "arg_parsing.h"
#include "file_processing.h"
#include "ls_options.h"

int
main(int argc, char **argv)
{
	struct stat sb;
	ls_options ls_opts = {0};
	int i;
	char *path;
	int num_paths;
	char **file_paths;
	char **dir_paths;
	int file_count;
	int dir_count;
	int optind = parse_commandline_args(argc, argv, &ls_opts);

	num_paths = argc - optind;

	if (num_paths == 0) {
		num_paths = 1;
		ls_opts.single_dir = 1;
		dir_paths = malloc(2 * sizeof(char *));
		if (dir_paths == NULL) {
			fprintf(stderr, "%s: %s\n", getprogname(), strerror(errno));
			exit(EXIT_FAILURE);
		}
		dir_paths[0] = ".";
		dir_paths[1] = NULL;
		process_paths(dir_paths, &ls_opts, 1);
		free(dir_paths);
		return EXIT_SUCCESS;
	}
	if (num_paths <= 1) {
		ls_opts.single_dir = 1;
	}

	/* Allocate space for files/dirs separately */
	file_paths = malloc((num_paths + 1) * sizeof(char *));
	dir_paths = malloc((num_paths + 1) * sizeof(char *));
	if (file_paths == NULL || dir_paths == NULL) {
		fprintf(stderr, "%s: %s\n", getprogname(), strerror(errno));
		exit(EXIT_FAILURE);
	}

	file_count = 0;
	dir_count = 0;

	/* Files are accessible from [optind, argc) */
	for (i = optind; i < argc; i++) {
		path = argv[i];
		if (stat(path, &sb) == -1) {
			fprintf(stderr, "%s: %s: %s\n", getprogname(), path,
			        strerror(errno));
			continue;
		}
		if (S_ISDIR(sb.st_mode)) {
			dir_paths[dir_count++] = path;
		} else {
			file_paths[file_count++] = path;
		}
	}

	file_paths[file_count] = NULL;
	dir_paths[dir_count] = NULL;

	if (file_count > 0) {
		process_paths(file_paths, &ls_opts, 0);
	}

	if (dir_count > 0) {
		if (file_count > 0) {
			printf("\n");
		}
		process_paths(dir_paths, &ls_opts, 1);
	}

	free(file_paths);
	free(dir_paths);

	return EXIT_SUCCESS;
}