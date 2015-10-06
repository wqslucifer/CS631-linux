//#define WINDOWS
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif // !PATH_MAX

#ifndef S_IFWHT
#define S_IFWHT 16000
#endif // S_IFWHT

#ifndef _PRINT_H
#define _PRINT_H


#include "ls.h"
#include <sys/ioctl.h>

int print_list(struct op_flag flag, char **path_list, int argc);
int print_long(struct op_flag flag, char **entries, int count_entries);
int print_short(struct op_flag flag, char **entries, int count_entries);
int print_rec(struct op_flag flag, char **path_list);
char **sort_list(struct op_flag flag, char **entries, int count_entries);
int get_block(char **entries);
#endif // !_PRINT_H
