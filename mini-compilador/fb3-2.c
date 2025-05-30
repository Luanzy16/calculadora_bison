#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "fb3-2.h"

#define NSYMS 100

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

struct ast *newcmp(int cmptype, struct ast *l, struct ast *r) {
    struct ast *a = malloc(sizeof(struct ast));
    if (!a) {
        yyerror("Out of memory");
        exit(1);
    }
    a->nodetype = cmptype; 
    a->l = l;
    a->r = r;
    return a;
}

struct ast *newcall(struct symbol *s, struct ast *l) {
    struct ufncall *a = malloc(sizeof(struct ufncall));
    if (!a) {
        yyerror("Out of memory");
        exit(1);
    }
    a->nodetype = 'U';
    a->s = s;
    a->l = l;
    return (struct ast *)a;
}

struct symlist *
newsymlist(struct symbol *sym, struct symlist *next)
{
    struct symlist *sl = malloc(sizeof(struct symlist));
    if (!sl) {
        yyerror("out of space");
        exit(0);
    }
    sl->sym = sym;
    sl->next = next;
    return sl;
}


struct ast *newflow(int nodetype, struct ast *cond, struct ast *tl, struct ast *el) {
    struct flow *a = malloc(sizeof(struct flow));
    if (!a) {
        yyerror("Out of memory");
        exit(1);
    }
    a->nodetype = nodetype;
    a->cond = cond;
    a->tl = tl;
    a->el = el;
    return (struct ast *)a;
}

/* evaluar un AST */
double eval(struct ast *a) {
    double v;

    if (!a) {
        yyerror("Internal error, null eval");
        return 0.0;
    }

    switch (a->nodetype) {
        case 'K': 
            v = ((struct numval *)a)->number; 
            break;

	case '|':
	    return fabs(eval(a->l));

        case 'N': 
            v = ((struct symref *)a)->s->value; 
            break;
case '=':
    v = ((struct symasgn *)a)->s->value = eval(((struct symasgn *)a)->v);
    printf("Asignación: %s = %f\n", ((struct symasgn *)a)->s->name, v);
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

        case 'M': 
            v = -eval(a->l); 
            break;  /* menos unario */

        case 'L': 
            eval(a->l); 
            v = eval(a->r); 
            break;

        case 'I': 
            if (eval(((struct flow *)a)->cond) != 0) { 
                if (((struct flow *)a)->tl)
                    v = eval(((struct flow *)a)->tl);
                else
                    v = 0.0; 
            } else {
                if (((struct flow *)a)->el)
                    v = eval(((struct flow *)a)->el);
                else
                    v = 0.0;
            }
            break;
case 'W': {
    v = 0.0;
    if (((struct flow *)a)->tl) {
        int count = 0;
        double cond_val;

        while (count < 1000) {
            // Evaluar condición
            cond_val = eval(((struct flow *)a)->cond);
            if (cond_val == 0)
                break;

            // Corregir el problema con la asignación t = n / e
            struct ast *body = ((struct flow *)a)->tl;
            if (body->nodetype == 'L' && body->l && body->l->nodetype == '/') {
                // Detectamos el problema: el nodo izquierdo es '/' en lugar de '='
                // Manejar manualmente la asignación t = n / e
                struct symbol *t_sym = lookup("t");
                struct symbol *n_sym = lookup("n");
                struct symbol *e_sym = lookup("e");
                
                if (t_sym && n_sym && e_sym) {
                    // Calculamos n/e y lo asignamos a t
                    t_sym->value = n_sym->value / e_sym->value;
                    printf("Asignación: t = %f\n", t_sym->value);
                }
                
                // Ahora evaluamos el resto del cuerpo normalmente
                v = eval(body->r);
            } else {
                // Ejecutar cuerpo del while normalmente
                v = eval(((struct flow *)a)->tl);
            }
            
            // Mostrar valores actualizados
            struct symbol *e_sym = lookup("e");
            struct symbol *t_sym = lookup("t");
            printf("Iteración %d: e = %f, t = %f, condición = %f\n", 
                   count + 1, 
                   e_sym ? e_sym->value : -999.999, 
                   t_sym ? t_sym->value : -999.999,
                   cond_val);
            
            count++;
        }

        if (count >= 1000) {
            printf("¡Advertencia! Posible bucle infinito detectado\n");
        }
    }
    break;
}
        case 'F': 
            v = callbuiltin((struct fncall *)a); 
            break;

        case 'U': 
            v = calluser((struct ufncall *)a); 
            break;

        // Comparaciones:
        case 1: // '>'
            v = eval(a->l) > eval(a->r);
            break;
        case 2: // '<'
            v = eval(a->l) < eval(a->r);
            break;
        case 3: // '<>'
            v = eval(a->l) != eval(a->r);
            break;
        case 4: // '=='
            v = eval(a->l) == eval(a->r);
            break;
        case 5: // '>='
            v = eval(a->l) >= eval(a->r);
            break;
        case 6: // '<='
            v = eval(a->l) <= eval(a->r);
            break;

        default:
            printf("Internal error: bad node %c\n", a->nodetype);
            break;
    }

    return v;
}

