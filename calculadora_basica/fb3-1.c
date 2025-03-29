#include <stdio.h>
#include <stdlib.h>
#include "fb3-1.h"

/* Create a new AST node */
struct ast *newast(int nodetype, struct ast *l, struct ast *r) {
    struct ast *a = malloc(sizeof(struct ast));
    if (!a) {
        yyerror("Out of memory");
        exit(1);
    }
    a->nodetype = nodetype;
    a->l = l;
    a->r = r;
    return a;
}

/* Create a new number node */
struct ast *newnum(double d) {
    struct numval *a = malloc(sizeof(struct numval));
    if (!a) {
        yyerror("Out of memory");
        exit(1);
    }
    a->nodetype = 'K';
    a->number = d;
    return (struct ast *)a;
}

/* Evaluate an AST */
double eval(struct ast *a) {
    double v;

    switch (a->nodetype) {
        case 'K':
            v = ((struct numval *)a)->number;
            break;
        case '+':
            v = eval(a->l) + eval(a->r);
            break;
        case '-':
            v = eval(a->l) - eval(a->r);
            break;
        case '*':
            v = eval(a->l) * eval(a->r);
            break;
        case '/':
            v = eval(a->l) / eval(a->r);
            break;
        default:
            yyerror("Unknown node type: %c", a->nodetype);
            v = 0;
    }
    return v;
}

/* Free the AST */
void treefree(struct ast *a) {
    if (!a) return;

    if (a->nodetype != 'K') {
        treefree(a->l);
        treefree(a->r);
    }
    free(a);
}
