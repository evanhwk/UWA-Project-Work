#include "cfind.h"
#include <ctype.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>

#define	OPTLIST		"acd:lrstuh"

/* 	Runs getopt based on given params 
	Parses raw option data into flag array
	RETURNS the last index of **argv of which getopt() != 1 
	RETURNS ON ERROR -1
*/
int initOpt(int argc, char **argv, bool fl[], int *depth)
{
	int opt;
	opterr	= 0;
	//   While getopt() returns a value that is not -1 (i.e. there are options still uninitialised)
    while((opt = getopt(argc, argv, OPTLIST)) != -1) {
		switch(opt) {						//   Switch statement to set flags for -a -c -d [depth] -h -l -r -s -t -u 
			case 'a': fl[0] = true; 
					  break;
			case 'c': fl[1] = true; 
					  break;
			case 'd': { char *stl_ptr;														//  char pointer to evaluate if any non-integer characters 
					  *depth = strtol(optarg, &stl_ptr, 10); 								//  Use strtol() - set return value as depth and pass the pointer stl_ptr
						if (!(*depth >= 0 && (*stl_ptr == '\0' || *stl_ptr == '\n'))){		//  if depth is NOT above 0 and if the pointer is filled and not null
							fprintf (stderr, "ERROR: Option -d requires a positive integer argument with no other characters\n");
							return -1;
						}
					  }
					  break;
			case 'l': fl[2] = true; 
					  break;
			case 'r': fl[3] = true; 
					  break;
			case 's': fl[4] = true; 
					  break;
			case 't': fl[5] = true;
					  break;
			case 'u': fl[6] = true; 
					  break;
			case 'h': printf("_______________________________________________________________________________________________________________\n\n");
					  printf("Usage: %s [ -a -c -d <depth> -l -r -s -t -u ] <pathname> <stat-expression>\n", argv[0]);
					  printf("\n");
					  printf("-a\t\tSpecifying -a requests that entries beginning with . be considered.\n");
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
			case '?': 						// when opt is ? - error has occured - Process error
				if (optopt == 'd') {
					fprintf (stderr, "ERROR: Option -%c requires an argument.\n", optopt);
					fprintf (stderr, "Usage: %s ** [ -a -c -d [depth] -l -r -s -t -u ] ** <pathname> <stat-expression>\n", argv[0]);
				}
				else if (isprint (optopt)) {
					fprintf (stderr, "ERROR: Unknown option '-%c'.\n", optopt);
					fprintf (stderr, "Usage: %s  ** [ -a -c -d [depth] -l -r -s -t -u ] ** <pathname> <stat-expression>\n", argv[0]);
				}
				else {
					fprintf (stderr, "ERROR: Unknown option character '\\x%x'.\n", optopt);
					fprintf (stderr, "Usage: %s ** [ -a -c -d [depth] -l -r -s -t -u ] ** <pathname> <stat-expression>\n", argv[0]);
				}
				return -1;
			default: return -1;
		}
	}
	return optind;
}

/* Parses **argv char array into char pointers, beginning from argv[notOpt]
	RETURNS 0
	RETURNS ON ERROR -1
*/
int initArg(int argc, char **argv, int notOpt, char **dirPath, char **statExpress)
{
	if ( (argc-notOpt) > 2)															//  If number of non option arguments is greater than 2, incorrect input detected
	{
		fprintf (stderr, "ERROR: Incorrect number of Arguments.\n");
		fprintf (stderr, "Usage: %s [options] ** <pathname> <stat-expression> **\n", argv[0]);
		return -1;
	}
	for (int notOptCount = 0; notOpt < argc; notOpt++, notOptCount++) {
		if (notOptCount == 0) {
			*dirPath = malloc(sizeof(argv[notOpt]));
			*dirPath = argv[notOpt]; 
		}
		else if (notOptCount == 1) {
			*statExpress = malloc(sizeof(argv[notOpt]));
			*statExpress = argv[notOpt];
		}
	}
	return 0;
}

