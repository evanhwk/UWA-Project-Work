/*
   CITS2002 Project 2 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
 */
#include "dirVect.h"
#include "exec.h"
									
#define INODE_DISPLAY_L 7												// Defining display limits for long list
#define LINK_DISPLAY_L 2
#define SIZE_DISPLAY_L 8
#define TIMEDATE_L 26

static bool fl_SHOW_HIDDEN, fl_COUNT_ONLY, fl_LONG_LIST, fl_REVERSE_SORT, fl_SORT_SIZE, fl_SORT_TIME, fl_UNLINK = false; // see parseFlags
static DIR_V DIRECTORY_VECTOR, *D_V_Ptr = &DIRECTORY_VECTOR; 							// Dynamic Vector which holds references to file names and data 				*/
static int ERROR_SAV = 0;															// For memory of non-fatal errors

/* 	Initialises File Scope Variables for various options (to be used with exec functions)
	Uses boolean array passed from the main function
	Applies priority rules for certain conflicting flags
	
	Rules in Place (Higher = More Priority) - Affect:  -c -s -t -u 
	* -c > -u > -l
	* -t > -s 
*/
void parse_flags(bool fl[])
{
	fl_SHOW_HIDDEN	= fl[0];
	fl_COUNT_ONLY	= fl[1];
	fl_LONG_LIST	= fl[2];
	fl_REVERSE_SORT	= fl[3];
	fl_SORT_SIZE	= fl[4];
	fl_SORT_TIME	= fl[5];
	fl_UNLINK		= fl[6];
	
	if (fl_COUNT_ONLY) { fl_UNLINK = false; fl_LONG_LIST = false; }
	if (fl_UNLINK) { fl_LONG_LIST = false; }
	if (fl_SORT_TIME) { fl_SORT_SIZE = false; }
	
}



/*  Get the full path of the current file 
	Return this as a *char
*/
static char * getFullPath(char dirPath[], char fileName[])
{
	char *temp = malloc( sizeof (char) * (strlen(dirPath)+1 + strlen(fileName)+1));		// This memory has to be freed at the callers discretion 				
	char *fullPath;
	CHECK_ALLOC(temp, "exec")
	fullPath = temp;
	
	if ( strcmp(dirPath, "./") == 0) {													// If command line input is . do not show directory path				
		fullPath = strcpy (fullPath, "");
	}
	else {
		fullPath = strcpy (fullPath, dirPath);
	}
	fullPath = strcat(fullPath, fileName);
	
	return fullPath;
}

