// utils.c

#include <stdio.h>
#include <stdlib.h>

/**
 * Prints an error message.
 * 
 * @param funcname The name of the function where the error occurred.
 * @param condition A description of what failed.
 * */
void fail(char funcname[], char condition[])
{
    fprintf(stderr, "Failure: %s - %s\n", funcname, condition);
}

void error(char message[], int code)
{
    fprintf(stderr, "*** Error: %s ***\n", message);
    exit(code);
}