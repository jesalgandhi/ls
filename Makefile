
all: clean ls

ls: ls.c
	cc -ansi -g -Wall -Werror -Wextra -Wformat=2 -Wjump-misses-init \
	-Wlogical-op -Wpedantic -Wshadow \
	ls.c arg_parsing.c flag_handlers.c file_processing.c -o ls

clean:
	rm -f ls