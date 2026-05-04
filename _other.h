#ifndef OTHER_H
#define OTHER_H

#include <stdlib.h>
#include <stdio.h>

#define KASSERT(x) do { \
    if (x) { \
        printf("Assertion failed"); \
        exit(1); \
    } \
} while(0)

#ifndef __predict_true
#define __predict_true(exp)   __builtin_expect(!!(exp), 1)
#endif

#ifndef __predict_false
#define __predict_false(exp)  __builtin_expect(!!(exp), 0)
#endif

#define __packed        __attribute__((__packed__))

#endif