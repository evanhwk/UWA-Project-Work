#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <ctype.h>
#include <unistd.h>
#include <getopt.h>

#include <assert.h>

#define DEFAULT_DIR "./"
#define DEFAULT_DEPTH 0
#define DEFAULT_EXPRESS "-1"

#define	OPTLIST		"acd:lrstuh"
#define EVAL true

int main(int argc, char *argv[])
{
	int opt;
	char *initDirPath = DEFAULT_DIR;
	char *initStatExpress = DEFAULT_EXPRESS;
	bool FLAGS[] = { false, false, false, false, false, false, false, false };  	/* Flags: { all, count, depth, list, reverse, size, time, unlink} */
	int depValue = DEFAULT_DEPTH;
													     
	opterr	= 0;
	/*  While getopt() returns a value that is not -1 (i.e. there are options still uninitialised) */
    while((opt = getopt(argc, argv, OPTLIST)) != -1) {
		switch(opt) {						/*  Switch statement to set flags for -a -c -d [depth] -h -l -r -s -t -u  */
			case 'a': FLAGS[0] = true; 
					  break;
			case 'c': FLAGS[1] = true; 
					  break;
			case 'd': FLAGS[2] = true; 
					  char *stl_ptr;										/* char pointer to evaluate if any non-integer characters */
					  depValue = strtol(optarg, &stl_ptr, 10); 				/* Use strtol() - set return value as depValue and pass the pointer stl_ptr */
					  #if (EVAL == true)
						  printf("depValue is %i, POINTER IS %s\n", depValue, stl_ptr);
					  #endif
					  if (!(depValue >= 0 && (*stl_ptr == '\0' || *stl_ptr == '\n'))){		/* if depValue is NOT above 0 and if the pointer is filled and not null */
						fprintf (stderr, "ERROR: Option -d requires a positive integer argument with no other characters\n");
						exit(EXIT_FAILURE);
						return 1;
					  }
					  break;
			case 'l': FLAGS[3] = true; 
					  break;
			case 'r': FLAGS[4] = true; 
					  break;
			case 's': FLAGS[5] = true; 
					  break;
			case 't': FLAGS[6] = true;
					  break;
			case 'u': FLAGS[7] = true; 
					  break;
			case 'h': fprintf(stdout, "====================================================================================================\nUsage: %s -a -c -d [depth] -l -r -s -t -u \n\n\n-a\t\tSpecifying -a requests that entries beginning with . be considered.\n-c\t\tPrint only the count of the number of matching file-entries, then EXIT.\n-d [depth]\tLimits the search to the indicated depth of directories.\n-l\t\tPrint a long listing of matching file-entries, printing in order (left to right):\n\t\teach entry's permissions, inode, number of links, owner's name, group-owner's name, size,\n\t\tmodificate-date, and name.\n-r\t\tReverse the order of any sorting options.\n-s\t\tPrint matching file-entries, sorted by size.\n\t\tIf both -s and -t are provided, -t takes precedence.\n-t\t\tPrint matching file-entries, sorted by modification time.\n-u\t\tAttempt to unlink (remove) as many matching file-entries as possible.\n====================================================================================================\n", argv[0]);
					  break;
			case '?': 						/* when opt is ? - error has occured - Process error */
				if (optopt == 'd') {
					fprintf (stderr, "ERROR: Option -%c requires an argument\n", optopt);
				}
				else if (isprint (optopt)) {
					fprintf (stderr, "ERROR: Unknown option '-%c'.\n", optopt);
				}
				else {
					fprintf (stderr, "ERROR: Unknown option character '\\x%x'.\n", optopt);
				}
				exit(EXIT_FAILURE);
				return 1;
			default: abort();
		}
    }
	#if (EVAL == true)
		printf("all = %d\n", FLAGS[0]); 
		printf("count = %d\n", FLAGS[1]); 
		printf("depth = %d; value = %i\n", FLAGS[2], depValue);
		printf("list = %d\n", FLAGS[3]); 
		printf("reverse = %d\n", FLAGS[4]); 
		printf("size = %d\n", FLAGS[5]); 
		printf("time = %d\n", FLAGS[6]);
		printf("unlink = %d\n", FLAGS[7]);
	#endif
	
	for (int notOptCount = 0; optind < argc; optind++, notOptCount++) {
		if (notOptCount == 0) {
			initDirPath = malloc(sizeof(argv[optind]));
			initDirPath = argv[optind]; 
			#if (EVAL == true)
				printf ("Directory Path is %s\n", initDirPath);
			#endif
		}
		else if (notOptCount == 1) { 
			initStatExpress = malloc(sizeof(argv[optind]));
			initStatExpress = argv[optind];
			#if (EVAL == true) 
				printf ("Stat Expression is %s\n", initStatExpress);
			#endif
		}
		else if (notOptCount > 1) {
			fprintf (stderr, "ERROR: Incorrect number of Arguments.\nUsage: %s [options] pathname 'stat-expression'\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	
	/* call function(const char *initDirPath, const char *initStateExpress, bool *FLAGS, int depth) */
	return 0;
}