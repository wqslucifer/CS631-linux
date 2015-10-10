#include "print.h"
#include "ls.h"
#include "cmp.h"


static int(*cmp)(const FTSENT *, const FTSENT *);
/* this is a function to print the result of ls */
int print_list(struct op_flag flag, char **argv, int argc)
{
	FTS *ftsp;
	FTSENT *fts_list, *p;
	int fts_options = set_options(flag);
	(void)sort_list(flag);
	if ((ftsp = fts_open(argv, fts_options, cmp_entry)) == NULL)
	{
		fprintf(stderr, "fts cannnot open %s", strerror(errno));
		return EXIT_FAILURE;
	}
	if (!flag.show_rec&&!flag.show_slink)
	{
		fts_list = fts_children(ftsp, 0);
		if (flag.print_long)
			return print_long(flag, NULL, fts_list);
		else
			return print_short(flag, NULL, fts_list);
	}
	
	while (p = fts_read(ftsp))
	{
		if (p == NULL)
			break;
		switch (p->fts_info)
		{
		case FTS_D:
			/* rule out the other directories in the current directory */
			if (p->fts_level != FTS_ROOTLEVEL)
				break;
			/* get entries inside */
			fts_list = fts_children(ftsp, 0);
			if (flag.print_long)
				return print_long(flag, p, fts_list);
			else
				return print_short(flag, p, fts_list);
			break;
		default:
			break;
		}
	}
	return 0;
}

int print_short(struct op_flag flag, FTSENT *fts_root, FTSENT* fts_list)
{
	FTSENT *p;
	struct stat *st;
	if (fts_root != NULL&&flag.argc > 1)
		printf("%s\n", fts_root->fts_path);
	//struct winsize ws;
	//ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws); // size in each letter
	//printf("columns %d\n", ws.ws_col);

	for (p = fts_list; p; p = p->fts_link)
	{
		if (p->fts_info == FTS_ERR || p->fts_info == FTS_NS)
			return EXIT_FAILURE;
		if (!flag.show_dot_file)
		{
			if (p->fts_name[0] == '.')
				continue;
		}
		st = p->fts_statp;
		// the following inside for loop can be one function
		if (flag.show_inode)
		{
			printf("%d ", (int)st->st_ino);
		}
		if (flag.show_block) // -s something goes off
		{
			printf("%lld ", (long long)st->st_blocks);
		}
		printf("%s", p->fts_name);
		print_symbol(flag, p);
		/* blank */
		printf("  ");
	}
	printf("\n");
	/* the print format is different when print one argument and more than one */

	return 0;
}

