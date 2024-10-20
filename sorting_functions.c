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

int
time_modified_sort(const FTSENT **a, const FTSENT **b)
{
	time_t a_time = (*a)->fts_statp->st_mtime;
	time_t b_time = (*b)->fts_statp->st_mtime;

	if (a_time > b_time) {
		return -1;
	} else if (a_time < b_time) {
		return 1;
	} else {
		return strcmp((*a)->fts_name, (*b)->fts_name);
	}
}

int
time_changed_sort(const FTSENT **a, const FTSENT **b)
{
	time_t a_time = (*a)->fts_statp->st_ctime;
	time_t b_time = (*b)->fts_statp->st_ctime;
	if (a_time > b_time) {
		return -1;
	}
	return 1;
}

int
time_accessed_sort(const FTSENT **a, const FTSENT **b)
{
	time_t a_time = (*a)->fts_statp->st_atime;
	time_t b_time = (*b)->fts_statp->st_atime;
	if (a_time > b_time) {
		return -1;
	}
	return 1;
}