#include <string.h>
#include <ctype.h>
#include <getopt.h>

extern int initOpt(int argc, char **argv, bool fl[], int *depth);
extern int initArg(int argc, char **argv, int notOpt, char **dirPath, char **statExpress);