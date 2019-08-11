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
	char *initDirPath		= DEFAULT_DIR;
	char *initStatExpress 	= DEFAULT_EXPRESS;
	int depth  				= DEFAULT_DEPTH;													
	bool flags[] 			= { false, false, false, false, false, false, false };  	/* Flags: { all, count, list, reverse, size, time, unlink} */
	
	notOpt = initOpt(argc, argv, flags, &depth);									
	if (notOpt == -1) {															/* Checking Options */	
		exit(EXIT_FAILURE);
	}
	argErr = initArg(argc, argv, notOpt, &initDirPath, &initStatExpress);
	if (argErr != 0) {																/* Checking Arguments */
		exit(EXIT_FAILURE);
	}
	parseFlags(flags);
	#if ( EVAL )
		printf("> Depth Value is %i\n", depth);
	#endif
	dirToList(initDirPath, initStatExpress);
	//readList();
	return 0;
}