/*  Parses the directory given and generates DIRECTORY_VECTOR 
	Can be called as a recursive function (for child directories)
	End result will be an unsorted DIR_V 
*/
void parse_dir(STAT_EXPRESSION statExpress, char dirPath[], int depth, int limit)
{
	/* init */
	if (!D_V_Ptr->init && !fl_UNLINK) {								/*  init DIRECTORY_VECTOR (unless already init) if we are NOT unlinking !!! */
		DIRECTORY_VECTOR = init_dir_vector();
	}
	dirPath = strcat(dirPath, "/");								// APPEND / to the end of the given directory path string
	char *fullPath;												// FULL pathname of a file - dirPath + file name 												
	
	/* EXEC */
	DIR *dir = opendir(dirPath);								// OPEN DIR
	struct dirent *d_Ptr;
	
	if (dir == NULL) { 											// If dir is NULL return error with perror - i.e. directory not found, too many symbolic links 		
		perror(dirPath);
		exit(EXIT_FAILURE);
	}					
	else {
		while ( (d_Ptr = readdir(dir)) != NULL) 					// WHILE dirent pointer d_Ptr has not reached the end of dir 										
		{		
			struct stat stat_Buff;											// Declare stat type + pointer to collect stat data about current file 		
			struct stat *stat_Ptr = &stat_Buff;				
			fullPath = getFullPath(dirPath, d_Ptr->d_name);																
			
			if (stat(fullPath, stat_Ptr) != 0) { 									/* CALL stat, return ERROR if returned */							
				perror(dirPath);
				ERROR_SAV = errno;														// If error occurs, save it to ERROR_SAV and continue
			}
			else if (S_ISDIR( stat_Ptr->st_mode ) ) 								/* IF REFERENCING DIRECTORY */												
			{									
				if ( strcmp(d_Ptr->d_name,".") && strcmp(d_Ptr->d_name,"..") )				// (Ignoring . and .. directories) - VERY IMPORTANT					
				{
					if ( ( fl_SHOW_HIDDEN || d_Ptr->d_name[0] != '.') )						// Determine if Hidden Folders (Starting with .) are ignored		
					{
						if ( !fl_UNLINK && evaluate_stat_expression(statExpress, d_Ptr->d_name, stat_Ptr) == true)		/* STAT EXPRESSION CHECK - If dir name matches stat expression	+ NOT UNLINKING */											
						{																							
							append_dir_vector(D_V_Ptr, fullPath, stat_Buff, depth);												// Append directory data to the DIR_V (before the recursion check)
						}
						
						if (limit < 0 || depth < limit) 																	/* CHECK FOR RECURSION - If limit is -1, OR if current depth is less than the limit */
						{
							parse_dir(statExpress, fullPath, depth+1, limit); 													// Call this function recursively, increasing depth by 1
						}									  										
						if (fl_UNLINK && evaluate_stat_expression(statExpress, d_Ptr->d_name, stat_Ptr) == true)			/* STAT EXPRESSION CHECK - If dir name matches stat expression	+ UNLINKING */														
						{
							if (rmdir(fullPath)!=0) {																				// Attempt to unlink the directory (after the recursion check - we are at the lowest dir)
								perror(fullPath);
								ERROR_SAV = errno;																					// perror + error saving if returns null
							}
						}
						free(fullPath);
					}
				}
			}
			else 																		/* IF REFERENCING A NON-DIRECTORY ITEM (file/links) */	
			{		
				if (fl_SHOW_HIDDEN == true || d_Ptr->d_name[0] != '.' )					// Ignore Hidden Items (Starting with .) unless specified otherwise
				{					
					if (evaluate_stat_expression(statExpress, d_Ptr->d_name, stat_Ptr) == true)		// If file/link name matches stat expression
					{					
						if (!fl_UNLINK) 																/* IF NOT UNLINKING */
						{
							append_dir_vector(D_V_Ptr, fullPath, stat_Buff, depth);		
						}
						else 														 					/* IF UNLINKING */
						{
							if (unlink(fullPath)!=0) {														// Attempt to unlink the file */
								perror(fullPath);
								ERROR_SAV = errno;															// If error occurs, save it to ERROR_SAV and continue*/
							}							
						}
						free(fullPath);
					}
				}
			}
		}
		closedir (dir);
	}
}
/*
	Function to print long list ( -l )
	FORMAT:
	Inode Permissions Link# Owner Group SizeBytes Modification_Time/Date FileName
*/
void printLongList()
{
	struct group *grp;																						// Define struct group & passwd pointers in order to derive user/group name
	struct passwd *pwd;
	char m_timedate[TIMEDATE_L];																				// Define char to use as buffer for strftime()
	
	for (int i = 0; i < D_V_Ptr->curr; i++ ){															// FOR each FILE_INFO in DIR_LIST
		pwd = getpwuid(D_V_Ptr->file[i].fStat.st_uid);															// Get user name
		grp = getgrgid(D_V_Ptr->file[i].fStat.st_gid);															// Get group name
		strftime(m_timedate, TIMEDATE_L, "%a %b %d %T %Y", localtime(&(D_V_Ptr->file[i].fStat.st_mtime)));		// Get time/date stamp
																					/* PRINTING */
		printf("%*llu ", INODE_DISPLAY_L, (unsigned long long) D_V_Ptr->file[i].fStat.st_ino);					// INODE ID
		printf( (S_ISDIR(D_V_Ptr->file[i].fStat.st_mode)) ? "d" : "-");												
		printf( (D_V_Ptr->file[i].fStat.st_mode & S_IRUSR) ? "r" : "-");
		printf( (D_V_Ptr->file[i].fStat.st_mode & S_IWUSR) ? "w" : "-");
		printf( (D_V_Ptr->file[i].fStat.st_mode & S_IXUSR) ? "x" : "-");
		printf( (D_V_Ptr->file[i].fStat.st_mode & S_IRGRP) ? "r" : "-");										// Permissions
		printf( (D_V_Ptr->file[i].fStat.st_mode & S_IWGRP) ? "w" : "-");
		printf( (D_V_Ptr->file[i].fStat.st_mode & S_IXGRP) ? "x" : "-");
		printf( (D_V_Ptr->file[i].fStat.st_mode & S_IROTH) ? "r" : "-");
		printf( (D_V_Ptr->file[i].fStat.st_mode & S_IWOTH) ? "w" : "-");
		printf( (D_V_Ptr->file[i].fStat.st_mode & S_IXOTH) ? "x" : "-");
		printf(" %*hu ", LINK_DISPLAY_L, (unsigned short) D_V_Ptr->file[i].fStat.st_nlink);						// No. of hard links
		printf("%s ", pwd->pw_name);																			// Name of Owner
		printf("%s ",grp->gr_name);																				// Group
		printf("%*lld ", SIZE_DISPLAY_L, (long long) D_V_Ptr->file[i].fStat.st_size);							// Size (in Bytes)
		printf("%s ", m_timedate);																				// Time/Date Stamp of Last Modification - Day/Week Month Day HH:MM:SS
		printf("%s\n", D_V_Ptr->file[i].fName);																	// Filename
	}
}

