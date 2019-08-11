#include "exec.h"
#include "dirList.h"

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
	
	if (SORT_SIZE_FLAG && SORT_TIME_FLAG) {
		SORT_SIZE_FLAG = false;																/* If both -t and -s, prioritise -t  */
	}
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
/* Takes Stat Express and SHOW_DOT_FLAG and determines if the current file should be referenced by the dir list*/
static bool ignoreCheck(char const *name, const STAT_EXPRESSION SE, struct stat *stat_buff)
{
  bool ignore = false;
  if ( ( SHOW_DOT_FLAG == false && name[0] == '.')) {
	  ignore = true;
  }
  else if ( evaluate_stat_expression(SE, name, stat_buff) == false) {
	  ignore = true;
  }
  return ignore;
}
/* Get the full path of the current file*/
static char * getFullPath(char dirPath[], char fileName[])
{
	char *temp = malloc( sizeof (char) * MAXPATHLEN);
	char *fullpath;
	if (temp != NULL) { 
		fullpath = temp;
	} 
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

void dirToList(char dirPath[], STAT_EXPRESSION statExpress, int depth)
{
	/* init */
	dirPath = strcat(dirPath, "/");								/* pathname of directory to be opened */
	DIR *dir = opendir(dirPath);								/* OPEN DIR */
	
	char *temp = malloc( sizeof (char) * MAXPATHLEN);
	char *fullpath;												/* fullpath of a file - dirPath + file name */
	if (temp != NULL) { 
		fullpath = temp;
	} 
	else {
		perror("exec");
		exit(EXIT_FAILURE);
	}
	
	if (!DIRECTORY_LIST.init)									/* If init flag has not been tripped init the dir list */
		DIRECTORY_LIST = initDirList();
	
	
	struct dirent *dp;
	/* exec */
	if (dir == NULL) {											/* If dir is NULL return error with perror - i.e. directory not found */
		perror("exec");
		exit(EXIT_FAILURE);
	}
	else {
		while ( (dp = readdir(dir)) != NULL ) {					/* WHILE dirent pointer dp has not reached the end of dir */
			struct stat statBuff, *stat_Ptr = &statBuff;		/* Declare stat type + pointer to collect stat information about current file */
			fullpath = getFullPath(dirPath, dp->d_name);		/* CALL getFullPath() */
			
			if (stat(fullpath, stat_Ptr) != 0) 					/* CALL stat, return ERROR if returned */
			{
				perror("exec");
				exit(EXIT_FAILURE);
			}
			else if ( S_ISDIR( stat_Ptr->st_mode ) ) 			/* if stat pointer references a DIRECTORY */
			{
				if ( strcmp(dp->d_name,".") && strcmp(dp->d_name,".."))						/* Ignore . and .. */
				{
					if ( ignoreCheck(dp->d_name, statExpress, stat_Ptr) == false)	/* CALL ignoreCheck */
					{
						appendDirList(D_L, fullpath, statBuff);						/* CALL appendDirList function to add this item to DIRECTORY_LIST IF it matches the Stat Expression */
					}
					if (depth != 0) {									/* If depth is non-zero */
						#if (EVAL)
							printf("<");
						#endif
						dirToList(fullpath, statExpress, depth - 1); 	/* Call this function recursively, with depth - 1 */
						free(fullpath);
					}
				}
			}
			else 
			{
				if ( ignoreCheck(dp->d_name, statExpress, stat_Ptr) == false)	/* CALL ignoreCheck */
				{
					appendDirList(D_L, fullpath, statBuff);
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

void printList()
{
	if ( COUNT_ONLY_FLAG == true ) {
		printf("%i\n", DIRECTORY_LIST.curr);
	}
	else
	{
		
	}
}