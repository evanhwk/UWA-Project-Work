#include "exec.h"
#include "init.h"

#define DEFAULT_DEPTH -1				/* no depth limit by default ( see dirToList() ) */
#define DEFAULT_EXPRESS "1"				/* true */

/*	MAIN function
	Check options and arguments (through initOpt)
	Call init functions
	Call exec Functions
	Pass parsed data from init functions to execution functions
*/
int main(int argc, char **argv)
{
	int notOpt, argErr;
	char *initDirPath;
	char *initStatExpress 	= DEFAULT_EXPRESS;
	int depth  				= DEFAULT_DEPTH;													
	bool flags[] 			= { false, false, false, false, false, false, false };  	/* Flags: { all, count, list, reverse, size, time, unlink} */
	STAT_EXPRESSION statExpress;
	
	if (argc == 1) {
		printf("_______________________________________________________________________________________________________________\n\n");
						  printf("Usage: %s [ -a -c -d <depth> -l -r -s -t -u ] <pathname> <stat-expression>\n", argv[0]);
						  printf("\n");
						  printf("-a\t\tRequests that entries beginning with . be considered.\n");
						  printf("-c\t\tPrint only the count of the number of matching file-entries, then EXIT.\n");
						  printf("-d [depth]\tLimits the search to the indicated depth of directories.\n");
						  printf("-l\t\tPrint a long listing of matching file-entries, printing in order (left to right):\n");
						  printf("\t\teach entry's permissions, inode, number of links, owner's name, group-owner's name, size,\n");
						  printf("\t\tmodificate-date, and name.\n-r\t\tReverse the order of any sorting options.\n");
						  printf("-s\t\tPrint matching file-entries, sorted by size.\n");
						  printf("\t\tIf both -s and -t are provided, -t takes precedence.\n");
						  printf("-t\t\tPrint matching file-entries, sorted by modification time.\n");
						  printf("-u\t\tAttempt to unlink (remove) as many matching file-entries as possible.\n");
						  printf("\n‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n");
						  exit(EXIT_SUCCESS);
	}
	notOpt = initOpt(argc, argv, flags, &depth);
	if (notOpt == -1) {																/* Checking Options */	
		exit(EXIT_FAILURE);
	}
	argErr = initArg(argc, argv, notOpt, &initDirPath, &initStatExpress);
	if (argErr != 0) {																/* Checking + Init Arguments */
		exit(EXIT_FAILURE);
	}
	parseFlags(flags);																/* Init Option Flags */
	statExpress = compile_stat_expression(initStatExpress); 						/* CREATE Stat Express */
	#if (EVAL)
		printf ("> Stat Expression is %s\n", initStatExpress);						
		printf ("Directory Path is %s/\n", initDirPath);
		printf ("Depth Value is %i\n", depth);
	#endif
	dirToList(initDirPath, statExpress, depth);										/* CREATE dir List */
	#if (EVAL)
		printf("\n");
	#endif
	free_stat_expression(statExpress);												/* FREE Stat Express */
	//sortList();
	printList();
	return 0;
}
