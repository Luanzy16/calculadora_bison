/* Calculator using AST */
%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "fb3-1.h"
    #include "fb3-1.tab.h"

    extern YYSTYPE yylval;

    int yylex(void);
    void yyerror(const char *s, ...);
%}

%union {
    struct ast *a;
    double d;
}

%token <d> NUMBER
%token EOL

%type <a> exp term factor

%left '+' '-'
%left '*' '/'

%%

input:
    | input exp EOL {
        printf("Result: %f\n", eval($2));
        treefree($2);
    }
    ;

exp:
      term        { $$ = $1; }
    | exp '+' term { $$ = newast('+', $1, $3); }
    | exp '-' term { $$ = newast('-', $1, $3); }
    ;

term:
      factor      { $$ = $1; }
    | term '*' factor { $$ = newast('*', $1, $3); }
    | term '/' factor { $$ = newast('/', $1, $3); }
    ;

factor:
      NUMBER     { $$ = newnum($1); }
    ;

%%

void yyerror(const char *s, ...) {
    fprintf(stderr, "Error: %s\n", s);
}


int main() {
    printf("Enter an expression:\n");
    return yyparse();
}
