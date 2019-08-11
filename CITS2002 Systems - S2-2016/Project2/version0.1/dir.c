#include "cfind.h"

int listDir(DIR *dir)
{
	struct dirent *ep;
	if (dir != NULL) {
		while ( (ep = readdir(dir)) ){
			printf("%s\n", ep->d_name);
		}
		(void) closedir (dir);
	}
	else {
		perror ("Couldn't open the directory");
	}
	return 0;
}