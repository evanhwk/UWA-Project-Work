#include <stdio.h>
#include <stdlib.h>											// malloc etc
#include <stdbool.h>

#define SHOW_DEPTH true										// Show/Hide depth in output

/* MACROS */

/* Checking if memory has been allocated correctly
	Exiting if failure detected */
#define CHECK_ALLOC(t, string) { 	 		 \
			if (t == NULL) {				 \
				perror(string);				 \
				exit(EXIT_FAILURE); 		 \
			}								 \
		}

