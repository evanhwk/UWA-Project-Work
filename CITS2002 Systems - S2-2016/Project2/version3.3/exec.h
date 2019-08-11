#ifndef _CFIND_H
	#define _CFIND_H
	#include "cfind.h"
#endif
#include <dirent.h>													// stat
#include <statexpr.h>													// STAT_EXPRESSION
#include <unistd.h>													// unlink
#include <errno.h>													// error codes
#include <pwd.h>													// struct passwd
#include <grp.h>													// struct group

extern void parse_flags(bool fl[]);
int parse_dir(STAT_EXPRESSION statExpress, char dirPath[], int depth, int depthLimit);
void sort_print();
