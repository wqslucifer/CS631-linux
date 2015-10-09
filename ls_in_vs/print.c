#include "print.h"
#include "ls.h"
#include "cmp.h"

/* this is a function to print the result of ls */
int print_list(struct op_flag flag, char **argv, int argc)
{
	FTS *ftsp;
	FTSENT *fts_list, *p;
	int fts_options = FTS_COMFOLLOW|FTS_SEEDOT;
	
	if ((ftsp = fts_open(argv, fts_options, cmp_pathlist)) == NULL)
	{
		fprintf(stderr, "fts cannnot open %s", strerror(errno));
		return EXIT_FAILURE;
	}
	while (p = fts_read(ftsp))
	{
		printf("%s:\n", p->fts_name);
	}


	return 0;
}

int print_short(struct op_flag flag, char **entries, int count_entries)
{
	struct stat st;
	struct winsize ws;
	int total_blocks = 0;
	int i = 0;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws); // size in each letter

	//printf("columns %d\n", ws.ws_col);

	if (flag.show_block) // -s
	{
		total_blocks = get_block(entries);
		printf("total %d\n", total_blocks);
	}
	for (i = 0; i < count_entries; i++)
	{
		// the following inside for loop can be one function !!!
		if (flag.show_inode)
		{
			if ((lstat(entries[i], &st)) < 0)
			{
				fprintf(stderr, "cannot get stat of %s: %s\n", entries[i], strerror(errno));
				return EXIT_FAILURE;
			}
			printf("%d ", (int)st.st_ino);
		}
		if (flag.show_block)
		{
			//print the block number of each entry
			if ((lstat(entries[i], &st)) < 0)
			{
				fprintf(stderr, "cannot get stat of %s: %s\n", entries[i], strerror(errno));
				return EXIT_FAILURE;
			}
			//!! error : each directory has 1 more block???, while non-dir file is correct
			printf("%lld ", (long long)st.st_blocks);
		}
		printf("%s", entries[i]);
		if (flag.show_symbol) // -F
		{
			if ((lstat(entries[i], &st)) < 0)
			{
				fprintf(stderr, "cannot get stat of %s: %s\n", entries[i], strerror(errno));
				return EXIT_FAILURE;
			}

			if (S_ISDIR(st.st_mode))
				printf("/");
			if (st.st_mode == S_IXUSR || st.st_mode == S_IXGRP || st.st_mode == S_IXOTH)
				printf("*");
			if (S_ISLNK(st.st_mode))
				printf("@");
			if (st.st_mode == S_IFWHT)
				printf("%%");
			if (S_ISSOCK(st.st_mode))
				printf("=");
			if (S_ISFIFO(st.st_mode))
				printf("|");
		}
		/* blank */
		printf("  ");
	}
	printf("\n");
	/* the print format is different when print one argument and more than one */
	return 0;
}

/* this function will call print_long and print_short inside to print
 * all subdir 
 */
int print_rec(struct op_flag flag, char **path_list)
{
	// call print_long 
	// call print_short
	return 0;
}

/* sort the input list to fit flags */
char **sort_list(struct op_flag flag, char **entries,int count_entries)
{
	if (flag.sort_switch)
	{
		switch (flag.sort_mode)
		{
		case O_LEXICO: 
			qsort(entries, count_entries, sizeof(entries), cmp_LEXORD);
			break;
		case O_SMALL_ENTRY:

			break;
		case O_OLD_ENTRY:
			break;
		case O_LARGEST_FILE:
			qsort(entries, count_entries, sizeof(entries), cmp_LFIRST);
			break;
		case O_TIME_MODE:
			if (flag.show_time_mode == T_LCHANGE)
			{

			}
			if (flag.show_time_mode == T_LMODIFY)
			{

			}
			if (flag.show_time_mode == T_LACCESS)
			{

			}
			break;
		default:
			return entries;
			break;
		}
	}
	return entries;
}

/* get total blocks of entries 
 * apply to each directory
 */
int get_block(char **entries)
{
	return 0;
}