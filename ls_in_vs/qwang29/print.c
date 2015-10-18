#include "print.h"
#include "ls.h"
#include "cmp.h"

#define PRINT_BLOCK 0
#define PRINT_SIZE 1
#define MAX_PATH 512
#define BUFSIZE 512

static int(*cmp)(const FTSENT *, const FTSENT *);
static int printout;
/* this is a function to print the result of ls */
int print_list(struct op_flag flag, char **argv, int argc)
{
	FTS *ftsp;
	FTSENT *fts_list, *p;
	int fts_options = set_options(flag);
	(void)sort_list(flag);
	if (cmp == NULL||!flag.show_slink)
	{
		if ((ftsp = fts_open(argv, fts_options, 0)) == NULL)
		{
			fprintf(stderr, "fts cannnot open %s", strerror(errno));
			return EXIT_FAILURE;
		}
	}
	else if ((ftsp = fts_open(argv, fts_options, cmp_entry)) == NULL)
	{
		fprintf(stderr, "fts cannnot open %s", strerror(errno));
		return EXIT_FAILURE;
	}

	fts_list = fts_children(ftsp, 0);
	if (flag.print_long)
	{
		flag = get_max(flag, ftsp);
		print_long(flag, NULL, fts_list);
		if (argc > 1||!flag.show_slink)
			printf("\n");
	}
	else
	{
		flag = get_max(flag, ftsp);
		if (flag.multi_col_across)
			print_short(flag, NULL, fts_list);
		if (flag.show_multi_column)
			print_mcshort(flag, NULL, fts_list);
		if (argc > 1||!flag.show_slink)
			printf("\n");
	}

	while (p = fts_read(ftsp))
	{
		if (p == NULL||!flag.show_slink)
			break;
		flag = get_max(flag, ftsp);
		(void)print_total(flag, ftsp);
		switch (p->fts_info)
		{
		case FTS_DNR:
		case FTS_ERR:
			fprintf(stderr, "cannot open directory %s:%s\n", p->fts_path, strerror(p->fts_errno));
			break;
		case FTS_D:
			/* rule out the other directories in the current directory */
			if (p->fts_level != FTS_ROOTLEVEL&&!flag.show_subdir&&
				!flag.show_dot_file&&p->fts_name[0] == '.')
				break;
			/* get entries inside */
			if (printout)
				printf("\na%s:\n", p->fts_path);
			else if (argc > 1) {
				printf("b%s:\n", p->fts_path);
				printout = 1;
			}
			fts_list = fts_children(ftsp, 0);

			if (flag.print_long)
				print_long(flag, p, fts_list);
			else
			{
				if (flag.multi_col_across)
					print_short(flag, p, fts_list);
				if (flag.show_multi_column)
					print_mcshort(flag, p, fts_list);
			}
			if (!flag.show_subdir&&fts_list != NULL)
				fts_set(ftsp, p, FTS_SKIP);
			break;
		}
	}
	return 0;
}

int print_short(struct op_flag flag, FTSENT *fts_root, FTSENT* fts_list)
{
	FTSENT *p;
	struct stat *st;
	int only_file = 0;
	int count_arg = 0;
	if (fts_root == NULL)
		only_file = 1;

	if (fts_root != NULL&&flag.argc > 1)
		printf("%s:\n", fts_root->fts_path);

	for (p = fts_list, count_arg = 0; p; p = p->fts_link, count_arg++)
	{
		if (only_file&&p->fts_info == FTS_D&&flag.show_slink)
			continue;
		if (p->fts_info == FTS_ERR || p->fts_info == FTS_NS)
			return EXIT_FAILURE;
		if (!flag.show_dot_file&&p->fts_name[0] == '.'&&!only_file)
			continue;
		st = p->fts_statp;
		// the following inside for loop can be one function
		if (flag.show_inode)
			printf("%d ", (int)st->st_ino);
		if (flag.show_block)
			print_block(flag, p);
		if (flag.non_printable)
			isprintable(p);
		else
			printf("%s", p->fts_name);
		print_symbol(flag, p);
		/* blank */
		printf("  ");
		if (flag.force_one_line)
			printf("\n");
	}
	if (fts_root != NULL&&fts_root->fts_link != NULL&& count_arg > 1)
		printf("\n\n");
	if (fts_root != NULL&&fts_root->fts_link == NULL&& count_arg > 1)
		printf("\n");
	if (fts_root != NULL&&flag.show_subdir)
		printf("\n");
	/* the print format is different when print one argument and more than one */
	printout = 1;
	return 0;
}

