%{
    #include "fb3-2.h"
    #include <stdio.h>
    #include <stdlib.h>

    extern int yylex();
    void yyerror(const char *s);
%}

%union {
    struct ast *a;
    double d;
    struct symbol *s; 
}

%token <d> NUMBER
%token <s> NAME
%token EOL
%token SQRT LOG SIN COS TAN
%type <a> exp term factor func

%left '+' '-'
%left '*' '/'
%right '='  /* Asignaciones tienen menor precedencia */

%%
input:
      exp EOL { printf("Result: %f\n", eval($1)); treefree($1); }
    | input exp EOL { printf("Result: %f\n", eval($2)); treefree($2); }
    ;

exp:
      term              { $$ = $1; }
    | func              { $$ = $1; }
    | exp '+' term      { $$ = newast('+', $1, $3); }
    | exp '-' term      { $$ = newast('-', $1, $3); }
    | exp '+' func      { $$ = newast('+', $1, $3); }
    | exp '-' func      { $$ = newast('-', $1, $3); }
    | NAME '=' exp      { $$ = newasgn($1, $3); }
    ;

term:
      factor            { $$ = $1; }
    | term '*' factor   { $$ = newast('*', $1, $3); }
    | term '/' factor   { $$ = newast('/', $1, $3); }
    | term '*' func   { $$ = newast('*', $1, $3); }
    | term '/' func   { $$ = newast('/', $1, $3); }
    ;

factor:
      NUMBER            { $$ = newnum($1); }
    | NAME              { $$ = newref($1); }
    | '(' exp ')'       { $$ = $2; }
    ;

func: 
     SQRT '(' exp ')' { $$ = newfunc('S', $3); }
    | LOG '(' exp ')'  { $$ = newfunc('L', $3); }
    | SIN '(' exp ')'  { $$ = newfunc('s', $3); }
    | COS '(' exp ')'  { $$ = newfunc('c', $3); }
    | TAN '(' exp ')'  { $$ = newfunc('t', $3); }
    ;

%%
int main() {
    printf("Enter an expression:\n");
    return yyparse();
}
