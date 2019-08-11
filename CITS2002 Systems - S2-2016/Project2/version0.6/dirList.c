#include "dirList.h"

#define LIST_INIT_SIZE 4

typedef struct{
	int size;
	int max;
	char *fileNames;
	struct stat fileData;
} FILE_INFO;

typedef struct{
	int size;
	int max;
	char **fileNames;
	struct stat *fileData;
} DLIST;