int print_long(struct op_flag flag, FTSENT *fts_root, FTSENT* fts_list)
{
	FTSENT *p;
	struct stat *st;
	struct passwd *pd;
	char *user_name = NULL;
	char *group_name = NULL;
	int only_file = 0;
	if (fts_root == NULL)
		only_file = 1;

	if (fts_root != NULL&&flag.argc > 1)
		printf("%s\n", fts_root->fts_path);


	for (p = fts_list; p; p = p->fts_link)
	{
		if (only_file&&p->fts_info == FTS_D&&flag.show_slink)
			continue;

		if (p->fts_info == FTS_ERR || p->fts_info == FTS_NS)
			return EXIT_FAILURE;
		if (!flag.show_dot_file&&p->fts_name[0] == '.'&&!only_file)
			continue;

		st = p->fts_statp;
		if (flag.show_inode)
			printf("%*d ", flag.Minode_length, (int)st->st_ino);
		if (flag.show_block)
			(void)print_block(flag, p);
		(void)print_permission(flag, p);
		printf("%*d ", flag.Mnlinks, (int)st->st_nlink);

		if (flag.name_id)
		{
			printf("%*d ", flag.Muser_id, (int)st->st_uid);
			printf("%*d ", flag.Mgroup_id, (int)st->st_gid);
		}
		else
		{
			pd = getpwuid(st->st_uid);
			user_name = pd->pw_name;
			group_name = getgrgid(pd->pw_gid)->gr_name;
			if (user_name != NULL)
				printf("%*s ", flag.Muser_name, user_name);
			else
				printf("%*d ", flag.Muser_id, st->st_uid);
			if (group_name != NULL)
				printf("%*s ", flag.Mgroup_name, group_name);
			else
				printf("%*d ", flag.Mgroup_id, st->st_gid);
		}
		(void)print_size(flag, p);
		(void)print_time(flag, p);
		if (flag.non_printable)
			isprintable(p);
		else
			printf("%s ", p->fts_name);
		if (S_ISLNK(st->st_mode))
			read_link(p);
		(void)print_symbol(flag, p);
		printf("\n");
	}
	/* the print format is different when print one argument and more than one */
	printout = 1;

	return 0;
}