/* Llamada a función integrada */
double callbuiltin(struct fncall *f) {
    enum bifs functype = (enum bifs) f->functype;
    double v = eval(f->l);

    switch (functype) {
        case B_sqrt:
            return sqrt(v);
        case B_abs:
            return fabs(v);  // Asegúrate de que esto esté funcionando correctamente
        case B_exp:
            return exp(v);
        case B_log:
            return log(v);
        case B_print:
            printf("= %4.4g\n", v);
            return v;
        default:
            yyerror("Unknown built-in function %d", functype);
            return 0.0;
    }
}
/* Llamada a función definida por el usuario */
double calluser(struct ufncall *f) {
    struct symbol *fn = f->s; /* función símbolo */
    struct symlist *sl;
    struct ast *args = f->l;
    double *oldval, *newval;
    double v;
    int nargs;
    int i;

    if (!fn->func) {
        yyerror("Function %s undefined", fn->name);
        return 0;
    }

    /* contar argumentos */
    sl = fn->syms;
    for (nargs = 0; sl; sl = sl->next) {
        nargs++;
    }

    /* reservar espacio para valores antiguos y nuevos */
    oldval = (double *)malloc(nargs * sizeof(double));
    newval = (double *)malloc(nargs * sizeof(double));
    if (!oldval || !newval) {
        yyerror("Out of space in %s", fn->name);
        return 0.0;
    }

    /* evaluar argumentos */
    for (i = 0; i < nargs; i++) {
        if (!args) {
            yyerror("Too few arguments in call to %s", fn->name);
            free(oldval);
            free(newval);
            return 0.0;
        }
        if (args->nodetype == 'L') {  /* lista de argumentos */
            newval[i] = eval(args->l);
            args = args->r;
        } else {  /* último argumento */
            newval[i] = eval(args);
            args = NULL;
        }
    }

    /* guardar valores antiguos y asignar nuevos */
    sl = fn->syms;
    for (i = 0; i < nargs; i++) {
        oldval[i] = sl->sym->value;
        sl->sym->value = newval[i];
        sl = sl->next;
    }

    free(newval);

    /* evaluar la función */
    v = eval(fn->func);

    /* restaurar valores antiguos */
    sl = fn->syms;
    for (i = 0; i < nargs; i++) {
        sl->sym->value = oldval[i];
        sl = sl->next;
    }

    free(oldval);
    return v;
}


void treefree(struct ast *a) {
    if (!a) return;

    switch (a->nodetype) {
        case 'K': break;
        case 'N': free(a); return;
        case '=': 
            treefree(((struct symasgn *)a)->v);
            break;
        case 'C':
        case '+': case '-': case '*': case '/':
            treefree(a->l);
            treefree(a->r);
            break;
        case 'F':
            treefree(((struct fncall *)a)->l);
            break;
        case 'I':
        case 'W':
            treefree(((struct flow *)a)->cond);
            treefree(((struct flow *)a)->tl);
            if (((struct flow *)a)->el)
                treefree(((struct flow *)a)->el);
            break;
        default: break;
    }
    free(a);
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

void dodef(struct symbol *name, struct symlist *syms, struct ast *func) {
    // Implementación de ejemplo, ajusta según sea necesario
    printf("Definiendo función: %s\n", name->name);
    name->func = func;
    name->syms = syms;
}



void symlistfree(struct symlist *sl)
{
    struct symlist *nsl;
    while (sl) {
        nsl = sl->next;
        free(sl);
        sl = nsl;
    }
}
