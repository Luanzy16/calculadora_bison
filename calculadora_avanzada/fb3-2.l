%{
    #include "fb3-2.h"
    #include "fb3-2.tab.h"
    extern int yylineno;
    void yyerror(const char *s);
%}

%option noyywrap

%%
[0-9]+(\.[0-9]*)?  { yylval.d = atof(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.s = lookup(yytext); return NAME; }
[+\-*/=(){};,]      { return yytext[0]; }
[ \t]              ;  /* Ignorar espacios en blanco */
\n                 { return EOL; }
.                  { printf("Unexpected character: %s\n", yytext); }

%%
