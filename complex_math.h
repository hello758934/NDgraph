#ifndef COMPLEX_MATH_H
#define COMPLEX_MATH_H

#include <stddef.h>

#define CM_MAX_DEGREE 10
#define CM_MAX_INPUT 96

typedef struct { double re, im; } CMComplex;
typedef struct {
    CMComplex roots[CM_MAX_DEGREE];
    int count;              /* Number of distinct plotted roots. */
    int degree;             /* Degree counting multiplicity. */
    int infinite_solutions; /* True for 0=0. */
} CMResult;

/* Supports z, i, +, -, *, /constant, ^integer, parentheses, and =.
   Implicit multiplication works for 2z, 3i, z(z+1), (z+1)(z-1).
   Returns 1 on success and 0 on failure; always writes a message. */
int cm_solve(const char *equation, CMResult *result, char *message, size_t message_size);

#endif
