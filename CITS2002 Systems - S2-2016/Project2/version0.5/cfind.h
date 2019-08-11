#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "./lib/statexpr.h"				// THIS SHOULD BE CHANGED ON SUBMISSION
//#include "/cslinux/adhoc/include/statexpr.h"


#define EVAL true

extern int initOpt(int argc, char **argv, bool fl[], int *depth);
extern int initArg(int argc, char **argv, int notOpt, char **dirPath, char **statExpress);
extern void parseFlags(bool fl[]);
extern void dirToList(char *dirPath, char *stringSE);