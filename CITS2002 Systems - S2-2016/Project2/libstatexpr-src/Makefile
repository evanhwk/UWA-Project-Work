#
# libstatexpr: Chris.McDonald@uwa.edu.au
#
ifeq ($(shell uname), Darwin)
    LIB = ./libstatexpr-osx.a
else
    LIB = ./libstatexpr-linux.a
endif

CC	= cc -std=c99 -Wall -Werror -pedantic -I.

it:
	$(CC) -c parsedate.c
	$(CC) -c statexpr.c
	ar rc $(LIB) parsedate.o statexpr.o

