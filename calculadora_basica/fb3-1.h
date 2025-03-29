/*
 * Declarations for a calculator (fb3-1.h)
 */
#ifndef FB3_1_H
#define FB3_1_H

/* Interface to the lexer */
extern int yylineno;
void yyerror(const char *s, ...);

/* Nodes in the abstract syntax tree */
struct ast {
    int nodetype;
    struct ast *l;
    struct ast *r;
};

struct numval {
    int nodetype; /* Type 'K' for constant */
    double number;
};

/* Function prototypes */
struct ast *newast(int nodetype, struct ast *l, struct ast *r);
struct ast *newnum(double d);
double eval(struct ast *);
void treefree(struct ast *);

#endif
