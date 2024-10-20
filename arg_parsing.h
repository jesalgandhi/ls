#ifndef ARG_PARSING_H
#define ARG_PARSING_H

#include "ls_options.h"


void usage();

/* Returns optind from getopt */
int parse_commandline_args(int argc, char **argv, ls_options *ls_opts);

#endif