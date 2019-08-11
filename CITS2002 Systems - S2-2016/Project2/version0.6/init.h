#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>

#define EVAL true	// THIS SHOULD BE CHANGED ON SUBMISSION

extern int initOpt(int argc, char **argv, bool fl[], int *depth);
extern int initArg(int argc, char **argv, int notOpt, char **dirPath, char **statExpress);
