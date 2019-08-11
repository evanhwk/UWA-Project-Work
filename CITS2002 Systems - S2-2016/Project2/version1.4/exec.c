/*
   CITS2002 Project 2 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
 */
#include "dirVect.h"
#include "exec.h"

#ifdef __linux__
	#define INODE_DISPLAY_L 18																				// Defining display limits for long list
#else
	#define INODE_DISPLAY_L 7
#endif
#define LINK_DISPLAY_L 2
#define SIZE_DISPLAY_L 8
#define TIMEDATE_L 26

static bool fl_SHOW_HIDDEN, fl_COUNT_ONLY, fl_LONG_LIST, fl_REVERSE_SORT, fl_SORT_SIZE, fl_SORT_TIME, fl_UNLINK = false; // see parseFlags
static DIR_V DIRECTORY_VECTOR, *D_V_Ptr = &DIRECTORY_VECTOR; 							// Dynamic Vector which holds references to file names and data 				*/
static int ERROR_SAV = 0;															// For memory of non-fatal errors

/* 	Initialises File Scope Variables for various options (to be used with exec functions)
	Uses boolean array passed from the main function
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
}

/*  Get the full path of the current file 
	Return this as a *char
*/
static char * getFullPath(char dirPath[], char fileName[])
{
	char *temp = malloc( sizeof (char) * (strlen(dirPath)+1 + strlen(fileName)+1));		// This memory has to be freed at the callers discretion 				
	char *fullpath;
	CHECK_ALLOC(temp, "exec")
	fullpath = temp;
	
	if ( strcmp(dirPath, "./") == 0) {													// If command line input is . do not show directory path				
		fullpath = strcpy (fullpath, "");
	}
	else {
		fullpath = strcpy (fullpath, dirPath);
	}
	fullpath = strcat(fullpath, fileName);
	
	return fullpath;
}

/*  Parses the directory given and generates a DIRECTORY_VECTOR 
	Can be called as a recursive function (for child directories)
	End result will be an unsorted DIR_V  
*/
void dir_to_vector(char dirPath[], STAT_EXPRESSION statExpress, int depth, int limit)
{
	/* init */
	if (!D_V_Ptr->init && !fl_UNLINK) {								/* !!!  IF NOT UNLINKING, init DIRECTORY_VECTOR (unless already init) !!! */
		DIRECTORY_VECTOR = init_dir_vector();
	}
	
											/* 			RECURSION BEGINS   		*/																			
	
	dirPath = strcat(dirPath, "/");								// APPEND / to the end of the given directory path string
	char *fullpath;												// FULL pathname of a file - dirPath + file name 												
	
	/* EXEC */
	DIR *dir = opendir(dirPath);								// OPEN DIR
	struct dirent *d_Ptr;
	
	if (dir == NULL) { 											// If dir is NULL return error with perror - i.e. directory not found 							
		P_ERROR_EXIT("exec")	
	}					
	else {
		while ( (d_Ptr = readdir(dir)) != NULL ) 					// WHILE dirent pointer d_Ptr has not reached the end of dir 										
		{					
			struct stat stat_Buff;											// Declare stat type + pointer to collect stat data about current file 		
			struct stat *stat_Ptr = &stat_Buff;				
			fullpath = getFullPath(dirPath, d_Ptr->d_name);																
			
			if (stat(fullpath, stat_Ptr) != 0) { 									/* CALL stat, return ERROR if returned */							
				P_ERROR_EXIT("exec")												
			}
			else if (S_ISDIR( stat_Ptr->st_mode ) ) 								/* IF REFERENCING DIRECTORY */												
			{									
				if ( strcmp(d_Ptr->d_name,".") && strcmp(d_Ptr->d_name,"..") )				// (Ignoring . and .. directories) - VERY IMPORTANT					
				{
					if ( (fl_SHOW_HIDDEN == true || d_Ptr->d_name[0] != '.') )						// Determine if Hidden Folders (Starting with .) are ignored		
					{
						if (!fl_UNLINK) 													/* IF NOT UNLINKING */
						{
							if (evaluate_stat_expression(statExpress, d_Ptr->d_name, stat_Ptr) == true)	{		// If dir name matches stat expression			
								append_dir_vector(D_V_Ptr, fullpath, stat_Buff, depth);									// Append validated data to the DIR_V
							}
						}
						if (limit < 0 || depth < limit) 													/* CHECK FOR RECURSION - If limit is -1, OR if current depth is less than the limit */
						{																					/* Below 0 = Infinite possible recursion */
							dir_to_vector(fullpath, statExpress, depth+1, limit); 								// Call this function recursively, increasing depth by 1	
	
						}
										/* 			RECURSION ENDS   		*/																				
						if (fl_UNLINK)														/* IF  UNLINKING */														
						{
							if (rmdir(fullpath)!=0) {										// Attempt to unlink the directory
								perror(fullpath);
								ERROR_SAV = errno;											// If error occurs, save it to ERROR_SAV and continue
							}
						}
						free(fullpath);
					}
				}
			}
			else 																	/* IF REFERENCING A NON-DIRECTORY ITEM (file) */	
			{		
				if (fl_SHOW_HIDDEN == true || d_Ptr->d_name[0] != '.' )					// Ignore Hidden Files (Starting with .) unless specified otherwise
				{					
					if (evaluate_stat_expression(statExpress, d_Ptr->d_name, stat_Ptr) == true)		// If file name matches stat expression
					{					
						if (!fl_UNLINK) 																/* IF NOT UNLINKING */
						{
							append_dir_vector(D_V_Ptr, fullpath, stat_Buff, depth);								// Append validated data to the DIR_V
						} 
						else if (fl_UNLINK)											 					/* IF UNLINKING */
						{
							if (unlink(fullpath)!=0) {														// Attempt to unlink the file */
								perror(fullpath);
								ERROR_SAV = errno;															// If error occurs, save it to ERROR_SAV and continue*/
							}
						}
						free(fullpath);
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
		printf("%*hu ", LINK_DISPLAY_L, (unsigned short) D_V_Ptr->file[i].fStat.st_nlink);						// No. of hard links
		printf("%s ", pwd->pw_name);																			// Name of Owner
		printf("%s ",grp->gr_name);																				// Group
		printf("%*lld ", SIZE_DISPLAY_L, (long long) D_V_Ptr->file[i].fStat.st_size);							// Size (in Bytes)
		printf("%s ", m_timedate);																				// Time/Date Stamp of Last Modification - Day/Week Month Day HH:MM:SS
		printf("%s\n", D_V_Ptr->file[i].fName);																	// Filename
	}
}


/*  Execute Sorting and Printing (dependant on option flags) to the command line
	Rules in Place (Higher = More Priority) for -c -s -t -u 
	* -c | Only print size of DIRECTORY_VECTOR to command line + No sorting
	* -u | No Printing + No Sorting - Return Error codes
	* -t | Sorting by Time/Date
	* -s | Sort by Size
	
	Calls sort_dir_vector with relevant enum type if sorting is required
	then Prints output
	RETURNS either 0 or error code if errors occured during unlinking
*/
int exec_sort_print()															
{																				/* DO NON-SORT CASES FIRST */		
	if (fl_COUNT_ONLY) {
			printf("%i\n", DIRECTORY_VECTOR.curr);										/* PRINT COUNT */
	}																
	else if (fl_UNLINK) {																/* UNLINK */ 
		return ERROR_SAV;																	/* Returns error code (if occured) OR 0 */
	}
	else
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
	return 0;
}
