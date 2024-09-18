#ifndef FILE_PROCESSING_H
#define FILE_PROCESSING_H

#include <dirent.h>
#include <stdio.h>

#include "flag_handlers.h"
#include "ls_options.h"

int process_directory(const char *path, ls_options *ls_opts);

#endif