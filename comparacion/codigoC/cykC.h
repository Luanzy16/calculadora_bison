#ifndef CYK_H
#define CYK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_RULES 50
#define MAX_SYMBOLS 10
#define MAX_LENGTH 100

typedef struct {
    char lhs;
    char rhs1;
    char rhs2;
} Rule;

void addRule(char lhs, char rhs1, char rhs2);
int cyk(char *input, int length);

#endif // CYK_H
