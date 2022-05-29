#ifndef TLPI_HDR_H
#define TLPI_HDR_H     /* Prevent accidental double inclusion */
#include <errno.h>     /* Declares errno and defines error constants */
#include <stdio.h>     /* Standard I/O functions */
#include <stdlib.h>    /* Prototypes of commonly used library functions, */
#include <string.h>    /* Commonly used string-handling functions */
#include <sys/types.h> /* Type definitions used by many programs plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>    /* Prototypes for many system calls */

#include "get_num.h" /* Declares our functions for handling numeric arguments (getInt(), getLong()) */
#define min(m, n) ((m) < (n) ? (m) : (n))
#define max(m, n) ((m) > (n) ? (m) : (n))
#endif