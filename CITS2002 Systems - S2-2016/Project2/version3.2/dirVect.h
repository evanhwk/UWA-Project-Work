#ifndef _CFIND_H
	#define _CFIND_H
	#include "cfind.h"
#endif
#include <sys/stat.h>											// struct stat
#include <string.h>											// str functions
#include <time.h>											// time functions

enum sortType {
	NAME,												// Sort By Name
	TIME,												//     	   Time
	SIZE,												//	   Size
};

typedef struct{
	int fDepth;											// directory depth from the input directory
	char *fName;											// name of file
	struct stat fStat;										// fileStat 
} FILE_INFO;

typedef struct{
	bool init;											// has been init? T/F
	int curr;											// current index/size
	int max;											// max: allocated memory
	FILE_INFO *file;										// FILE_INFO Array
} DIR_V;

extern DIR_V init_dir_vector();
extern void append_dir_vector(DIR_V *dV, char filePath[], struct stat fileStat, int depth);
extern void sort_dir_vector(DIR_V *dV, enum sortType sortBy);
