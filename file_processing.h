#ifndef FILE_PROCESSING_H
#define FILE_PROCESSING_H

#include <sys/stat.h>

#include <dirent.h>
#include <stdio.h>

#include "flag_handlers.h"
#include "ls_options.h"

int process_directory(const char *path, struct stat *sb, ls_options *ls_opts);

int process_non_dir_file(const char *file, struct stat *sb,
                         ls_options *ls_opts);

int process_regular_file(const char *file, struct stat *sb,
                         ls_options *ls_opts);

void display_file_info(const char *file, const struct stat *sb,
                       ls_options *ls_opts);

void print_long_format(const char *file, const struct stat *sb,
                       ls_options *ls_opts);

#endif