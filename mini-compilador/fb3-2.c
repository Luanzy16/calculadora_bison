#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fb3-2.h"

#define NSYMS 999
struct symbol symtab[NSYMS];

struct symbol *lookup(char *sym) {
    struct symbol *sp;

    for (sp = symtab; sp < &symtab[NSYMS]; sp++) {
        if (sp->name && strcmp(sp->name, sym) == 0)
            return sp;

        if (!sp->name) {
            sp->name = strdup(sym);
            sp->value = 0;
            sp->func = NULL;
            sp->syms = NULL;
            return sp;
        }
    }

    yyerror("Symbol table full");
    return NULL;
}


/* Crear un nodo AST binario */
struct ast *newast(int nodetype, struct ast *l, struct ast *r) {
    struct ast *a = malloc(sizeof(struct ast));
    if (!a) { yyerror("Sin memoria"); exit(1); }
    a->nodetype = nodetype;
    a->l = l;
    a->r = r;
    return a;
}

struct ast *newnum(double d) {
    struct numval *a = malloc(sizeof(struct numval));
    if (!a) { yyerror("Sin memoria"); exit(1); }
    a->nodetype = 'K';
    a->number = d;
    return (struct ast *)a;
}

struct ast *newref(struct symbol *s) {
    struct symref *a = malloc(sizeof(struct symref));
    if (!a) { yyerror("Sin memoria"); exit(1); }
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


struct ast *newcmp(int cmptype, struct ast *l, struct ast *r) {
    struct fncmp *a = malloc(sizeof(struct fncmp));
    if (!a) { yyerror("Sin memoria"); exit(1); }
    a->nodetype = '0';
    a->fn = cmptype;
    a->l = l;
    a->r = r;
    return (struct ast *)a;
}

struct ast *newflow(int nodetype, struct ast *cond, struct ast *tl, struct ast *el) {
    struct flow *a = malloc(sizeof(struct flow));
    if (!a) { yyerror("Sin memoria"); exit(1); }
    a->nodetype = nodetype;
    a->cond = cond;
    a->tl = tl;
    a->el = el;
    return (struct ast *)a;
}

struct ast *newcall(struct symbol *s, struct ast *args) {
    struct ufncall *a = malloc(sizeof(struct ufncall));
    if (!a) { yyerror("Sin memoria"); exit(1); }
    a->nodetype = 'C';
    a->s = s;
    a->l = args;
    return (struct ast *)a;
}

struct ast *newfunc(int functype, struct ast *args) {
    struct fncall *a = malloc(sizeof(struct fncall));
    if (!a) { yyerror("Sin memoria"); exit(1); }
    a->nodetype = 'F';
    a->functype = functype;
    a->l = args;
    return (struct ast *)a;
}

struct symlist *newsymlist(struct symbol *sym, struct symlist *next) {
    struct symlist *sl = malloc(sizeof(struct symlist));
    if (!sl) { yyerror("Sin memoria"); exit(1); }
    sl->sym = sym;
    sl->next = next;
    return sl;
}

void symlistfree(struct symlist *sl) {
    while (sl) {
        struct symlist *next = sl->next;
        free(sl);
        sl = next;
    }
}

void dodef(struct symbol *name, struct symlist *syms, struct ast *func) {
    name->syms = syms;
    name->func = func;
}

double eval(struct ast *a) {
    double v;

    if (!a) {
        yyerror("Nodo nulo");
        return 0.0;
    }

    switch (a->nodetype) {
        case 'K': v = ((struct numval *)a)->number; break;
        case 'N': v = ((struct symref *)a)->s->value; break;
        case '=': {
            v = eval(((struct symasgn *)a)->v);
            ((struct symasgn *)a)->s->value = v;
            break;
        }        
        case '+': v = eval(a->l) + eval(a->r); break;
        case '-': v = eval(a->l) - eval(a->r); break;
        case '*': v = eval(a->l) * eval(a->r); break;
        case '/': v = eval(a->l) / eval(a->r); break;
        case 'M': v = -eval(a->l); break;
        case 'L':
            eval(a->l);
            v = eval(a->r);
            break;
        case 'I': {
            struct flow *f = (struct flow *)a;
            if (eval(f->cond) != 0)
                v = eval(f->tl);
            else
                v = f->el ? eval(f->el) : 0.0;
            break;
        }
        case 'W': {
            struct flow *f = (struct flow *)a;
            v = 0.0;
            int count = 0;
            while (eval(f->cond) != 0) {
                v = eval(f->tl);
                if (++count > 10000) {
                    printf("¡Advertencia! Bucle infinito detectado\n");
                    break;
                }
            }
            break;
        }
        case 'F': v = callbuiltin((struct fncall *)a); break;
        case 'C': v = calluser((struct ufncall *)a); break;
        case '0': {
            struct fncmp *f = (struct fncmp *)a;
            double lv = eval(f->l);
            double rv = eval(f->r);
            switch (f->fn) {
                case 1: v = (lv > rv); break;
                case 2: v = (lv < rv); break;
                case 3: v = (lv != rv); break;
                case 4: v = (lv == rv); break;
                case 5: v = (lv >= rv); break;
                case 6: v = (lv <= rv); break;
                default:
                    yyerror("Comparación desconocida");
                    v = 0.0;
            }
            break;
        }
        default:
            yyerror("Nodo desconocido");
            v = 0.0;
    }

    return v;
}

void treefree(struct ast *a) {
    if (!a) return;

    switch (a->nodetype) {
        case '+': case '-': case '*': case '/': case 'L':
            treefree(a->r);
            treefree(a->l);
            break;

        case 'M': // Unary minus
            treefree(a->l);
            break;

        case 'K': // Constante
            free(a);
            break;

        case 'N': // Referencia a variable
            free((struct symref *)a);
            break;

        case '=': // Asignación
            treefree(((struct symasgn *)a)->v);
            free((struct symasgn *)a);
            break;

        case 'I': case 'W': { // Condicional o while
            struct flow *f = (struct flow *)a;
            treefree(f->cond);
            treefree(f->tl);
            treefree(f->el);
            free(f);
            break;
        }

        case 'F': // Función incorporada
            treefree(((struct fncall *)a)->l);
            free((struct fncall *)a);
            break;

        case 'C': // Función del usuario
            treefree(((struct ufncall *)a)->l);
            free((struct ufncall *)a);
            break;

        case '0': { // Comparación
            struct fncmp *f = (struct fncmp *)a;
            treefree(f->l);
            treefree(f->r);
            free(f);
            break;
        }

        default:
            fprintf(stderr, "treefree: nodo desconocido '%c'\n", a->nodetype);
            break;
    }
}


double callbuiltin(struct fncall *f) {
    double v = eval(f->l);

    switch (f->functype) {
        case B_sqrt: return sqrt(v);
        case B_exp: return exp(v);
        case B_log: return log(v);
        case B_abs: return fabs(v);
        case B_print: printf("= %4.4g\n", v); return v;
        default:
            yyerror("Función desconocida");
            return 0.0;
    }
}

double calluser(struct ufncall *f) {
    struct symbol *fn = f->s;

    if (!fn->func) {
        yyerror("Function '%s' undefined", fn->name);
        return 0;
    }

    struct symlist *sl;
    struct ast *args;
    double *oldval = NULL;
    double v;
    int nargs = 0;

    // Contar número de argumentos
    for (sl = fn->syms; sl; sl = sl->next)
        nargs++;

    oldval = malloc(nargs * sizeof(double));
    if (!oldval) {
        yyerror("Out of memory in function call");
        exit(1);
    }

    // Guardar valores anteriores y asignar nuevos
    args = f->l;
    sl = fn->syms;
    for (int i = 0; i < nargs; i++) {
        if (!sl) {
            yyerror("Too many arguments in call to '%s'", fn->name);
            free(oldval);
            return 0;
        }
        if (!args) {
            yyerror("Missing arguments in call to '%s'", fn->name);
            free(oldval);
            return 0;
        }

        if (sl->sym) {
            oldval[i] = sl->sym->value;
            sl->sym->value = eval(args);
        }

        args = args->r;  // siguiente en lista
        sl = sl->next;
    }

    v = eval(fn->func);

    // Restaurar valores anteriores
    sl = fn->syms;
    for (int i = 0; i < nargs; i++) {
        if (sl->sym)
            sl->sym->value = oldval[i];
        sl = sl->next;
    }

    free(oldval);
    return v;
}

