#ifndef FILE_PROCESSING_H
#define FILE_PROCESSING_H

int process_paths(const char **paths, ls_options *ls_opts);

int process_entry(FTSENT *entry, ls_options *ls_opts);

int print_entry(FTSENT *entry, ls_options *ls_opts);

void print_long_format(FTSENT *entry, ls_options *ls_opts);

#endif