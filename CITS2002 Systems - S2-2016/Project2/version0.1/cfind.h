#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "./lib/statexpr.h"
//#include "/cslinux/adhoc/include/statexpr.h"

#include <sys/types.h>
#include <dirent.h>

#define EVAL false

typedef void * PROG_DATA;


extern int initOpt(int, char **, bool *, int *);
extern int initArg(int, char **, int, char *, char *);
extern int listDir(DIR *);