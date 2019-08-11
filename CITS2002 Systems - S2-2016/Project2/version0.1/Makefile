# Makefile for cfind

# Check for OS
ifeq ($(shell uname), Darwin)
	LIB = /cslinux/adhoc/lib/libstatexpr-osx.a
else
	LIB = ./lib/libstatexpr-linux.a
endif

PROJECT	= cfind
HEADERS	= $(PROJECT).h
OBJ		= main.o init.o dir.o expr.o

C99		= cc -std=c99
CFLAGS	= -Wall -pedantic -Werror

## Link together all OBJ files and LIB ##
# PROJECT depends on: OBJ.o
# cc -std=c99 -Wall -pedantic -Werror -o program A.o B.o C.o./lib/libfile.a

$(PROJECT) : $(OBJ)
		$(C99) $(CFLAGS) -o $(PROJECT) $(OBJ) $(LIB)
		
## Compile object.o ##
# OBJECT depends on: SOURCE.c and HEADER.h
# cc -std=c99 -Wall -pedantic -Werror -c A.c
# NOTE: $< = The name of the first dependency. (%.c)

%.o : %.c $(HEADERS)
		$(C99) $(CFLAGS) -c $<

## Remove all object files ##
clean:
	rm -f $(PROJECT) $(OBJ)
