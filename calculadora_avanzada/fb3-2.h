#ifndef FB3_2_H
#define FB3_2_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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


/* Funciones del AST */
struct ast *newast(int nodetype, struct ast *l, struct ast *r);
struct ast *newnum(double d);
struct ast *newref(struct symbol *s);
struct ast *newasgn(struct symbol *s, struct ast *v);
double eval(struct ast *);
void treefree(struct ast *);
void yyerror(const char *s);
struct ast *newfunc(int functype, struct ast *l);
struct symbol *lookup(char *sym);


#endif
