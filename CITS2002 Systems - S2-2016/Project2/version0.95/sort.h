#include <time.h>

#ifdef __linux__
struct  timespec {
        time_t  tv_sec ;		/* Seconds. */
        long  tv_nsec ;			/* Nanoseconds. */
};
#endif

extern int sortList(DIR_LIST *dL, enum sortType sortBy);
