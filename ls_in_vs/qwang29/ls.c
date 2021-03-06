/*
ls.c 
 */
#include "ls.h"
#include "print.h"

int main(int argc, char **argv, char **env)
{
	int op;
	struct op_flag flag;
	static char cur_dir[] = "./";
	char *cur_dir_argv[] = { cur_dir, NULL };
	flag = initial_flag(flag, env);	/* set initial flag for user and terminal */
#ifndef DEBUG
	setprogname(argv[PROGRAM_NAME]);
#endif
	while ((op = getopt(argc, argv, "AacCdFfhiklnqRrSstuwx1")) != -1)
	{
		switch (op)
		{
		case 'A':
			flag.show_dot_file = 1;
			break;
		case 'a':
			flag.show_dot = 1;
			flag.show_dot_file = 1;
			break;
		case 'c':
			flag.sort_switch = 1; // on
			flag.sort_mode = O_TIME_MODE;
			flag.show_time_mode = T_LCHANGE;	/* time: file status last changed */
			break;
		case 'C':
			flag.show_multi_column = 1;
			flag.multi_col_across = 0;
			break;
		case 'd':
			flag.show_slink = 0;
			break;
		case 'F':
			flag.show_symbol = 1;	// -F
			break;
		case 'f':
			flag.sort_switch = 0;
			break;
		case 'h':	/* override k */
			flag.show_human = 1;
			flag.show_kbytes = 0;
			break;
		case 'i':
			flag.show_inode = 1; //inode
			break;
		case 'k':	/* override h */
			flag.show_kbytes = 1;
			flag.show_human = 0;
			break;
		case 'l'://L
			flag.print_long = 1;
			break;
		case 'n':
			flag.print_long = 1;
			flag.name_id = 1;
			flag.show_slink = 0;
			break;
		case 'q':
			flag.non_printable = 1;
			break;
		case 'R':
			flag.show_subdir = 1;
			break;
		case 'r':
			flag.reverse_switch = 1;
			break;
		case 'S':
			flag.sort_switch = 1;
			flag.sort_mode = O_LARGEST_FILE;
			break;
		case 's':
			flag.show_block = 1;
			break;
		case 't':
			flag.sort_switch = 1;
			flag.sort_mode = O_TIME_MODE;
			flag.show_time_mode = T_LMODIFY;
			break;
		case 'u':
			flag.sort_switch = 1;
			flag.sort_mode = O_TIME_MODE;
			flag.show_time_mode = T_LACCESS;
			break;
		case 'w':
			flag.non_printable = 0;
			break;
		case 'x':
			flag.show_multi_column = 0;
			flag.multi_col_across = 1;
			break;
		case '1'://one
			flag.force_one_line = 1;
			break;
		default:
		case '?':
			usage();
			break;
		}
	}

	argc -= optind;
	argv += optind;

	if (!argc)
	{
		argv = cur_dir_argv;
		argc++;
	}
	flag.argc = argc;

	if (print_list(flag, argv, argc) > 0)
	{
		fprintf(stderr,"print list error: %s\n",strerror(errno));
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

void usage(void)
{
	printf("usage\n");
}

struct op_flag initial_flag(struct op_flag flag, char **env)
{
	/* terminal = 1 */
	// get if terminal
	int termianl = 1;
	int userid = getuid();
	if (userid == ROOT_ID)
	{
		/* root user */
		// default -A 
		flag.show_dot_file = 1;
		flag.user_type = 0;
	}
	else
	{
		/* regular user */
		flag.show_dot = 0;
		flag.show_dot_file = 0;
		flag.name_id = 0;
		flag.non_printable = 1;
		flag.print_long = 0;
		flag.reverse_switch = 0;
		flag.show_block = 0;
		flag.show_inode = 0;
		flag.show_rec = 1;
		flag.show_human = 0;//-h
		flag.show_kbytes = 0;//-k
		flag.show_slink = 1;
		flag.show_subdir = 0;//-R
		flag.show_symbol = 0;//-F
		flag.show_time_mode = T_LMODIFY;
		flag.sort_mode = O_LEXICO;
		flag.sort_switch = 1;//-f
		flag.user_type = 1;
		flag.multi_col_across = 0;
		if (termianl)
		{
			flag.show_multi_column = 1;
			flag.terminal = 1;
			flag.force_one_line = 0;
		}
		else
		{
			flag.show_multi_column = 0;
			flag.terminal = 0;
			flag.force_one_line = 1;
		}
	}
	flag.Msize_length = 0;
	flag.Mnlinks = 0;
	flag.Muser_name = 0;
	flag.Mgroup_name = 0;
	flag.Muser_id = 0;
	flag.Mgroup_id = 0;
	flag.Minode_length = 0;
	flag.Mblk_length = 0;
	return flag;
}

/* get different time of time */
struct file_time get_time(struct file_time ft)
{
	return ft;
}

void print_flag(struct op_flag op)
{
	printf("force_one_line: %d\n", op.force_one_line);
	printf("name_id: %d\n", op.name_id);
	printf("non_printable: %d\n", op.non_printable);
	printf("print_long: %d\n", op.print_long);
	printf("reverse_switch: %d\n", op.reverse_switch);
	printf("show_block: %d\n", op.show_block);
	printf("show_dot: %d\n", op.show_dot);
	printf("show_dot_file: %d\n", op.show_dot_file);
	printf("show_inode: %d\n", op.show_inode);
	printf("show_multi_column: %d\n", op.show_multi_column);
	printf("show_rec: %d\n", op.show_rec);
	printf("show_slink: %d\n", op.show_slink);
	printf("show_subdir: %d\n", op.show_subdir);
	printf("show_symbol: %d\n", op.show_symbol);
	printf("show_time_mode: %d\n", op.show_time_mode);
	printf("sort_mode: %d\n", op.sort_mode);
	printf("sort_switch: %d\n", op.sort_switch);
	printf("terminal: %d\n", op.terminal);
	printf("user_type: %d\n", op.user_type);
}