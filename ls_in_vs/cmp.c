/* This is the definition of comparation function */


#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "ls.h"
#include "print.h"

#define LTHAN 1
#define EQUAL 0
#define STHAN -1


/* cmpstringp: 
 * sort after getting the list of entries
 * this function sorts the list of entries and put non-directory ahead,
 * then sort each part in lexicographical order
 */

int cmp_pathlist(const FTSENT *p1, const FTSENT *p2)
{
	return strcmp(p1->fts_name, p2->fts_name);
}
/* separate the input entries, just put non-directory ahead */

/* simply compare in lexicographical order */
int cmp_LEXORD(const FTSENT *p1, const FTSENT *p2)
{
	return strcmp(p1->fts_name, p2->fts_name);
}

/* large file first */
int cmp_LFIRST(const FTSENT *p1, const FTSENT *p2)
{
	struct stat *st1;
	struct stat *st2;
	if (p1->fts_info == FTS_ERR || p2->fts_info == FTS_ERR)
		return cmp_LEXORD(p1, p1);
	if (p1->fts_info == FTS_NS || p2->fts_info == FTS_NS)
		return cmp_LEXORD(p1, p1);
	st1 = p1->fts_statp;
	st2 = p2->fts_statp;
	return st1->st_size > st2->st_size ? STHAN : (st1->st_size < st2->st_size ? LTHAN : cmp_LEXORD(p1, p1));
}

/* sort with access time */
int cmp_ATIME(const FTSENT *p1, const FTSENT *p2)
{
	struct stat *st1;
	struct stat *st2;
	if (p1->fts_info == FTS_ERR || p2->fts_info == FTS_ERR)
		return cmp_LEXORD(p1, p1);
	if (p1->fts_info == FTS_NS || p2->fts_info == FTS_NS)
		return cmp_LEXORD(p1, p1);
	st1 = p1->fts_statp;
	st2 = p2->fts_statp;
	return st1->st_atime > st2->st_atime ? STHAN : (st1->st_atime < st2->st_atime ? LTHAN : cmp_LEXORD(p1, p1));
}

/* sort with modified time */
int cmp_MTIME(const FTSENT *p1, const FTSENT *p2)
{
	struct stat *st1;
	struct stat *st2;
	if (p1->fts_info == FTS_ERR || p2->fts_info == FTS_ERR)
		return cmp_LEXORD(p1, p1);
	if (p1->fts_info == FTS_NS || p2->fts_info == FTS_NS)
		return cmp_LEXORD(p1, p1);
	st1 = p1->fts_statp;
	st2 = p2->fts_statp;
	return st1->st_mtime > st2->st_mtime ? STHAN : (st1->st_mtime < st2->st_mtime ? LTHAN : cmp_LEXORD(p1, p1));
}

/* sort with changed time */
int cmp_CTIME(const FTSENT *p1, const FTSENT *p2)
{
	struct stat *st1;
	struct stat *st2;
	if (p1->fts_info == FTS_ERR || p2->fts_info == FTS_ERR)
		return cmp_LEXORD(p1, p1);
	if (p1->fts_info == FTS_NS || p2->fts_info == FTS_NS)
		return cmp_LEXORD(p1, p1);
	st1 = p1->fts_statp;
	st2 = p2->fts_statp;
	return st1->st_ctime > st2->st_ctime ? STHAN : (st1->st_ctime < st2->st_ctime ? LTHAN : cmp_LEXORD(p1, p1));
}
