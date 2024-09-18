#ifndef LS_H
#define LS_H

/* enabled if user does not provide any files */
int current_dir;

/* Available options */
int o_list_all_except_dot;       /* -A: list all entries except . or .. */
int o_include_dot_entries;       /* -a: include entries that start with . */
int o_use_status_time;           /* -c: use file status change time */
int o_list_directories_as_files; /* -d: list directories as plain files */
int o_type_indicate;             /* -F: append type symbol to listed files */
int o_no_sorting;                /* -f: do not sort the output */
int o_human_readable_size;       /* -h: show sizes in human readable format */
int o_print_inode;               /* -i: print inode numbers for each file */
int o_report_kb;                 /* -k: show sizes in kilobytes */
int o_long_format;               /* -l: use long listing format */
int o_numeric_ids;               /* -n: show numeric owner and group ids */
int o_f_non_printable;     /* -q: force printing of non printable characters */
int o_recursive;           /* -R: recursively list subdirectories */
int o_reverse_sort;        /* -r: reverse sort order */
int o_sort_by_size;        /* -S: sort: file size */
int o_display_block_usage; /* -s: show #blocks used by each file */
int o_sort_by_mod_time;    /* -t: sort: modification time */
int o_use_access_time;     /* -u: use access time */
int o_raw_print_non_printable; /* -w: print non printable characters raw */

#endif