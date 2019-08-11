#ifndef _CFIND_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>

#define EVAL false

typedef struct{
	char *fName;
	struct stat fStat;
} FILE_INFO;

typedef struct{
	bool init;
	int curr;
	int max;
	FILE_INFO *file;
} DIR_LIST;

enum sortType {
	NAME,
	TIME,
	SIZE,
	R_NAME,
	R_TIME,
	R_SIZE
};

#endif
