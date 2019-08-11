/*
   CITS2002 Project 2 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
 */
#include "exec.h"
#include "init.h"

#define DEFAULT_DEPTH_LIMIT -1				// No Depth limit by default
#define DEFAULT_EXPRESS_STRING "1"			// Default Stat Expression is TRUE

#define	OPTLIST		"acd:lrstu"

/*	Prints out a help prompt
 */
static void printHelp()
{
	printf("Usage: %s [options] path [expression]\n", PROG_NAME);
	puts("where options are:\n");
	puts("-a\t\tRequests that entries beginning with . be considered.");
	puts("-c\t\tPrint only the count of the number of matching file-entries, then EXIT.");
	puts("-d [depth]\tLimits the search to the indicated depth of directories.");
	puts("-l\t\tPrint a long listing of matching file-entries, printing in order (left to right):");
	puts("\t\tinode, each entry's permissions, number of links, owner's name, group-owner's name, size,");
	puts("\t\tmodificate-date, and name.\n-r\t\tReverse the order of any sorting options.");
	puts("-s\t\tPrint matching file-entries, sorted by size.");
	puts("\t\tIf both -s and -t are provided, -t takes precedence.");
	puts("-t\t\tPrint matching file-entries, sorted by modification time.");
	puts("-u\t\tAttempt to unlink (remove) as many matching file-entries as possible.");
}

/* 	Runs getopt based on given params 
 *	Parses raw option data into flag array
 *	RETURNS the last index of **argv of which getopt() != 1 (ON ERROR -1)
*/
static int initOpt(int argc, char **argv, bool fl[], int *depthLimit)
{
	int opt;
	opterr = 0;										// Indicates that this program will print out errors to stderr manually
	while((opt = getopt(argc, argv, OPTLIST)) != -1) {					// While getopt() returns a value that is not -1 (i.e. there are options still uninitialised)
		switch(opt) {										//   Switch statement to set flags for -a -c -d [depth] -h -l -r -s -t -u 
			case 'a': fl[0] = true; break;
			case 'c': fl[1] = true; break;
			case 'd': {
					char *string_Buff;							//  temp string to evaluate if any non-integer characters 
					int int_Buff;
					int_Buff = strtol(optarg, &string_Buff, 10); 				//  Use strtol() - set return value as depth and pass the pointer stl_ptr as buffer
					if ((int_Buff >= 0 && (*string_Buff == '\0' || *string_Buff == '\n'))) {	//  if depth is above 0 and if the pointer is not referencing a char array
						*depthLimit = int_Buff;
					}
					else {
						fprintf(stderr, "%s: illegal argument for option '-d'\n", PROG_NAME);
						printHelp();
						exit(EXIT_FAILURE);
					}
					break;
				  }
			case 'l': fl[2] = true; break;
			case 'r': fl[3] = true; break;
			case 's': fl[4] = true; break;
			case 't': fl[5] = true; break;
			case 'u': fl[6] = true; break;
			case '?': if (optopt == 'd') {
					fprintf(stderr, "%s: option '-d' requires an argument\n", PROG_NAME);
				  }
				  else if (isprint (optopt)) {
          				fprintf (stderr, "%s: illegal option `-%c'.\n", PROG_NAME, optopt);
				  }
       				  else {
         				fprintf (stderr, "%s: unknown option character `\\x%x'.\n", PROG_NAME, optopt);
				  }
				  printHelp();									// When opt is ? - error has occured (handled by getopt library), print help prompt, then exit.
				  exit(EXIT_FAILURE);
			default:  abort();									// If this occurs it is due to a coding error, therefore use abort and not exit
		}
	}
	return optind;
}

/*  	Parses **argv char array into char pointers, beginning from argv[notOpt]
*/
static void initArg(int argc, char **argv, int notOpt, char **dirPath, char **evalString)
{
	char *temp;
	if ( (argc-notOpt) > 2 || (argc-notOpt) == 0)										//  If number of non option arguments is greater than 2, incorrect input detected
	{
		if (argc != 1) {										 		// Providing just program name prints out ONLY help prompt
			fprintf (stderr, "%s: incorrect number of arguments\n", PROG_NAME);
		}
		printHelp();
		exit(EXIT_FAILURE);
	}
	for (int notOptCount = 0; notOpt < argc; notOpt++, notOptCount++) {							// Loop through non-opt arguments
		if (notOptCount == 0) {
			temp = malloc(sizeof(argv[notOpt])+1);									/* size+1 as this string will be modified later on during execution*/
			CHECK_ALLOC(temp, "init")		// see MACROS in cfind.h  - if temp is not NULL
			*dirPath = temp;
			*dirPath = argv[notOpt];
		}
		else if (notOptCount == 1) {
			temp = malloc(sizeof(argv[notOpt]));
			CHECK_ALLOC(temp, "init")		// see MACROS in cfind.h - if temp is not NULL
			*evalString = temp;
			*evalString = argv[notOpt];
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
	int ERROR_SAV;
	char *initDirPath;
	char *initEvalString	= DEFAULT_EXPRESS_STRING;
	int depthLimit  	= DEFAULT_DEPTH_LIMIT;
	bool optFlags[] 	= { false, false, false, false, false, false, false};  	/* Flags: { all, count, list, reverse, size, time, unlink} */
	STAT_EXPRESSION statExpress;

	notOpt = initOpt(argc, argv, optFlags, &depthLimit);			// Checking Options + init Opt Flags (pass optflags and &depthLimit to be modified)
	initArg(argc, argv, notOpt, &initDirPath, &initEvalString);		// Checking + Init Arguments (pass address of initDirPath and initEvalString to be modified)
										/* END OF ARGUMENT / OPT CHECKING */
	statExpress = compile_stat_expression(initEvalString); 				// CREATE Stat Express
	parse_flags(optFlags);								// Parse flags (i.e. -ts -> -t, etc)
	ERROR_SAV = parse_dir(statExpress, initDirPath, 0, depthLimit);			// Parse data, create DIR_V [if necessary] OR unlink files, return error no.
	free_stat_expression(statExpress);							// FREE Stat Express
	sort_print();									// Execute SORTING and PRINTING
	if ( ERROR_SAV != 0) { exit(EXIT_FAILURE); }					// Calling exit_failure if any errors occured
	exit(EXIT_SUCCESS);								// exit
}
