#ifndef SORTING_FUNCTIONS_H

#define SORTING_FUNCTIONS_H

#include <sys/stat.h>
#include <sys/types.h>

#include <fts.h>

int lexicographical_sort(const FTSENT **a, const FTSENT **b);

int reverse_lexicographical_sort(const FTSENT **a, const FTSENT **b);

int time_modified_sort(const FTSENT **a, const FTSENT **b);

int time_changed_sort(const FTSENT **a, const FTSENT **b);

int time_accessed_sort(const FTSENT **a, const FTSENT **b);

#endif