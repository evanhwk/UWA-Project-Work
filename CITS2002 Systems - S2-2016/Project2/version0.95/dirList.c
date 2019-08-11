#include "cfind.h"
#include "dirList.h"

#define LIST_INIT_SIZE 4

/* Initialises DIR_LIST used to store parsed file data
*/
DIR_LIST initDirList()
{
	#if (EVAL)
		printf("—————————————————————————————\n");
		printf("  DIRECTORY LIST VECTOR  \n");
		printf("—————————————————————————————\n");
	#endif
	
	FILE_INFO *temp;
	DIR_LIST dL;
	dL.init	 	 = true;
	dL.curr		 = 0;
	dL.max		 =	LIST_INIT_SIZE;
	temp		 =	malloc( sizeof (FILE_INFO) * dL.max);
	
	if (temp != NULL) {
			dL.file = temp;
	}
	else {
		perror("dirList");				/* most likely return - errno 12 out of memory */
		exit(EXIT_FAILURE);
	}
	return dL;
}

static void validateDirList(DIR_LIST *dL)
{
	FILE_INFO *temp;
	if (dL->curr >= dL->max) {
		dL->max *= 2;
		temp	 =	realloc(dL->file, sizeof (FILE_INFO) * dL->max);
		if (temp != NULL) {
			dL->file = temp;
			#if (EVAL)
				printf("*");
			#endif
		}
		else {
			perror("dirList");			/* most likely return - errno 12 out of memory */
			exit(EXIT_FAILURE);
		}
	}
}

void appendDirList(DIR_LIST *dL, char nameBuff[], struct stat statBuff)
{
	char *temp;
	validateDirList(dL);
	int i = dL->curr++;
	temp = malloc( sizeof (char) * strlen(nameBuff)+1);
	
	if (temp !=NULL) {
		dL->file[i].fName = temp;
	}
	else {
		perror("dirList");				/* most likely return errno 12 - out of memory */
		exit(EXIT_FAILURE);
	}
	
	dL->file[i].fName = strcpy(dL->file[i].fName, nameBuff);
	dL->file[i].fStat = statBuff;
	#if (EVAL)
		printf("%i\t| %s\n", i, nameBuff);
	#endif
}