#include "sorting_functions.h"

#include <sys/stat.h>
#include <sys/types.h>

#include <fts.h>
#include <string.h>

int
lexicographical_sort(const FTSENT **a, const FTSENT **b)
{
	return strcmp((*a)->fts_name, (*b)->fts_name);
}

int
reverse_lexicographical_sort(const FTSENT **a, const FTSENT **b)
{
	return strcmp((*b)->fts_name, (*a)->fts_name);
}