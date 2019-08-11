#include "cfind.h"
#include "sort.h"
#include "exec.h"
#include "dirList.h"

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

int sortList(DIR_LIST *dL, enum sortType sortBy)
{
	switch (sortBy) {
		case NAME: { qsort(dL->file, dL->curr, sizeof (FILE_INFO), cmpName); break; }
		case R_NAME: { qsort(dL->file,dL->curr, sizeof (FILE_INFO), cmpNameR); break; }
		case TIME: { qsort(dL->file,dL->curr, sizeof (FILE_INFO), cmpTime); break; }
		case R_TIME: { qsort(dL->file,dL->curr, sizeof (FILE_INFO), cmpTimeR); break; }
		case SIZE: { qsort(dL->file,dL->curr, sizeof (FILE_INFO), cmpSize); break; }
		case R_SIZE: { qsort(dL->file,dL->curr, sizeof (FILE_INFO), cmpSizeR); break; }
		default: { return -1; }
	}
	return 0;
}

