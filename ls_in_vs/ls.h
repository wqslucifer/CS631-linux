#define DEBUG

#ifndef _MYLS_H
#define _MYLS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <fts.h>

#ifdef WINDOWS
#include "unistd.h"
#include "dirent.h"
#else
#include <unistd.h>
#include <dirent.h>
#endif

#define PROGRAM_NAME 0
#define ROOT_ID 0
/* time mode */
#define T_LCHANGE 0
#define T_LMODIFY 1
#define T_LACCESS 2
/* sort mode */
#define O_LEXICO 0			/* 0: lexicographical order */
#define O_SMALL_ENTRY 1		/* 1: smallest entries first*/
#define O_OLD_ENTRY 2		/* 2: oldest entries first*/
#define O_LARGEST_FILE 3	/* 3: largest file first*/
#define O_TIME_MODE 4		/* 4: sorted by time ---->> subselected show_time_mode*/

#define BUFF_SIZE 4096


void usage(void);
struct op_flag initial_flag(struct op_flag flag, char **env);
void print_flag(struct op_flag op);



struct op_flag
{
	int user_type;			/* 0: super-user
							* 1: local-user
							*/
	/* device type */
	int terminal;			/* output is a terminal or not */

	/* DIR switch */
	int show_dot;			/* show '.' and '.. */
	int show_dot_file;		/* show directory entries whose name begin with a dot ('.') */

	/* detail mode */
	int show_multi_column;	/* Force multi-column output; this is the default when output is to a terminal */
	int multi_col_sort;		/* -x:  */
	int force_one_line;		/* -1: force output to be one entry per line. Default when output is not to a terminal */
	int show_time_mode;		/* time mode:
							* 0: time last changed							// -t -l
							* 1: time recently modified first				// -t -l
							* 2: time last access							// -t -l
							*/
	int sort_mode;			/* 0: lexicographical order
							* 1: smallest entries first
							* 2: oldest entries first
							* 3: largest file first
							* 4: sorted by time ---->> subselected show_time_mode
							*/

	/* access mode */
	int show_rec;			/* whether recursively search * dir */
	int show_subdir;		/* whether search subdirectories */

	/* print mode */
	int sort_switch;		/* -f: sort switch */
	int print_long;			/* show as a list */
	int show_symbol;		/* -F: whether show special symbol after file or directory */
	int show_slink;			/* -d: show symbolic links in the argument list are not indirected through */
	int show_inode;			/* show inode number */
	int show_size_format;	/* -h: causing the sizes to be reported in bytes displayed in a human readable format */
	int show_size_kbytes;	/* -k: causing the sizes to be reported in kilobytes */
	/* Modifies the -s and -l
	* need -s and -l
	*/
	int name_id;			/* a switch: show owner group name:0 or ID:1 */
	int reverse_switch;		/* whether reverse the output */
	int non_printable;		/* terminal -> '?'  not terminal -> force raw printing */
	int show_block;			/* -s: show the number of file system blocks */

};
struct file_time
{
	char abb_month[4];
	char day[3];
	int hour;
	int minute;
};
struct long_format
{
	int file_mode;		/* file mode */
	int number_link;	/* number of links */
	char *owner;		/* owner name */
	char *group;		/* group name */
	int number_bytes;	/* number of bytes in the file */
	struct file_time time_modify;
	struct file_time time_access;
	struct file_time time_change;
	char *pathname;		/* pathname */
};

#endif