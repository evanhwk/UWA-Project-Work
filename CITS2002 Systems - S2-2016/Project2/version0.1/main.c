#include "cfind.h"

#define DEFAULT_DIR "./"
#define DEFAULT_DEPTH 0
#define DEFAULT_EXPRESS "1"

/*	MAIN function
	Check options and arguments (through initOpt)
	Calling init functions
	Pass parsed data from init functions to execution functions
*/
int main(int argc, char **argv)
{
	int notOpt, argErr;																		
	char *initDirPath = DEFAULT_DIR;
	char *initStatExpress = DEFAULT_EXPRESS;
	
	STAT_EXPRESSION statExpress;
	DIR *dirPath;
	int depValue = DEFAULT_DEPTH;													
	bool fl[] = { false, false, false, false, false, false, false, false };  	/* Flags: { all, count, depth, list, reverse, size, time, unlink} */
	
	notOpt = initOpt(argc, argv, fl, &depValue);									
	if ( notOpt == -1 ) {															/* Checking Options */	
		exit(EXIT_FAILURE);
	}
	argErr = initArg(argc, argv, notOpt, initDirPath, initStatExpress);
	if (argErr != 0) {																/* Checking Arguments */
		exit(EXIT_FAILURE);
	}
	statExpress = compile_stat_expression(initStatExpress);
	dirPath = opendir(initDirPath);
		if (statExpress != NULL && dirPath != NULL)
		{
			printf("> SUCCESS\n");
		}
	listDir(dirPath);
	return 0;
}
