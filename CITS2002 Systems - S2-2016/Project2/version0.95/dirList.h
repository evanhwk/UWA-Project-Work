#include <string.h>
#include <time.h>

//typedef void *         DIR_LIST;

extern DIR_LIST initDirList();
extern void appendDirList(DIR_LIST * dirList, char nameBuff[], struct stat statBuff);