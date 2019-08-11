#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    int i = 0;
    char* strArray[4][40];
    char writablestring[] = "The C Programming Language";
    char *token = strtok(writablestring, " ");


    while(token != NULL)
    {
		strArray[3][i] = malloc(strlen(token) + 1);
        strcpy(strArray[3][i], token);
        printf("[%s]\n", token);
        token = strtok(NULL, " ");
        i++;
    }
    return 0;
}