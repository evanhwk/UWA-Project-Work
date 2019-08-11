/*
   CITS2002 Project 2 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
 */
#include "cfind.h"
#include "init.h"
#include "exec.h"

#define DEFAULT_DEPTH_LIMIT -1				// No Depth limit by default ( see dir_to_vector() )
#define DEFAULT_EXPRESS "1"					// Default Stat Expression is TRUE

/*	MAIN function
	Check options and arguments (through initOpt)
	Call init functions
	Call exec Functions
	Pass parsed data from init functions to execution functions
	Exits on success or failure
*/
int main(int argc, char **argv)
{
	int notOpt;
	char *initDirPath;
	char *initStatExpress 	= DEFAULT_EXPRESS;
	int depth_LIMIT  		= DEFAULT_DEPTH_LIMIT;													
	bool flags[] 			= { false, false, false, false, false, false, false};  	/* Flags: { all, count, list, reverse, size, time, unlink} */
	STAT_EXPRESSION statExpress;
	
	if (argc == 1) {
		printf("_______________________________________________________________________________________________________________\n\n");
						  printf("Usage: %s [ -a -c -d <depth> -l -r -s -t -u ] <pathname> <stat-expression>\n", argv[0]);
						  printf("\n");
						  printf("-a\t\tRequests that entries beginning with . be considered.\n");
						  printf("-c\t\tPrint only the count of the number of matching file-entries, then EXIT.\n");
						  printf("-d [depth]\tLimits the search to the indicated depth of directories.\n");
						  printf("-l\t\tPrint a long listing of matching file-entries, printing in order (left to right):\n");
						  printf("\t\tinode, each entry's permissions, number of links, owner's name, group-owner's name, size,\n");
						  printf("\t\tmodificate-date, and name.\n-r\t\tReverse the order of any sorting options.\n");
						  printf("-s\t\tPrint matching file-entries, sorted by size.\n");
						  printf("\t\tIf both -s and -t are provided, -t takes precedence.\n");
						  printf("-t\t\tPrint matching file-entries, sorted by modification time.\n");
						  printf("-u\t\tAttempt to unlink (remove) as many matching file-entries as possible.\n");
						  printf("\n‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n");
						  exit(EXIT_SUCCESS);
	}
	notOpt = init_opt(argc, argv, flags, &depth_LIMIT);									// Checking Options
	parse_flags(flags);	
	init_arg(argc, argv, notOpt, &initDirPath, &initStatExpress);					// Checking + Init Arguments
												/*	END OF ARGUMENT / OPT CHECKING */
	statExpress = compile_stat_expression(initStatExpress); 						// CREATE Stat Express
	#if (EVAL)
		printf ("> Stat Expression is %s\n", initStatExpress);						
		printf ("> Directory Path is %s/\n", initDirPath);
		printf ("> Depth Value is %i\n", depth);
	#endif
	dir_to_vector(initDirPath, statExpress, 0, depth_LIMIT);						// CREATE directory vector
	#if (EVAL)
		printf("\n");
	#endif
	free_stat_expression(statExpress);												// FREE Stat Express
	if ( exec_sort_print() != 0 )  exit(EXIT_FAILURE);								// EXEC SORTING and PRINTING
	exit(EXIT_SUCCESS);
}