int print_long(struct op_flag flag, FTSENT *fts_root, FTSENT* fts_list)
{
	FTSENT *p;
	struct stat *st;
	struct passwd *pd;
	char *user_name = NULL;
	char *group_name = NULL;
	if (fts_root != NULL&&flag.argc > 1)
		printf("%s\n", fts_root->fts_path);
	
	for (p = fts_list; p; p = p->fts_link)
	{
		if (p->fts_info == FTS_ERR || p->fts_info == FTS_NS)
			return EXIT_FAILURE;
		if (!flag.show_dot_file)
		{
			if (p->fts_name[0] == '.')
				continue;
		}
		(void)print_permission(flag, p);
		st = p->fts_statp;
		if (flag.name_id)
		{
			printf("%5d ", (int)st->st_uid);
			printf("%5d ", (int)st->st_gid);
		}
		else
		{
			pd = getpwuid(st->st_uid);
			user_name = pd->pw_name;
			group_name = getgrgid(pd->pw_gid)->gr_name;
			if (user_name != NULL)
				printf("%s ", user_name);
			else
				printf("%5d ", st->st_uid);
			if (group_name != NULL)
				printf("%s\t", group_name);
			else
				printf("%5d ", st->st_gid);
		}
		//print_size();
		(void)print_time(flag, p);
		printf("%s", p->fts_name);
		(void)print_symbol(flag, p);
		printf("\n");
	}
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
void sort_list(struct op_flag flag)
{	
	if (flag.sort_switch)
	{
		switch (flag.sort_mode)
		{
		case O_LEXICO: 
			cmp = cmp_LEXORD;
			break;
		case O_SMALL_ENTRY: //-r
			break;
		case O_OLD_ENTRY: // -r
			break;
		case O_LARGEST_FILE:
			cmp = cmp_LFIRST;
			break;
		case O_TIME_MODE:
			if (flag.show_time_mode == T_LCHANGE)
			{
				cmp = cmp_CTIME;
			}
			if (flag.show_time_mode == T_LMODIFY)
			{
				cmp = cmp_MTIME;
			}
			if (flag.show_time_mode == T_LACCESS)
			{
				cmp = cmp_ATIME;
			}
			break;
		default:
			cmp = cmp_LEXORD;
			break;
		}
	}
	else
	{
		cmp = NULL;
	}
}

/* get total blocks of entries 
 * apply to each directory
 */
int get_block(char **entries)
{
	return 0;
}
int cmp_entry(const FTSENT **p1, const FTSENT **p2)
{
	const FTSENT *first = *p1;
	const FTSENT *second = *p2;
	if (first->fts_info == FTS_ERR || second->fts_info == FTS_ERR)
		return EQUAL;
	if (first->fts_info == FTS_NS || second->fts_info == FTS_NS)
	{
		if (first->fts_info != FTS_NS)
			return LTHAN;
		else if (second->fts_info != FTS_NS)
			return STHAN;
		else
			return cmp_LEXORD(*p1, *p2);
	}
	if (first->fts_level == 0 && first->fts_info != second->fts_info)
	{
		if (first->fts_info == FTS_D)
			return (LTHAN);
		else if (second->fts_info == FTS_D)
			return (STHAN);
	}
	return cmp(*p1, *p2);
}

int set_options(struct op_flag flag)
{
	int options = 0;
	if (flag.print_long)
		options = FTS_PHYSICAL;
	else
		options = FTS_LOGICAL;
	if (flag.show_dot)
		options |= FTS_SEEDOT;
	return options;
}

void print_permission(struct op_flag flag, FTSENT *p)
{
	struct stat *st;
	if (p->fts_info == FTS_ERR || p->fts_info == FTS_NS)
	{
		fprintf(stderr, "cannot get fts_info or stat %s", strerror(errno));
	}
	st = p->fts_statp;
	switch (st->st_mode&S_IFMT)
	{
	case S_IFBLK:
		printf("b");
		break;
	case S_IFCHR:
		printf("c");
		break;
	case S_IFDIR:
		printf("d");
		break;
	case S_IFLNK:
		printf("l");
		break;
	case S_IFSOCK:
		printf("s");
		break;
	case S_IFWHT:
		printf("w");
		break;
	default:
		printf("-");
		break;
	}
	//////////////////////////////////////////////////////////////////////////
	// owner permissions
	if (st->st_mode&S_IRWXU & S_IRUSR)
		printf("r");
	else
		printf("-");
	if (st->st_mode&S_IRWXU & S_IWUSR)
		printf("w");
	else
		printf("-");
	if (st->st_mode&S_IRWXU & S_IXUSR)
	{
		if (st->st_mode&S_IFMT&S_ISUID)
			printf("s");
		else
			printf("x");
	}
	else if (st->st_mode&S_IFMT&S_ISUID)
		printf("S");
	else
		printf("-");
	//////////////////////////////////////////////////////////////////////////
	// group permissions
	if (st->st_mode&S_IRWXG & S_IRGRP)
		printf("r");
	else
		printf("-");
	if (st->st_mode&S_IRWXG & S_IWGRP)
		printf("w");
	else
		printf("-");
	if (st->st_mode&S_IRWXG & S_IXGRP)
	{
		if (st->st_mode&S_IFMT&S_ISGID)
			printf("s");
		else
			printf("x");
	}
	else if (st->st_mode&S_IFMT&S_ISGID)
		printf("S");
	else
		printf("-");
	//////////////////////////////////////////////////////////////////////////
	// other permissions
	if (st->st_mode&S_IRWXO & S_IROTH)
		printf("r");
	else
		printf("-");
	if (st->st_mode&S_IRWXO & S_IWOTH)
		printf("w");
	else
		printf("-");
	if (st->st_mode&S_IRWXO & S_IXOTH)
	{
		if (st->st_mode&S_ISVTX)
			printf("t");
		else
			printf("x");
	}
	else if (st->st_mode&S_ISVTX)
		printf("T");
	else
		printf("-");
	printf("+ ");
}


void print_time(struct op_flag flag, FTSENT *p)
{
	struct stat *st;
	struct tm* tmptr;
	char str_time[TIME_SIZE];
	time_t t;
	st = p->fts_statp;
	switch (flag.show_time_mode)
	{
	case T_LACCESS:
		t = st->st_atime;
		break;
	case T_LCHANGE:
		t = st->st_ctime;
		break;
	case T_LMODIFY:
		t = st->st_mtime;
		break;
	default:
		t = st->st_mtime;
		break;
	}
	tmptr = localtime(&t);
	if (strftime(str_time, sizeof(str_time), "%b %e %H:%M ", tmptr))
	{
		printf("%s", str_time);
	}
}
void print_symbol(struct op_flag flag, FTSENT *p)
{
	struct stat *st;
	st = p->fts_statp;
	if (flag.show_symbol) // -F
	{
		if (S_ISDIR(st->st_mode))
			printf("/");
		if (st->st_mode == S_IXUSR || st->st_mode == S_IXGRP || st->st_mode == S_IXOTH)
			printf("*");
		if (S_ISLNK(st->st_mode))
			printf("@");
		if (st->st_mode == S_IFWHT)
			printf("%%");
		if (S_ISSOCK(st->st_mode))
			printf("=");
		if (S_ISFIFO(st->st_mode))
			printf("|");
	}
}