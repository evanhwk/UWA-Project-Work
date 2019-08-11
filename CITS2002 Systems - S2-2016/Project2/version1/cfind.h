#include <stdio.h>
#include <stdlib.h>											// malloc etc
#include <stdbool.h>

#define EVAL false
/* MACROS */

/* Calling perror() and exit(EXIT_FAILURE) */
#define P_ERROR_EXIT(program) {		\
        perror((program));			\
		exit(EXIT_FAILURE); }	
		
/* Checking if memory has been allocated correctly
	Exiting if failure detected */
#define CHECK_ALLOC(t, string) { 	 		 \
			if (t == NULL) {				 \
				perror(string);				 \
				exit(EXIT_FAILURE); 		 \
			}								 \
		}
