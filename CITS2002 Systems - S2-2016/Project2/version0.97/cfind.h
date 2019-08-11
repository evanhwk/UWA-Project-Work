#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define EVAL false
/* MACROS */

#define P_ERROR_EXIT(program) {		\
        perror((program));			\
		exit(EXIT_FAILURE); }	

#define CHECK_ALLOC(t, program) { 	 		 \
			if (t == NULL) {				 \
				perror(program);			 \
				exit(EXIT_FAILURE); 		 \
			}								 \
		}

