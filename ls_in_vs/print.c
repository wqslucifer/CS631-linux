#include "print.h"
#include "ls.h"
#include "cmp.h"

/* this is a function to print the result of ls */
int print_list(struct op_flag flag, char **path_list, int argc)
{
	DIR *dir;
	struct dirent *dirp;
	char *local_dir = "./";
	char **entries = NULL;
	char *dir_name = NULL;
	int count_entries = 0;
	struct stat st;
	int i = 0;
	int j = 0;
	char cur_dir[BUFF_SIZE];
	getcwd(cur_dir,BUFF_SIZE);
	//char sub_dir[BUFF_SIZE];

	/* sort when the sort_switch on, otherwise just separate directory and non-directory */
	if (flag.sort_switch) /* -f */
	{
		qsort(path_list, argc, sizeof(path_list), cmp_pathlist);
	}
	else
	{
		qsort(path_list, argc, sizeof(path_list), nsort_pathlist);
	}
	/* all entries stored in path_list, non-dir first */	
	if (flag.show_subdir&&flag.show_rec) //-R not -d
	{
		return print_rec(flag, path_list);
	}
	else
	{
		/* if there is not path_list input, then default input path is local directory ./ */
		if (argc == 0)
		{
			path_list[argc] = local_dir;
			argc++;
		}
		/* the print format is different when print one argument and more than one */
		for (i = 0; i < argc; i++)
		{
			entries = (char **)malloc(sizeof(char**));
			if ((lstat(path_list[i], &st)) < 0)
			{
				fprintf(stderr, "cannot get stat of %s: %s", path_list[i], strerror(errno));
				return EXIT_FAILURE;
			}
			if (!S_ISDIR(st.st_mode))
			{
				//print_file();
			}
			else
			{
				if (argc > 1)
				{
					printf("%s:\n", path_list[i]);
				}
				if ((dir = opendir(path_list[i])) == NULL)
				{
					fprintf(stderr, "cannot open dir: %s\n", strerror(errno));
					return EXIT_FAILURE;
				}
				else
				{
					while ((dirp = readdir(dir)) != NULL)
					{
						dir_name = (char *)malloc((PATH_MAX + 1)*sizeof(char));
						strcpy(dir_name, dirp->d_name);
						entries = (char **)realloc(entries, (j + 2)*sizeof(char **));
						entries[j] = dir_name;
						j++;
					};
					count_entries = j;
					entries[count_entries] = NULL;
				}
				//////////////////////////////////////////////////////////////////////////
				chdir(path_list[i]);
				entries = sort_list(flag, entries, count_entries);
				if (flag.print_long)
				{
					print_long(flag, entries, count_entries);
				}
				else
				{
					print_short(flag, entries, count_entries);
				}
				chdir(cur_dir);
				/* print extra \n for multi-row output */
				if (i < argc - 1)
				{
					printf("\n");
				}
				//////////////////////////////////////////////////////////////////////////
				/* clean */
				j = 0;
				count_entries = 0;
				while (entries[j] != NULL)
				{
					free(entries[j]);
					entries[i] = NULL;
					j++;
				}
				free(entries);
				entries = NULL;
				closedir(dir);
				j = 0;
			}
		}

	}
	return 0;
}

int print_long(struct op_flag flag, char **entries, int count_entries)
{
	int i = 0;
	for (i = 0; i < count_entries; i++)
	{
		printf("%s ", entries[i]);
	}
	printf("\n");

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