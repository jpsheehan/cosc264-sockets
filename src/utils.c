// utils.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

/**
 * Prints an error message then exits.
 * 
 * @param message The message to print.
 * @param code The exit code.
 * */
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

    // set the newline character to zero
    str[strlen(str) - 1] = 0;

    return str;
}

/**
 * Returns the largest value in the array.
 * 
 * @param nums The array to iterate through.
 * @param n The length of the array
 * */
int max(int nums[], int n)
{
    int largest;
    for (int i = 0; i < n; i++) {
        if (i == 0 || nums[i] > largest) {
            largest = nums[i];
        }
    }
    return largest;
}