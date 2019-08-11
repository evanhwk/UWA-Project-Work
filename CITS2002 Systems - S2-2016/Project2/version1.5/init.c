/*
   CITS2002 Project 2 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
 */
#include "exec.h"
#include "init.h"

#define DEFAULT_DEPTH_LIMIT -1				// No Depth limit by default ( see dir_to_vector() )
#define DEFAULT_EXPRESS "1"					// Default Stat Expression is TRUE

#define	OPTLIST		"acd:lrstu"

/* 	Runs getopt based on given params 
	Parses raw option data into flag array
	RETURNS the last index of **argv of which getopt() != 1 (ON ERROR -1)
*/
int init_opt(int argc, char **argv, bool fl[], int *depth)
{
	int opt;
	//   While getopt() returns a value that is not -1 (i.e. there are options still uninitialised)
    while((opt = getopt(argc, argv, OPTLIST)) != -1) {
		switch(opt) {						//   Switch statement to set flags for -a -c -d [depth] -h -l -r -s -t -u 
			case 'a':	fl[0] = true; break;
			case 'c':	fl[1] = true; break;
			case 'd':	{													/* This check provides more robust opt checking than the sample but is still good to include (i.e. -d3c returns an error now) */
							char *stl_ptr;														//  char pointer to evaluate if any non-integer characters 
							int temp;
							temp = strtol(optarg, &stl_ptr, 10); 								//  Use strtol() - set return value as depth and pass the pointer stl_ptr
							if ((temp >= 0 && (*stl_ptr == '\0' || *stl_ptr == '\n')))		 	//  if depth is above 0 and if the pointer is filled and not null
							{
								*depth = temp;
							}
							else 
							{
								fprintf (stderr, "%s: option requires an argument -- '-d'\n-d requires a positive integer argument with no other characters\n", argv[0]);		
								exit(EXIT_FAILURE);
							}
							break;
						}	
			case 'l':	fl[2] = true; break;
			case 'r':	fl[3] = true; break;
			case 's':	fl[4] = true; break;
			case 't':	fl[5] = true; break;
			case 'u':	fl[6] = true; break;
			case '?': 	if (isprint (optopt)) 													// When opt is ? - error has occured - Process error
						{
							fprintf (stderr, "Usage: %s  ** [ -a -c -d [depth] -l -r -s -t -u ] ** <pathname> <stat-expression>\n", argv[0]);
							exit(EXIT_FAILURE);
						}
						else 
						{
							fprintf (stderr, "Usage: %s ** [ -a -c -d [depth] -l -r -s -t -u ] ** <pathname> <stat-expression>\n", argv[0]);
							exit(EXIT_FAILURE);
						}
						exit(EXIT_FAILURE);
			default:	fprintf (stderr, "Usage: %s ** [ -a -c -d [depth] -l -r -s -t -u ] ** <pathname> <stat-expression>\n", argv[0]);
						abort();																// If this occurs it is due to a coding error, therefore use abort and not exit
		}
	}
	return optind;
}

/*  Parses **argv char array into char pointers, beginning from argv[notOpt]
*/
void init_arg(int argc, char **argv, int notOpt, char **dirPath, char **statExpress)
{
	char *temp;
	if ( (argc-notOpt) > 2 || (argc-notOpt) == 0)															//  If number of non option arguments is greater than 2, incorrect input detected
	{
		fprintf (stderr, "init: Incorrect number of Arguments.\n");
		fprintf (stderr, "Usage: %s [options] ** <pathname> <stat-expression> **\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	for (int notOptCount = 0; notOpt < argc; notOpt++, notOptCount++) {
		if (notOptCount == 0) {
			temp = malloc(sizeof(argv[notOpt])+1);															/* size+1 as this string will be modified later on during execution*/
			CHECK_ALLOC(temp, "init")
			*dirPath = temp;												
			*dirPath = argv[notOpt];
		}
		else if (notOptCount == 1) {
			temp = malloc(sizeof(argv[notOpt]));
			CHECK_ALLOC(temp, "init")
			*statExpress = temp;
			*statExpress = argv[notOpt];
		}
	}
}

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
	dir_to_vector(initDirPath, statExpress, 0, depth_LIMIT);						// CREATE directory vector
	free_stat_expression(statExpress);												// FREE Stat Express
	if ( exec_sort_print() != 0 )  exit(EXIT_FAILURE);								// EXEC SORTING and PRINTING
	exit(EXIT_SUCCESS);
}
