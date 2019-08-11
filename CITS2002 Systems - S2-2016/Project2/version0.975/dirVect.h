#ifndef _CFIND_H
#define _CFIND_H
	#include "cfind.h"
#endif
#include <sys/stat.h>												// for struct stat
#include <string.h>													// for str functions
#include <time.h>

//typedef void *         DIR_V;

#ifdef __linux__										/* for local use */
struct  timespec {
        time_t  tv_sec ;
        long  tv_nsec ;
};
#endif

enum sortType {
	NAME,
	TIME,
	SIZE,
	R_NAME,
	R_TIME,
	R_SIZE
};

typedef struct{
	char *fName;
	struct stat fStat;
} FILE_INFO;

typedef struct{
	bool init;
	int curr;
	int max;
	FILE_INFO *file;
} DIR_V;

extern DIR_V init_dir_vector();
extern void append_dir_vector(DIR_V *dV, char filePath[], struct stat fileStat);
extern void sort_dir_vector(DIR_V *dV, enum sortType sortBy);
