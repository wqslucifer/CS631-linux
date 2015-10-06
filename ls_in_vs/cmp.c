/* This is the definition of comparation function */


#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "cmp.h"

#define LTHAN 1
#define EQUAL 0
#define STHAN -1


/* cmpstringp: 
 * sort after getting the list of entries
 * this function sorts the list of entries and put non-directory ahead,
 * then sort each part in lexicographical order
 */
int cmp_pathlist(const void *p1, const void *p2)
{
	struct stat st1;
	struct stat st2;
	if (lstat(*(char * const *)p1, &st1) < 0)
	{
		fprintf(stderr, "cannot get stat of %s: %s", (char *)p1, strerror(errno));
	}
	if (lstat(*(char * const *)p2, &st2) < 0)
	{
		fprintf(stderr, "cannot get stat of %s: %s", (char *)p2, strerror(errno));
	}
	if (S_ISDIR(st1.st_mode))
	{
		if (S_ISDIR(st2.st_mode))
		{
			return strcmp(*(char * const *)p1, *(char * const *)p2);
		}
		else // non-directory
		{
			return LTHAN;
		}
	}
	else  //non-directory
	{
		if (S_ISDIR(st2.st_mode))
		{
			return STHAN;
		}
		else //non-dirctory
		{
			return strcmp(*(char * const *)p1, *(char * const *)p2);
		}
	}
}
/* separate the input entries, just put non-directory ahead */
int nsort_pathlist(const void *p1, const void *p2)
{
	struct stat st1;
	struct stat st2;
	if (lstat(*(char * const *)p1, &st1) < 0)
	{
		fprintf(stderr, "cannot get stat of %s: %s", (char *)p1, strerror(errno));
	}
	if (lstat(*(char * const *)p2, &st2) < 0)
	{
		fprintf(stderr, "cannot get stat of %s: %s", (char *)p2, strerror(errno));
	}
	if (S_ISDIR(st1.st_mode))
	{
		if (S_ISDIR(st2.st_mode))
		{
			return EQUAL;
		}
		else // non-directory
		{
			return LTHAN;
		}
	}
	else  //non-directory
	{
		if (S_ISDIR(st2.st_mode))
		{
			return STHAN;
		}
		else //non-dirctory
		{
			return EQUAL;
		}
	}
}

/* simply compare in lexicographical order */
int cmp_LEXORD(const void *p1, const void *p2)
{
	return strcmp(*(char * const *)p1, *(char * const *)p2);
}

/* large file first */
int cmp_LFIRST(const void *p1, const void *p2)
{
	struct stat st1;
	struct stat st2;
	if (lstat(*(char * const *)p1, &st1) < 0)
	{
		fprintf(stderr, "cannot get stat of %s: %s", (char *)p1, strerror(errno));
	}
	if (lstat(*(char * const *)p2, &st2) < 0)
	{
		fprintf(stderr, "cannot get stat of %s: %s", (char *)p2, strerror(errno));
	}
	return st1.st_size > st1.st_size ? STHAN : (st1.st_size < st2.st_size ? LTHAN : ENOENT);
}