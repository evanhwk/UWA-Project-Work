# Makefile for cfind

# Check for OS
ifeq ($(shell uname), Darwin)
	LIB = /cslinux/adhoc/lib/libstatexpr-osx.a
	LIB_HEADER_DIR = /cslinux/adhoc/include
else
	LIB = ./lib/libstatexpr-linux.a
	LIB_HEADER_DIR = ./lib
endif

PROJECT	= cfind
HEADERS	= init.h exec.h dirList.h
OBJ		= main.o init.o exec.o dirList.o

C99		= cc -std=c99
CFLAGS	= -Wall -pedantic -Werror -I$(LIB_HEADER_DIR)

## Link together all OBJ files
$(PROJECT) : $(OBJ)
		$(C99) $(CFLAGS) -o $(PROJECT) $(OBJ) $(LIB)
		
## Compile object.o files ##
%.o : %.c $(HEADERS)
		$(C99) $(CFLAGS) -c $<

## Remove all object files ##
clean:
	rm -f $(PROJECT) $(OBJ)