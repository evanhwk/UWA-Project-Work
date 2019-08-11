#ifndef _CFIND_H
#define _CFIND_H
	#include "cfind.h"
#endif
#include <string.h>
#include <ctype.h>
#include <getopt.h>

extern int init_opt(int argc, char **argv, bool fl[], int *depth);
extern void init_arg(int argc, char **argv, int notOpt, char **dirPath, char **statExpress);
