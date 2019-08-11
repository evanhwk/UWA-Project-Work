/*
   CITS2002 Project 2 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
 */
#include "dirVect.h"
#include "exec.h"

static bool fl_SHOW_HIDDEN, fl_COUNT_ONLY, fl_LONG_LIST, fl_REVERSE_SORT, fl_SORT_SIZE, fl_SORT_TIME, fl_UNLINK = false; /* see parseFlags */
static DIR_V DIRECTORY_VECTOR, *D_V = &DIRECTORY_VECTOR; 						/* Dynamic Vector which holds references to file names and data */

/* 
	Initialises File Scope Variables for various options (to be used with exec functions)
	Uses boolean array passed from the main function
*/
void parse_flags(bool fl[])
{
	fl_SHOW_HIDDEN = fl[0];
	fl_COUNT_ONLY = fl[1];
	fl_LONG_LIST = fl[2];
	fl_REVERSE_SORT = fl[3];
	fl_SORT_SIZE = fl[4];
	fl_SORT_TIME = fl[5];
	fl_UNLINK = fl[6];
	
	#if (EVAL)
		printf("_______________________________________________________________________________________________________________\n\n");
		printf("+———————————+\n");
		printf("|   FLAGS   |\n");
		printf("+———————+———+\n");
		printf("| Hiddn\t| %d |\n", fl_SHOW_HIDDEN); 
		printf("| Unlnk\t| %d |\n", fl_UNLINK);
		printf("| Lcnt\t| %d |\n", fl_COUNT_ONLY); 
		printf("| Llist\t| %d |\n", fl_LONG_LIST); 
		printf("| Srev\t| %d |\n", fl_REVERSE_SORT); 
		printf("| Stime\t| %d |\n", fl_SORT_TIME);
		printf("| Ssize\t| %d |\n", fl_SORT_SIZE); 
		printf("+———————+———+\n");
	#endif
}

/* 
	Takes STAT_EXPRESSION and fl_SHOW_HIDDEN and determines 
	if the current file should be referenced by the directory vector
*/
static bool examineData(char const *name, const STAT_EXPRESSION SE, struct stat *stat_buff)
{
	bool useData = true;
	/* If show hidden is false and this is a hidden item OR evaluate_stat_expression returns false for this item */
	if ( ( fl_SHOW_HIDDEN == false && name[0] == '.' ) || (evaluate_stat_expression(SE, name, stat_buff) == false) )
		useData = false;
	return useData;
}

/*  
	Get the full path of the current file 
	Return this as a *char
*/
static char * getFullPath(char dirPath[], char fileName[])
{
	char *temp = malloc( sizeof (char) * (strlen(dirPath)+1 + strlen(fileName)+1));		/* This memory has to be freed at the callers discretion */
	char *fullpath;
	CHECK_ALLOC(temp, "exec")
	fullpath = temp;
	if ( strcmp(dirPath, "./") != 0)						/* Following behaviour of cfind-sample solution - If command line input is . do not show directory path */
		fullpath = strcpy (fullpath, dirPath);
	else
		fullpath = strcpy (fullpath, "");
	fullpath = strcat(fullpath, fileName);
	return fullpath;
}

/* 
	Parses the directory given and generates DIR_V DIRECTORY_VECTOR 
	Can be called as a recursive function (for child directories)
	End result will be an unsorted DIR_V 
*/
void dir_to_vector(char dirPath[], STAT_EXPRESSION statExpress, int depth)
{
	/* init */
	if (!D_V->init && !fl_UNLINK) {								/* !!!  IF NOT UNLINKING, init DIRECTORY_VECTOR (unless already init)*/
		DIRECTORY_VECTOR = init_dir_vector();
	}
	dirPath = strcat(dirPath, "/");								/* Pathname of directory to be opened */
	char *fullpath;												/* FULL pathname of a file - dirPath + file name */
	
	/* exec */
	DIR *dir = opendir(dirPath);								/* OPEN DIR */
	struct dirent *dp;
	
	if (dir == NULL) { 											/* If dir is NULL return error with perror - i.e. directory not found */
		P_ERROR_EXIT("exec")	
	}					
	else {
		while ( (dp = readdir(dir)) != NULL ) 					/* WHILE dirent pointer dp has not reached the end of dir */
		{					
			struct stat stat_Buff;											/* Declare stat type + pointer to collect stat information about current file */
			struct stat *stat_Ptr = &stat_Buff;				
			fullpath = getFullPath(dirPath, dp->d_name);					/* CALL getFullPath() */
			
			if (stat(fullpath, stat_Ptr) != 0) { 									/* CALL stat, return ERROR if returned */
				P_ERROR_EXIT("exec")												
			}
			else if (S_ISDIR( stat_Ptr->st_mode ) ) 								/* If stat pointer references a DIRECTORY */
			{									
				if ( strcmp(dp->d_name,".") && strcmp(dp->d_name,"..") )					/* (Ignoring . and .. directories) - VERY IMPORTANT */
				{
					if (examineData(dp->d_name, statExpress, stat_Ptr) == true) 			/* CALL examineData to apply stat expression and option flags */
					{
						if (!fl_UNLINK) 																/* IF NOT UNLINKING */
						{
							append_dir_vector(D_V, fullpath, stat_Buff);									/* Append validated data to the DIR_V */
						}
						if (depth != 0) 																/* If depth is non-zero - RECURSIVE CHECK */
						{																					
							#if (EVAL)
								printf("<");
							#endif
							dir_to_vector(fullpath, statExpress, depth - 1); 								/* Call this function recursively, with depth - 1 */
							free(fullpath);
						}
						if (fl_UNLINK)
						{
							//if (rmdir(fullpath)!=0) {
								//P_ERROR_EXIT("exec")
							//}
						}
					}
				}
			}
			else 																	/* If stat pointer does NOT reference a directory */
			{																
				if (examineData(dp->d_name, statExpress, stat_Ptr) == true)							/* CALL examineData to apply stat expression and ignore flags */
				{					
					if (!fl_UNLINK) 																/* IF NOT UNLINKING */
					{
						append_dir_vector(D_V, fullpath, stat_Buff);											/* Append data to the DIR_V, given stat expression*/
					} 
					else if (fl_UNLINK)
					{
						//if (unlink(fullpath)!=0) {
						//	P_ERROR_EXIT("exec")
						//}
					}
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

/* 
	Execute Printing (dependant on option flags) to the command line
	Rules in Place: 
	* -u | No Printing + No Sorting
	* -c | Only print size of DIRECTORY_VECTOR to command line + No sorting
	* -t | Sorting by -t takes priority over -s
*/
void exec_print()
{
	if (fl_UNLINK) {
	}
	else if (fl_COUNT_ONLY) {
			printf("%i\n", DIRECTORY_VECTOR.curr);
	}
	else
	{
		if (fl_REVERSE_SORT) {
			if (fl_SORT_TIME) {
				sort_dir_vector(D_V, R_TIME);
			}
			else if (fl_SORT_SIZE) {
				sort_dir_vector(D_V, R_SIZE);
			}
			else {
				sort_dir_vector(D_V, R_NAME);
			}
		}
		else {
			if (fl_SORT_TIME) {
				sort_dir_vector(D_V, TIME);
			}
			else if (fl_SORT_SIZE) {
				sort_dir_vector(D_V, SIZE);
			}
			else {
				sort_dir_vector(D_V, NAME);
			}
		}
		
		if (fl_LONG_LIST) {
			puts("this sucks");
		}
		else {
			for (int i = 0; i < D_V->curr; i++ ){
				puts(D_V->file[i].fName);
			}
		}
	}
}
