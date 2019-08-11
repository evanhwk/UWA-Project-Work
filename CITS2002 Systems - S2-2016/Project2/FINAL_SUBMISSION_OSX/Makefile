# Makefile for cfind
LIB_SE = /cslinux/adhoc/lib/libstatexpr-osx.a
LIB_SE_HEADER_DIR = /cslinux/adhoc/include

PROJECT	= cfind
HEADERS	= $(PROJECT).h init.h exec.h dirVect.h
OBJ	= init.o exec.o dirVect.o

C99	= cc -std=c99
CFLAGS	= -Wall -pedantic -Werror -I$(LIB_SE_HEADER_DIR)

## Link together all OBJ files
$(PROJECT) : $(OBJ)
	$(C99) $(CFLAGS) -o $(PROJECT) $(OBJ) $(LIB_SE)		
## Compile object.o files ##
%.o : %.c $(HEADERS)
	$(C99) $(CFLAGS) -c $<

## Remove all object files ##
clean:
	rm -f $(PROJECT) $(OBJ)
