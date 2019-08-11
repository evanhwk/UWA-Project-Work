#include <stdio.h>
#include <stdlib.h>											// malloc etc
#include <stdbool.h>

#define PROG_NAME "cfind"
#define SHOW_DEPTH false										// Show/Hide depth in output

/* MACROS */

/* Checking if memory has been allocated correctly
	Exiting if failure detected */
#define CHECK_ALLOC(t, string) { 	 		\
			if (t == NULL) {		\
				perror(string);		\
				exit(EXIT_FAILURE); 	\
			}				\
		}
