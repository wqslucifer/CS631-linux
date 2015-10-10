//#define WINDOWS
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif // !PATH_MAX

#ifndef S_IFWHT
#define S_IFWHT 16000
#endif // S_IFWHT

#ifndef _PRINT_H
#define _PRINT_H

#define LTHAN 1
#define EQUAL 0
#define STHAN -1

#define TIME_SIZE 50

#include "ls.h"

#include <sys/ioctl.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

int print_list(struct op_flag flag, char **argv, int argc);
int print_long(struct op_flag flag, FTSENT *fts_root, FTSENT* fts_list);
int print_short(struct op_flag flag, FTSENT *fts_root, FTSENT* fts_list);
int print_rec(struct op_flag flag, char **path_list);
void sort_list(struct op_flag flag);
int get_block(char **entries);
int cmp_entry(const FTSENT **p1, const FTSENT **p2);
int set_options(struct op_flag flag);
void print_permission(struct op_flag flag, FTSENT *p);
void print_time(struct op_flag flag, FTSENT *p);
void print_symbol(struct op_flag flag, FTSENT *p);



#endif // !_PRINT_H