/*  Sorting and Printing DIRECTORY_VECTOR (dependant on option flags) to the command line
	Calls sort_dir_vector with relevant enum type if sorting is required
	then Prints output
	RETURNS either 0 or error code if errors occured during unlinking
*/
int sort_print()															
{																				/* DO NON-SORT CASES FIRST */		
	if (fl_COUNT_ONLY) {
			printf("%i\n", DIRECTORY_VECTOR.curr);										/* PRINT COUNT */
	}																
	else if (!fl_UNLINK)													
	{																			/* SORTING THE DIRECTORY VECTOR */
		if (fl_REVERSE_SORT) {															/* REVERSE SORT... */
			if (fl_SORT_TIME) {
				sort_dir_vector(D_V_Ptr, R_TIME);											/* TIME */
			}
			else if (fl_SORT_SIZE) {
				sort_dir_vector(D_V_Ptr, R_SIZE);											/* SIZE */
			}
			else {	
				sort_dir_vector(D_V_Ptr, R_NAME);											/* NAME */
			}
		}
		else {																			/* NORMAL SORT... */
			if (fl_SORT_TIME) {																/* TIME */
				sort_dir_vector(D_V_Ptr, TIME);
			}
			else if (fl_SORT_SIZE) {														/* SIZE */
				sort_dir_vector(D_V_Ptr, SIZE);
			}
			else {																			/* NAME */
				sort_dir_vector(D_V_Ptr, NAME);
			}
		}																				/* PRINTING */
		if (fl_LONG_LIST) {																/* LONG LIST */
			printLongList();
		}
		else {																			/* REGULAR */
			for (int i = 0; i < D_V_Ptr->curr; i++ ){
				#if (SHOW_DEPTH)
					printf("%i\t%s\n", D_V_Ptr->file[i].fDepth, D_V_Ptr->file[i].fName);
				#else
					puts(D_V_Ptr->file[i].fName);
				#endif
			}
		}
	}
	return ERROR_SAV;															/* Returns error code (if occured) OR 0 */
}