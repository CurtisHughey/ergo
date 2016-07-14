// Canabalized from http://c.learncodethehardway.org/book/ex20.html

#ifndef _DBG_H
#define _DBG_H

//#define DEBUG  // Uncomment to turn on debugging

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef DEBUG
#define DEBUG_PRINT(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__); fflush(stderr)
#else
#define DEBUG_PRINT(M, ...) do {} while (0);
#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define ERROR_PRINT(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__); fflush(stderr)

#define UNUSED(x) (void)(x)

#endif