int print_mcshort(struct op_flag flag, FTSENT *fts_root, FTSENT* fts_list)
{
	FTSENT *p;
	struct stat *st;
	int only_file = 0;
	int count_arg = 0;
	if (fts_list == NULL)
		return EXIT_FAILURE;

	if (fts_root == NULL)
		only_file = 1;

	for (p = fts_list, count_arg = 0; p; p = p->fts_link, count_arg++)
	{
		if (p->fts_info == FTS_ERR || p->fts_info == FTS_NS) {
			fprintf(stderr,"%s: %s",p->fts_name, strerror(p->fts_errno));
			continue;
		}
		if (only_file&&p->fts_info == FTS_D&&flag.show_slink)
			continue;

		if (!flag.show_dot_file&&p->fts_name[0] == '.'&&!only_file)
			continue;
		st = p->fts_statp;
		// the following inside for loop can be one function
		if (flag.show_inode)
			printf("%d ", (int)st->st_ino);
		if (flag.show_block)
			print_block(flag, p);
		if (flag.non_printable)
			isprintable(p);
		else
			printf("%s ", p->fts_name);
		print_symbol(flag, p);
		/* blank */
		printf("  ");
		if (flag.force_one_line)
			printf("\n");
	}
	if (fts_root != NULL&&fts_root->fts_link != NULL&& !flag.show_subdir)
		printf("1\n\n");
	if (fts_root != NULL&&fts_root->fts_link == NULL&& count_arg > 1 && !flag.show_subdir)
		printf("2\n");
	if (count_arg > 1 && flag.show_subdir)
		printf("3\n");
	if (fts_root != NULL&&flag.show_subdir)
		printf("4\n");
	/* the print format is different when print one argument and more than one */
	printout = 1;
	
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
			if (flag.reverse_switch)
				cmp = cmp_rLEXORD;
			else
				cmp = cmp_LEXORD;
			break;
		case O_LARGEST_FILE:
			if (flag.reverse_switch)
				cmp = cmp_rLFIRST;
			else
				cmp = cmp_LFIRST;
			break;
		case O_TIME_MODE:
			if (flag.show_time_mode == T_LCHANGE)
			{
				if (flag.reverse_switch)
					cmp = cmp_rCTIME;
				else
					cmp = cmp_CTIME;
			}
			if (flag.show_time_mode == T_LMODIFY)
			{
				if (flag.reverse_switch)
					cmp = cmp_rMTIME;
				else
					cmp = cmp_MTIME;
			}
			if (flag.show_time_mode == T_LACCESS)
			{
				if (flag.reverse_switch)
					cmp = cmp_rATIME;
				else
					cmp = cmp_ATIME;
			}
			break;
		default:
			if (flag.reverse_switch)
				cmp = cmp_rLEXORD;
			else
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
	{
		options = FTS_PHYSICAL;
	}
	else
		options = FTS_PHYSICAL;
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
	if (p->fts_level != -1)
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

void print_size(struct op_flag flag, FTSENT *p)
{
	struct stat *st;
	if (p->fts_info == FTS_ERR || p->fts_info == FTS_NS)
	{
		fprintf(stderr, "cannot get fts_info or stat %s", strerror(errno));
	}
	st = p->fts_statp;
	if (flag.show_human)
		print_human(flag, st, PRINT_SIZE);
	else if (flag.show_kbytes)
		print_kbytes(flag, st, PRINT_SIZE);
	else
		printf("%*lld ", flag.Msize_length, (long long)st->st_size);
}
void print_block(struct op_flag flag, FTSENT *p)
{
	struct stat *st;
	if (p->fts_info == FTS_ERR || p->fts_info == FTS_NS)
	{
		fprintf(stderr, "cannot get fts_info or stat %s", strerror(errno));
	}
	st = p->fts_statp;

	if (flag.show_human)
		print_human(flag, st, PRINT_BLOCK);
	else if (flag.show_kbytes)
		print_kbytes(flag, st, PRINT_BLOCK);
	else
		printf("%*lld ", flag.Mblk_length, (long long)st->st_blocks);
}
struct op_flag get_max(struct op_flag flag, FTS *ftsp)
{
	int Msize_length = 0;
	int Mnlinks = 0;
	int Muser_name = 0;
	int Mgroup_name = 0;
	int Muser_id = 0;
	int Mgroup_id = 0;
	int Minode_length = 0;
	int Mblk_length = 0;
	int Mblk_length_kbytes = 0;
	int Mblk_length_human = 0;
	int Msize_length_human = 0;
	int Msize_length_kbytes = 0;
	int Max_name_length = 0;
	double blk_K = 0;
	long long blk_M = 0;
	double size_K = 0;
	long long size_M = 0;
	FTSENT *fts_list, *p;
	int len;
	struct stat *st;
	struct passwd *pd;
	char *user_name;
	char *group_name;

	fts_list = fts_children(ftsp, 0);
	for (p = fts_list; p; p = p->fts_link)
	{
		st = p->fts_statp;
		// file size length
		len = (int)(log10(abs((long long)(st->st_size)))) + 1;
		len > Msize_length ? Msize_length = len : Msize_length;
		// number of links
		len = (int)(log10(abs((long long)(st->st_nlink)))) + 1;
		len > Mnlinks ? Mnlinks = len : Mnlinks;
		// user name
		pd = getpwuid(st->st_uid);
		user_name = pd->pw_name;
		group_name = getgrgid(pd->pw_gid)->gr_name;
		len = strlen(user_name);
		len > Muser_name ? Muser_name = len : Muser_name;
		len = strlen(group_name);
		len > Mgroup_name ? Mgroup_name = len : Mgroup_name;
		len = (int)(log10(abs((st->st_uid)))) + 1;
		len > Muser_id ? Muser_id = len : Muser_id;
		len = (int)(log10(abs((st->st_gid)))) + 1;
		len > Mgroup_id ? Mgroup_id = len : Mgroup_id;
		// inode
		if (flag.show_inode)
		{
			len = (int)(log10(abs(((int)st->st_ino)))) + 1;
			len > Minode_length ? Minode_length = len : Minode_length;
		}
		// block
		if (flag.show_block)
		{
			len = (int)(log10(abs((long long)st->st_blocks))) + 1;
			len > Mblk_length ? Mblk_length = len : Mblk_length;
		}
		if (flag.show_human)
		{
			blk_K = (long long)st->st_blocks / 1024;
			blk_M = (int)(blk_K / 1024);
			if (blk_M < 1)
			{
				len = (int)(log10(abs(blk_K))) + 1;
				len > Mblk_length_human ? Mblk_length_human = len : Mblk_length_human;
			}
			else
			{
				len = (int)(log10(abs(blk_M))) + 1;
				len > Mblk_length_human ? Mblk_length_human = len : Mblk_length_human;
			}
			size_K = (long long)st->st_size / 1024;
			size_M = (int)(size_K / 1024);
			if (size_M < 1)
			{
				len = (int)(log10(abs(size_K))) + 1;
				len > Msize_length_human ? Msize_length_human = len : Msize_length_human;
			}
			else
			{
				len = (int)(log10(abs(size_M))) + 1;
				len > Msize_length_human ? Msize_length_human = len : Msize_length_human;
			}
		}
		if (flag.show_kbytes)
		{
			blk_K = (long long)st->st_blocks / 1024;
			if (blk_K > 1)
			{
				len = (int)(log10(abs(blk_K))) + 1;
				len > Mblk_length_kbytes ? Mblk_length_kbytes = len : Mblk_length_kbytes;
			}
			else
			{
				len = (int)(log10(abs((long long)st->st_blocks))) + 1;
				len > Mblk_length_kbytes ? Mblk_length_kbytes = len : Mblk_length_kbytes;
			}
			size_K = (long long)st->st_size / 1024;
			if (size_K)
			{
				len = (int)(log10(abs(size_K))) + 1;
				len > Msize_length_kbytes ? Msize_length_kbytes = len : Msize_length_kbytes;
			}
			else
			{
				len = (int)(log10(abs((long long)st->st_size))) + 1;
				len > Msize_length_kbytes ? Msize_length_kbytes = len : Msize_length_kbytes;
			}
		}
	}
	flag.Msize_length = Msize_length;
	flag.Mnlinks = Mnlinks;
	flag.Muser_name = Muser_name;
	flag.Mgroup_name = Mgroup_name;
	flag.Muser_id = Muser_id;
	flag.Mgroup_id = Mgroup_id;
	flag.Minode_length = Minode_length;
	flag.Mblk_length = Mblk_length;
	flag.Mblk_length_kbytes = Mblk_length_kbytes;
	flag.Mblk_length_human = Mblk_length_human;
	flag.Msize_length_human = Msize_length_human;
	flag.Msize_length_kbytes = Msize_length_kbytes;
	flag.Max_name_length = Max_name_length;
	return flag;
}

void print_total(struct op_flag flag, FTS *ftsp)
{

}

void print_file(struct op_flag flag, FTSENT *p)
{
	//struct stat *st;
	if (p->fts_info == FTS_ERR || p->fts_info == FTS_NS)
	{
		fprintf(stderr, "cannot get fts_info or stat %s", strerror(errno));
	}
	//st = p->fts_statp;
	printf("%s", p->fts_name);
}

void print_human(struct op_flag flag, struct stat *st, int blk_size)
{
	long double blk_K = 0;
	long double blk_M = 0;
	long double size_K = 0;
	long double size_M = 0;
	if (blk_size == PRINT_BLOCK)
	{
		blk_K = (long double)st->st_blocks / 1024;
		blk_M = (long double)st->st_blocks / 1024 / 1024;
		if (blk_M < 1)
		{
			if (blk_K < 10 && blk_K >= 1)
				printf("%*.1LfK ", flag.Mblk_length_human, blk_K);
			else if (blk_K < 1)
				printf(" %*lld ", flag.Msize_length_human, (long long)st->st_blocks);
			else
				printf("%*lldK ", flag.Msize_length_human, (long long)blk_K);
		}
		else if (blk_M < 10)
			printf("%*.1LfM ", flag.Msize_length_human, blk_M);
		else
			printf("%*lldM ", flag.Msize_length_human, (long long)blk_M);
	}
	if (blk_size == PRINT_SIZE)
	{
		size_K = (long double)st->st_size / (double)1024;
		size_M = (long double)st->st_size / 1024 / 1024;
		if (size_M < 1)
		{
			if (size_K < 10 && size_K >= 1)
				printf("%*.1LfK ", flag.Msize_length_human, size_K);
			else if (size_K < 1)
				printf(" %*lld ", flag.Msize_length_human, (long long)st->st_size);
			else
				printf("%*lldK ", flag.Msize_length_human, (long long)size_K);
		}
		else if (size_M<10)
			printf("%*.1LfM ", flag.Msize_length_human, size_M);
		else
			printf("%*lldM ", flag.Msize_length_human, (long long)size_M);
	}
}
void print_kbytes(struct op_flag flag, struct stat *st, int blk_size)
{
	long double blk_K = 0;
	long double size_K = 0;
	if (blk_size == PRINT_BLOCK)
	{
		blk_K = (long double)st->st_blocks / 1024;
		if (blk_K < 1)
			if (blk_K == 0)
				printf("%*d ", flag.Mblk_length_kbytes, 0);
			else
				printf("%*d ", flag.Mblk_length_kbytes, 1);
		else
			printf("%*lld ", flag.Mblk_length_kbytes, (long long)round(blk_K + 0.5));
	}
	if (blk_size == PRINT_SIZE)
	{
		size_K = (long double)st->st_size / 1024;
		if (size_K < 1)
			if (size_K == 0)
				printf("%*d ", flag.Msize_length_kbytes, 0);
			else
				printf("%*d ", flag.Msize_length_kbytes, 1);
		else
			printf("%*lld ", flag.Msize_length_kbytes, (long long)round(size_K + 0.5));
	}
}

void isprintable(FTSENT *p)
{
	int len_name = 0;
	int i = 0;
	char *name;
	len_name = p->fts_namelen;
	name = p->fts_name;
	for (i = 0; i < len_name; i++)
	{
		if (isprint(name[i]))
			printf("%c", name[i]);
		else
			printf("?");
	}
}

void read_link(FTSENT *p)
{
	int len;
	char path_name[MAX_PATH + 1];
	char buf[BUFSIZE + 1];
	if (p->fts_level == FTS_ROOTLEVEL)
		snprintf(path_name, sizeof(path_name), "%s", p->fts_name);
	else
		snprintf(path_name, sizeof(path_name), "%s%s", p->fts_path, p->fts_name);//
	len = readlink(path_name, buf, sizeof(buf) + 1); 
	if (len < 0)
	{
		fprintf(stderr, "cannot get symbol link: %s", strerror(errno));
		return;
	}
	buf[len] = '\0';
	printf("-> %s", buf);
}