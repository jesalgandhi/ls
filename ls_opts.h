#ifndef LS_OPTIONS_H
#define LS_OPTIONS_H

typedef struct {
	int current_dir;
	/* Available flags */
	int o_list_all_except_dot;
	int o_include_dot_entries;
	int o_use_status_time;
	int o_list_directories_as_files;
	int o_type_indicate;
	int o_no_sorting;
	int o_human_readable_size;
	int o_print_inode;
	int o_report_kb;
	int o_long_format;
	int o_numeric_ids;
	int o_f_non_printable;
	int o_recursive;
	int o_reverse_sort;
	int o_sort_by_size;
	int o_display_block_usage;
	int o_sort_by_mod_time;
	int o_use_access_time;
	int o_raw_print_non_printable;
} ls_options;

#endif