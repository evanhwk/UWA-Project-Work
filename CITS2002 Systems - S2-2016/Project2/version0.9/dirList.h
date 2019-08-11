#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <sys/stat.h>

#define EVAL true	// THIS SHOULD BE CHANGED ON SUBMISSION

//typedef void *         DIR_LIST;

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

extern DIR_LIST initDirList();
extern void appendDirList(DIR_LIST * dirList, char nameBuff[], struct stat statBuff);