/*
   CITS2002 Project 2 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
 */
#include "dirVect.h"

#define VECTOR_INIT_SIZE 4

/* Initialises a DIR_V vector used to store parsed file data and returns it
*/
DIR_V init_dir_vector()
{
	FILE_INFO *temp;
	DIR_V dV;
	dV.init	 	 = true;
	dV.curr		 = 0;
	dV.max		 = VECTOR_INIT_SIZE;
	temp		 = malloc( sizeof (FILE_INFO) * dV.max);
	CHECK_ALLOC(temp, "dirVect")									
	dV.file = temp;
	
	return dV;
}

/*  Validates the current state of the vector referenced by *dV
	Uses dV->curr and dV-> max
	To determine if there is enough memory allocated
	If not double the current DIR_V vector size
 */
static void validateSize(DIR_V *dV)
{
	FILE_INFO *temp;
	if (dV->curr >= dV->max) {
		dV->max	*= 2;
		temp	 = realloc(dV->file, sizeof (FILE_INFO) * dV->max);
		CHECK_ALLOC(temp, "dirVect")
		dV->file = temp;
	}
}
/*  Appends a new item to the end of the vector referneced by *dV
*/
void append_dir_vector(DIR_V *dV, char fileName[], struct stat fileStat, int depth)
{
	char *temp;
	validateSize(dV);
	int i = dV->curr++;
	temp = malloc( sizeof (char) * strlen(fileName)+1);
	CHECK_ALLOC(temp, "dirVect")
	dV->file[i].fName	= temp;

	dV->file[i].fName	= strcpy(dV->file[i].fName, fileName);
	dV->file[i].fStat	= fileStat;
	dV->file[i].fDepth	= depth;
}

/* Function for sorting by name
*/
static int cmpName (const void *a, const void *b)
{
	const FILE_INFO *ia	= (const FILE_INFO *)a;
	const FILE_INFO *ib	= (const FILE_INFO *)b;	
	
	return strcmp(ia->fName, ib->fName);
}

/* Function for sorting by name in reverse
*/
static int cmpNameR (const void *a, const void *b) 
{
	
	const FILE_INFO *ia	= (const FILE_INFO *)a;
	const FILE_INFO *ib	= (const FILE_INFO *)b;	
	
	return strcmp(ib->fName, ia->fName);
}

/* Function for sorting by time
*/
static int cmpTime (const void *a, const void *b) 
{
	const FILE_INFO *ia	= (const FILE_INFO *)a;
	const FILE_INFO *ib	= (const FILE_INFO *)b;	
	return (int) difftime(ia->fStat.st_mtime, ib->fStat.st_mtime);
}

/* Function for sorting by time in reverse
*/
static int cmpTimeR (const void *a, const void *b) 
{
	const FILE_INFO *ia	= (const FILE_INFO *)a;
	const FILE_INFO *ib	= (const FILE_INFO *)b;
	
	return (int) difftime(ib->fStat.st_mtime, ia->fStat.st_mtime);
}

/* Function for sorting by size
*/
static int cmpSize (const void *a, const void *b) 
{
	const FILE_INFO *ia	= (const FILE_INFO *)a;
	const FILE_INFO *ib	= (const FILE_INFO *)b;
	if (ia->fStat.st_size > ib->fStat.st_size){
		return 1;
	} 
	else if (ia->fStat.st_size == ib->fStat.st_size) {
		return 0;
	}
	else {
		return -1;
	}
}

/* Function for sorting by size in reverse
*/
static int cmpSizeR (const void *a, const void *b) 
{
	const FILE_INFO *ia	= (const FILE_INFO *)a;
	const FILE_INFO *ib	= (const FILE_INFO *)b;
	
	if (ib->fStat.st_size > ia->fStat.st_size){
		return 1;
	} 
	else if (ib->fStat.st_size == ia->fStat.st_size) {
		return 0;
	}
	else {
		return -1;
	}
}

/* Parses enum sortType in order to determine how the referenced DIR_V should be sorted
*/
void sort_dir_vector(DIR_V *dV, enum sortType sortBy)
{
	switch (sortBy) {
		case NAME: { qsort(dV->file, dV->curr, sizeof (FILE_INFO), cmpName); break; }				// sort by Name
		case R_NAME: { qsort(dV->file,dV->curr, sizeof (FILE_INFO), cmpNameR); break; }				// reverse sort by Name
		case TIME: { qsort(dV->file,dV->curr, sizeof (FILE_INFO), cmpTime); break; }				// sort by Time
		case R_TIME: { qsort(dV->file,dV->curr, sizeof (FILE_INFO), cmpTimeR); break; }				// reverse sort by Time
		case SIZE: { qsort(dV->file,dV->curr, sizeof (FILE_INFO), cmpSize); break; }				// sort by Size
		case R_SIZE: { qsort(dV->file,dV->curr, sizeof (FILE_INFO), cmpSizeR); break; }				// reverse sort by Size
		default: { abort(); }
	}
}
