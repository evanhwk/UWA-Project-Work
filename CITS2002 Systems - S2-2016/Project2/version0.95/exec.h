#include <sys/types.h>
#include <dirent.h>
#include <statexpr.h>

extern void parseFlags(bool fl[]);
extern void dirToList(char dirPath[], STAT_EXPRESSION statExpress, int depth);
extern void execPrint();
