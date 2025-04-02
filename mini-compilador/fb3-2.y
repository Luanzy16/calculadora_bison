%{
    #include "fb3-2.h"
    #include <stdio.h>
    #include <stdlib.h>

    extern int yylex();
    void yyerror(const char *s, ...);
%}

%union {
    struct ast *a;
    double d;
    struct symbol *s; 
    struct symlist *sl;
    int fn;
}

%token <d> NUMBER
%token <s> NAME
%token <fn> FUNC
%token EOL
%token IF THEN ELSE WHILE DO LET
%type <a> exp stmt list explist
%type <sl> symlist

%nonassoc <fn> CMP
%left '+' '-'
%left '*' '/'
%right '='  /* Asignaciones tienen menor precedencia */
%nonassoc '|' UMINUS

%start calclist
%%
calclist: 
      /* vacío */
    | calclist stmt EOL { printf("= %4.4g\n> ", eval($2)); treefree($2); }
    | calclist LET NAME '(' symlist ')' '=' list EOL {
        dodef($3, $5, $8);
        printf("Defined %s\n> ", $3->name);
    }
    | calclist error EOL { yyerrok; printf("> "); }
    ;

exp:
      exp CMP exp     { $$ = newcmp($2, $1, $3); }
    | exp '+' exp     { $$ = newast('+', $1, $3); }
    | exp '-' exp     { $$ = newast('-', $1, $3); }
    | exp '*' exp     { $$ = newast('*', $1, $3); }
    | exp '/' exp     { $$ = newast('/', $1, $3); }
    | '|' exp         { $$ = $2; }
    | '(' exp ')'     { $$ = $2; }
    | '-' exp %prec UMINUS { $$ = newast('M', $2, NULL); }
    | NUMBER          { $$ = newnum($1); }
    | NAME            { $$ = newref($1); }
    | NAME '=' exp    { $$ = newasgn($1, $3); }
    | FUNC '(' explist ')' { $$ = newfunc($1, $3); }
    | NAME '(' explist ')' { $$ = newcall($1, $3); }
    ;

explist: 
      exp { $$ = $1; }
    | exp ',' explist { $$ = newast('L', $1, $3); }
    ;

symlist: 
      NAME { $$ = newsymlist($1, NULL); }
    | NAME ',' symlist { $$ = newsymlist($1, $3); }
    ;

stmt:
      IF exp THEN list { $$ = newflow('I', $2, $4, NULL); }
    | IF exp THEN list ELSE list { $$ = newflow('I', $2, $4, $6); }
    | WHILE exp DO list { $$ = newflow('W', $2, $4, NULL); }
    | exp
    ;

list: 
      /* vacío */ { $$ = NULL; }
    | stmt ';' list { $$ = $3 == NULL ? $1 : newast('L', $1, $3); }
    ;

%%
int main() {
    printf("Enter an expression:\n");
    return yyparse();
}

void yyerror(const char *s, ...) {
    fprintf(stderr, "Error: %s\n", s);
}
