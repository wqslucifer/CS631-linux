#define _CMP_H
/* declaration of functions */
#include "ls.h"

int cmp_pathlist(const FTSENT **p1, const FTSENT **p2);
int nsort_pathlist(const void *p1, const void *p2);
int cmp_LEXORD(const void *p1, const void *p2);
int cmp_LFIRST(const void *p1, const void *p2);