/*
   CITS2002 Project 2 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
 */
#include "dirVect.h"

#define VECTOR_INIT_SIZE 4

/* Initialises DIR_V vector used to store parsed file data
*/
DIR_V init_dir_vector()
{
	#if (EVAL)
		printf("—————————————————————————————\n");
		printf("  DIRECTORY VECTOR  \n");
		printf("—————————————————————————————\n");
	#endif
	
	FILE_INFO *temp;
	DIR_V dV;
	dV.init	 	 = true;
	dV.curr		 = 0;
	dV.max		 =	VECTOR_INIT_SIZE;
	temp		 =	malloc( sizeof (FILE_INFO) * dV.max);
	CHECK_ALLOC(temp, "dirVect")
	dV.file = temp;
	
	return dV;
}

/*  Uses dV->curr and dV-> max
	To determine if there is enough memory allocated
	If not double the current DIR_V vector size
 */
static void examineSize(DIR_V *dV)
{
	FILE_INFO *temp;
	if (dV->curr >= dV->max) {
		dV->max *= 2;
		temp	 =	realloc(dV->file, sizeof (FILE_INFO) * dV->max);
		CHECK_ALLOC(temp, "dirVect")
		dV->file = temp;
		#if (EVAL)
			printf("*");
		#endif
	}
}

void append_dir_vector(DIR_V *dV, char fileName[], struct stat fileStat)
{
	char *temp;
	examineSize(dV);
	int i = dV->curr++;
	temp = malloc( sizeof (char) * strlen(fileName)+1);
	CHECK_ALLOC(temp, "dirVect")
	dV->file[i].fName = temp;
	
	dV->file[i].fName = strcpy(dV->file[i].fName, fileName);
	dV->file[i].fStat = fileStat;
	#if (EVAL)
		printf("%i\t %s\n", i, fileName);
	#endif
}

static int cmpName (const void *a, const void *b)
{
	const FILE_INFO *ia = (const FILE_INFO *)a;
	const FILE_INFO *ib = (const FILE_INFO *)b;	
	
	return strcmp(ia->fName, ib->fName);
}

static int cmpNameR (const void *a, const void *b) 
{
	
	const FILE_INFO *ia = (const FILE_INFO *)a;
	const FILE_INFO *ib = (const FILE_INFO *)b;	
	
	return strcmp(ib->fName, ia->fName);
}

static int cmpTime (const void *a, const void *b) 
{
	const FILE_INFO *ia = (const FILE_INFO *)a;
	const FILE_INFO *ib = (const FILE_INFO *)b;	
	return difftime(ia->fStat.st_mtime, ib->fStat.st_mtime);
}

static int cmpTimeR (const void *a, const void *b) 
{
	const FILE_INFO *ia = (const FILE_INFO *)a;
	const FILE_INFO *ib = (const FILE_INFO *)b;
	
	return difftime(ib->fStat.st_mtime, ia->fStat.st_mtime);
}

static int cmpSize (const void *a, const void *b) 
{
	const FILE_INFO *ia = (const FILE_INFO *)a;
	const FILE_INFO *ib = (const FILE_INFO *)b;
	
	return ia->fStat.st_size > ib->fStat.st_size;
}

static int cmpSizeR (const void *a, const void *b) 
{
	const FILE_INFO *ia = (const FILE_INFO *)a;
	const FILE_INFO *ib = (const FILE_INFO *)b;
	
	return ib->fStat.st_size > ia->fStat.st_size;
}

void sort_dir_vector(DIR_V *dV, enum sortType sortBy)
{
	switch (sortBy) {
		case NAME: { qsort(dV->file, dV->curr, sizeof (FILE_INFO), cmpName); break; }
		case R_NAME: { qsort(dV->file,dV->curr, sizeof (FILE_INFO), cmpNameR); break; }
		case TIME: { qsort(dV->file,dV->curr, sizeof (FILE_INFO), cmpTime); break; }
		case R_TIME: { qsort(dV->file,dV->curr, sizeof (FILE_INFO), cmpTimeR); break; }
		case SIZE: { qsort(dV->file,dV->curr, sizeof (FILE_INFO), cmpSize); break; }
		case R_SIZE: { qsort(dV->file,dV->curr, sizeof (FILE_INFO), cmpSizeR); break; }
		default: { abort(); }
	}
}
