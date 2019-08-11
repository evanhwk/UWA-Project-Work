#include "cfind.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <dirent.h>

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

static bool SHOW_DOT_FLAG, COUNT_ONLY_FLAG, LONG_LIST_FLAG, REVERSE_SORT_FLAG, SORT_SIZE_FLAG, SORT_TIME_FLAG, UNLINK_FLAG = false;

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
		SORT_SIZE_FLAG = false;
	}
	#if ( EVAL )
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

static void initDirList(DLIST * L)
//static void initDirList(FILE_INFO ** L)
{
	L->size = 0;
	L->max = LIST_INIT_SIZE;
	L->fileNames = malloc( sizeof (char **) * LIST_INIT_SIZE);
	L->fileData = malloc( sizeof (struct stat *) * LIST_INIT_SIZE);
}

// THIS IS NOT YET 100% IMPLEMENTED PENDING DIRECTION CHANGE 
static void processDirList(DLIST * L, char *nameBuff, struct stat statBuffer)
{
	char **tempN;
	struct stat *tempD;
	
	L->size++;
	//printf("SIZE: %i, MAX: %i\n", L->size, L->max);
	//printf("> SIZE OF DIR LIST: %lu\n", sizeof  (char **));
	if ( L->size > L->max) {
		L->max *= 2;
		tempN = realloc(L->fileNames, sizeof (char **) * L->max);
		//printf("%i\n", (int) ( sizeof (char **) * L->max / sizeof (char **) ) );
		tempD = realloc(L->fileData, sizeof (struct stat *) * L->max);
		if (tempN != NULL && tempD != NULL) {
			L->fileNames = tempN;
			L->fileData = tempD;
		}
	}
	L->fileNames[L->size] = nameBuff;
	L->fileData[L->size] = statBuffer;
	#if (EVAL == true)
		printf(" %i\t| %s\n", L->size, L->fileNames[L->size]);
	#endif
}

void dirToList(char *dirPath, char *stringSE)
{
	DIR *dir = opendir(dirPath);
	STAT_EXPRESSION statExpress = compile_stat_expression(stringSE); 
	
	#if (EVAL == true)
		printf ("> Directory Path is %s\n", dirPath);
		printf ("> Stat Expression is %s\n", stringSE);
	#endif
	
	DLIST dirL, *dirL_Ptr = &dirL;
	//FILE_INFO dirL, *dirL_Ptr = &dirL;
	struct dirent *dp;
	char fullpath[MAXPATHLEN];

	if (dir != NULL) {		
		initDirList(dirL_Ptr);
		#if ( EVAL )
		printf("—————————————————————————————\n");
		printf("  DIRECTORY LIST VECTOR  \n");
		printf("—————————————————————————————\n");
		#endif
		while ( (dp = readdir(dir)) ) {
			struct stat statBuff, *stat_Ptr = &statBuff;
			sprintf(fullpath, "%s%s", dirPath, dp->d_name);
			if (stat(fullpath, stat_Ptr) != 0)
			{
				perror(NULL);
			}
			else if ( ignoreCheck(dp->d_name, statExpress, stat_Ptr) == false)
			{
				processDirList(dirL_Ptr, dp->d_name, statBuff);
			}
		}
		closedir (dir);
		printf("—————————————————————————————\n");
	}
	else {
		perror("ERROR");
		exit(EXIT_FAILURE);
	}
	
	if (statExpress != NULL)
	{
		printf("> SUCCESS\n");
	}
}

int execute(char *dirPath, char *stringSE, bool *fl, int depth)
{
	//dirToList(dirPath, stringSE);
	//if (statExpress != NULL && list != NULL)
	//{
	//	printf("> SUCCESS\n");
	//}
	return 0;
}
