#include "cfind.h"
#include "exec.h"
#include "dirList.h"
#include "sort.h"

#define LIST_INIT_SIZE 4

static bool SHOW_DOT_FLAG, COUNT_ONLY_FLAG, LONG_LIST_FLAG, REVERSE_SORT_FLAG, SORT_SIZE_FLAG, SORT_TIME_FLAG, UNLINK_FLAG = false; /* see parseFlags */
static DIR_LIST DIRECTORY_LIST, *D_L = &DIRECTORY_LIST; 						/* Dynamic Vector which holds references to file names and data */

/* Initialises File Scope Variables for various options (to be used with exec functions)
Uses boolean array passed from the main function*/
void parseFlags(bool fl[])
{
	SHOW_DOT_FLAG = fl[0];
	COUNT_ONLY_FLAG = fl[1];
	LONG_LIST_FLAG = fl[2];
	
	REVERSE_SORT_FLAG = fl[3];
	SORT_SIZE_FLAG = fl[4];
	SORT_TIME_FLAG = fl[5];
	
	UNLINK_FLAG = fl[6];
	
	if (SORT_SIZE_FLAG && SORT_TIME_FLAG)
		SORT_SIZE_FLAG = false;																/* If both -t and -s, prioritise -t  */
	
	#if (EVAL)
		printf("_______________________________________________________________________________________________________________\n\n");
		printf("+———————————+\n");
		printf("|   FLAGS   |\n");
		printf("+———————+———+\n");
		printf("| Lall \t| %d |\n", SHOW_DOT_FLAG); 
		printf("| Lcnt\t| %d |\n", COUNT_ONLY_FLAG); 
		printf("| Llist\t| %d |\n", LONG_LIST_FLAG); 
		printf("| Srev\t| %d |\n", REVERSE_SORT_FLAG); 
		printf("| Ssize\t| %d |\n", SORT_SIZE_FLAG); 
		printf("| Stime\t| %d |\n", SORT_TIME_FLAG);
		printf("| Unlnk\t| %d |\n", UNLINK_FLAG);
		printf("+———————+———+\n");
	#endif
}
/* Takes Stat Express and SHOW_DOT_FLAG and determines if the current file should be referenced by the directory list*/
static bool ignoreCheck(char const *name, const STAT_EXPRESSION SE, struct stat *stat_buff)
{
  bool ignore = false;
  if ( ( SHOW_DOT_FLAG == false && name[0] == '.'))
	  ignore = true;
  else if ( evaluate_stat_expression(SE, name, stat_buff) == false)
	  ignore = true;
  return ignore;
}

/* Get the full path of the current file*/
static char * getFullPath(char dirPath[], char fileName[])
{
	char *temp = malloc( sizeof (char) * (strlen(dirPath)+1 + strlen(fileName)+1));
	char *fullpath;
	if (temp != NULL)
		fullpath = temp;
	else {
		perror("exec");
		exit(EXIT_FAILURE);
	}
	if ( strcmp(dirPath, "./") != 0)					/* Following behaviour of cfind-sample solution - If directory is . do not show directory */
		fullpath = strcpy (fullpath, dirPath);
	else
		fullpath = strcpy (fullpath, "");
	fullpath = strcat (fullpath, fileName);
	return fullpath;
}

/* Parses the directory given and generates DIR_LIST DIRECTORY_LIST 
Can be called as a recursive function (for child directories)
End result will be an unsorted DIR_LIST */
void dirToList(char dirPath[], STAT_EXPRESSION statExpress, int depth)
{
	/* init */
	if (!D_L->init)											/* !!! If init flag has not been tripped init the dir list */
		DIRECTORY_LIST = initDirList();
	
	dirPath = strcat(dirPath, "/");								/* Pathname of directory to be opened */
	char *fullpath;												/* FULL pathname of a file - dirPath + file name */
	
	/* exec */
	DIR *dir = opendir(dirPath);								/* OPEN DIR */
	struct dirent *dp;
	
	if (dir == NULL) {											/* If dir is NULL return error with perror - i.e. directory not found */
		perror("exec");
		exit(EXIT_FAILURE);
	}
	else {
		while ( (dp = readdir(dir)) != NULL ) {					/* WHILE dirent pointer dp has not reached the end of dir */
			struct stat statBuff, *stat_Ptr = &statBuff;		/* Declare stat type + pointer to collect stat information about current file */
			fullpath = getFullPath(dirPath, dp->d_name);		/* CALL getFullPath() */
			
			if (stat(fullpath, stat_Ptr) != 0) { 					/* CALL stat, return ERROR if returned */
				perror("exec");
				exit(EXIT_FAILURE);
			}
			else if (S_ISDIR( stat_Ptr->st_mode ) ) {									/* If stat pointer references a DIRECTORY */
				if ( strcmp(dp->d_name,".") && strcmp(dp->d_name,"..") )	{			/* (Ignoring . and .. directories) */
					
					if (ignoreCheck(dp->d_name, statExpress, stat_Ptr) == false) {		/* CALL ignoreCheck to apply stat expression */
						appendDirList(D_L, fullpath, statBuff);							/* Append data to the DIR_LIST, given stat expression*/
						
						if (depth != 0) {									/* If depth is non-zero */
							#if (EVAL)
								printf("<");
							#endif
							dirToList(fullpath, statExpress, depth - 1); 	/* Call this function recursively, with depth - 1 */
							free(fullpath);
						}
					}
				}
			}
			else {																/* If stat pointer does NOT reference a directory */
				if (ignoreCheck(dp->d_name, statExpress, stat_Ptr) == false) {	/* CALL ignoreCheck to apply stat expression */
					appendDirList(D_L, fullpath, statBuff);						/* Append data to the DIR_LIST, given stat expression*/
					free(fullpath);
				}
			}
		}
		closedir (dir);
	}
	#if (EVAL)
		printf(">");
	#endif
}

void execPrint()
{
	int err = 0;
	if (COUNT_ONLY_FLAG) {
		printf("%i\n", DIRECTORY_LIST.curr);
	}
	else if (REVERSE_SORT_FLAG) {
		if (SORT_SIZE_FLAG) {
			err = sortList(D_L, R_SIZE);
		}
		else if (SORT_TIME_FLAG) {
			err = sortList(D_L, R_TIME);
		}
		else {
			err = sortList(D_L, R_NAME);
		}
	}
	else {
		if (SORT_SIZE_FLAG) {
			#if (EVAL)
			printf("SORTING BY SIZE\n\n\n\n\n\n\n");
			#endif
			err = sortList(D_L, SIZE);
		}
		else if (SORT_TIME_FLAG) {
			printf("SORTING BY TIME\n\n\n\n\n\n\n");
			err = sortList(D_L, TIME);
		}
		else {
			err = sortList(D_L, NAME);
		}
	}
	if (err != 0) {
		fprintf(stderr, "error occured while sorting\n");
		exit(EXIT_FAILURE);
	}
	
	if (LONG_LIST_FLAG ) {
		puts("this sucks");
	}
	else if (!COUNT_ONLY_FLAG)
	{
		for (int i = 0; i < D_L->curr; i++ ){
			puts(D_L->file[i].fName);
		}
	}
}
