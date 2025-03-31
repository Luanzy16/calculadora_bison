#include "fb3-2.h"
#include <string.h>
#include <math.h>

#define NSYMS 100
#define M_PI 3.14159265358979323846
#define DEG_TO_RAD(x) ((x) * M_PI / 180.0)
static struct symbol symtab[NSYMS];

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

struct ast *newref(struct symbol *s) {
    struct symref *a = malloc(sizeof(struct symref));
    if (!a) {
        yyerror("Out of memory");
        exit(1);
    }
    a->nodetype = 'N';
    a->s = s;
    return (struct ast *)a;
}

struct ast *newasgn(struct symbol *s, struct ast *v) {
    struct symasgn *a = malloc(sizeof(struct symasgn));
    if (!a) {
        yyerror("Out of memory");
        exit(1);
    }
    a->nodetype = '=';
    a->s = s;
    a->v = v;
    return (struct ast *)a;
}

struct ast *newfunc(int functype, struct ast *l) {
    struct fncall *a = malloc(sizeof(struct fncall));
    if (!a) {
        yyerror("Out of memory");
        exit(1);
    }
    a->nodetype = 'F';
    a->functype = functype;
    a->l = l;
    return (struct ast *)a;
}


double eval(struct ast *a) {
    double v;
    switch (a->nodetype) {
        case 'K': v = ((struct numval *)a)->number; break;
        case '+': v = eval(a->l) + eval(a->r); break;
        case '-': v = eval(a->l) - eval(a->r); break;
        case '*': v = eval(a->l) * eval(a->r); break;
        case '/': 
            if (eval(a->r) == 0) {
                yyerror("Division by zero");
                return 0;
            }
            v = eval(a->l) / eval(a->r); 
            break;
        case 'N': v = ((struct symref *)a)->s->value; break;
        case '=': v = ((struct symasgn *)a)->s->value = eval(((struct symasgn *)a)->v); break;
        case 'F': {
            struct fncall *f = (struct fncall *)a;
            double operand = eval(f->l);
            switch (f->functype){
                case 'S': v = sqrt(operand); break;
                case 'L': v = log10(operand); break;
                case 's': v = sin(DEG_TO_RAD(operand)); break;
                case 'c': v = cos(DEG_TO_RAD(operand)); break;
                case 't': v = tan(DEG_TO_RAD(operand)); break;
                default: yyerror("funcion desconocida"); v = 0;
            }
            break;
        }
        default: yyerror("Unknown node type"); v = 0;
    }
    return v;
}

void treefree(struct ast *a) {
    if (!a) return;

    switch (a->nodetype) {
        case 'K': break;  
        case 'N': free(a); return;
        case '=': 
            treefree(((struct symasgn *)a)->v);
            free(a);
            return;
        default:
            treefree(a->l);
            treefree(a->r);
            break;
    }
    free(a);
}

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

struct symbol *lookup(char *sym) {
    struct symbol *sp;
    
    for (sp = symtab; sp < &symtab[NSYMS]; sp++) {
        if (sp->name && strcmp(sp->name, sym) == 0) 
            return sp;

        if (!sp->name) { 
            sp->name = strdup(sym);
            sp->value = 0; 
            return sp;
        }
    }

    yyerror("Symbol table full");
    return NULL;
}


