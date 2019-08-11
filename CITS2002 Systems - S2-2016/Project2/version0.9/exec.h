#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <dirent.h>
#include <statexpr.h>

#define EVAL true	// THIS SHOULD BE CHANGED ON SUBMISSION

extern void parseFlags(bool fl[]);
extern void dirToList(char dirPath[], STAT_EXPRESSION statExpress, int depth);
extern void printList();