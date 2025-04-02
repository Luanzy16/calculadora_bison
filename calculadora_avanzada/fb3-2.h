#ifndef FB3_2_H
#define FB3_2_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*#define NHASH 9997
struct symbol symtab[NHASH];*/

/* Estructura del Árbol de Sintaxis Abstracta (AST) */
struct ast {
    int nodetype;
    struct ast *l;
    struct ast *r;
};

/* Estructura para nodos con valores numéricos */
struct numval {
    int nodetype;  /* 'K' indica un número */
    double number;
};

/* Estructura para variables */
struct symref {
    int nodetype;  /* 'N' indica una variable */
    struct symbol *s;
};

/* Estructura para asignación de variables */
struct symasgn {
    int nodetype;  /* '=' indica asignación */
    struct symbol *s;
    struct ast *v;
};

/* Tabla de símbolos */
struct symbol {
    char *name;
    double value;
    struct ast *func; /* Si es una función, apunta al AST */
    struct symlist *syms; /* Lista de parámetros de la función */
};

/* Lista de símbolos (parámetros de una función) */
struct symlist {
    struct symbol *sym;
    struct symlist *next;
};

struct fncall {
    int nodetype;  /* 'F' */
    int functype; 
    struct ast *l;
};

struct ufncall { /* user function */
    int nodetype; /* type C */
    struct ast *l; /* list of arguments */
    struct symbol *s;
    };

struct flow {
    int nodetype; /* type I or W */
    struct ast *cond; /* condition */
    struct ast *tl; /* then branch or do list */
    struct ast *el; /* optional else branch */
};


/* Funciones del AST */
struct ast *newast(int nodetype, struct ast *l, struct ast *r);
struct ast *newnum(double d);
struct ast *newcmp(int cmptype, struct ast *l, struct ast *r);
struct ast *newcall(struct symbol *s, struct ast *l);
struct ast *newref(struct symbol *s);
struct ast *newasgn(struct symbol *s, struct ast *v);
struct ast *newfunc(int functype, struct ast *l);
struct symbol *lookup(char *sym);
struct ast *newflow(int nodetype, struct ast *cond, struct ast *tl, struct ast *tr);
struct symlist *newsymlist(struct symbol *sym, struct symlist *next);

double eval(struct ast *);
void treefree(struct ast *);
void yyerror(const char *s, ...);
void symlistfree(struct symlist *sl);
void dodef(struct symbol *name, struct symlist *syms, struct ast *stmts);
double callbuiltin(struct fncall *f);
double calluser(struct ufncall *f);


enum bifs {
    B_sqrt = 1,
    B_exp,
    B_log,
    B_print
};

#endif
