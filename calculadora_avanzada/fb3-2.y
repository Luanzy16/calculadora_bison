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
%type <a> exp term factor

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
    | exp '+' term      { $$ = newast('+', $1, $3); }
    | exp '-' term      { $$ = newast('-', $1, $3); }
    | NAME '=' exp      { $$ = newasgn($1, $3); }
    ;

term:
      factor            { $$ = $1; }
    | term '*' factor   { $$ = newast('*', $1, $3); }
    | term '/' factor   { $$ = newast('/', $1, $3); }
    ;

factor:
      NUMBER            { $$ = newnum($1); }
    | NAME              { $$ = newref($1); }
    | '(' exp ')'       { $$ = $2; }
    ;

%%
int main() {
    printf("Enter an expression:\n");
    return yyparse();
}