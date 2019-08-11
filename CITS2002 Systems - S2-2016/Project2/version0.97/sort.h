#include <time.h>

#ifdef __linux__										/* for local use */
struct  timespec {
        time_t  tv_sec ;
        long  tv_nsec ;
};
#endif

enum sortType {
	NAME,
	TIME,
	SIZE,
	R_NAME,
	R_TIME,
	R_SIZE
};

extern void sortList(DIR_LIST *dL, enum sortType sortBy);
