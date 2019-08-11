#ifndef _CFIND_H
#define _CFIND_H
	#include "cfind.h"
#endif
#include <dirent.h>
#include <statexpr.h>
//#include <sys/stat.h>	
//#include <string.h>			
//#include <time.h> 						

extern void parse_flags(bool fl[]);
extern void dir_to_vector(char dirPath[], STAT_EXPRESSION statExpress, int depth);
extern void exec_print();
