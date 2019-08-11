#ifndef _CFIND_H
#define _CFIND_H
	#include "cfind.h"
#endif
#include <dirent.h>													// stat
#include <statexpr.h>												// STAT_EXPRESSION
#include <unistd.h>													// unlink
#include <errno.h>													// error codes
#include <pwd.h>													// struct passwd
#include <grp.h>													// struct group

//#include <sys/stat.h>												
//#include <string.h>												
//#include <time.h> 												// see dirVect

extern void parse_flags(bool fl[]);
extern void dir_to_vector(char dirPath[], STAT_EXPRESSION statExpress, int depth, int limit);
int exec_sort_print();
