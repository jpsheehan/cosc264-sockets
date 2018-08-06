// utils.c

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

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

/**
 * Returns the current time string without a newline character
 * @return A pointer to the datetime string.
 * */
char* getCurrentTimeString()
{
    time_t rawtime;
    struct tm* info;
    time(&rawtime);
    info = localtime(&rawtime);
    char* str = asctime(info);
    str[strlen(str) - 1] = 0;
    return str;
}