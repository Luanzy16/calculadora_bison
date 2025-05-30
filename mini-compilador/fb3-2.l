%{
    #include "fb3-2.h"
    #include "fb3-2.tab.h"
    extern int yylineno;
%}

%option noyywrap

/* float exponent */
EXP ([Ee][-+]?[0-9]+)

%%

">" { yylval.fn = 1; return CMP; }
"<" { yylval.fn = 2; return CMP; }
"<>" { yylval.fn = 3; return CMP; }
"==" { yylval.fn = 4; return CMP; }
">=" { yylval.fn = 5; return CMP; }
"<=" { yylval.fn = 6; return CMP; }

"if" { return IF; }
"then" { return THEN; }
"else" { return ELSE; }
"while" { return WHILE; }
"do" { return DO; }
"let" { return LET;}

"abs"  { yylval.fn = B_abs; return FUNC; }
"sqrt" { yylval.fn = B_sqrt; return FUNC; }
"exp" { yylval.fn = B_exp; return FUNC; }
"log" { yylval.fn = B_log; return FUNC; }
"print" { yylval.fn = B_print; return FUNC; }

[0-9]+(\.[0-9]*)?([eE][-+]?[0-9]+)? |
\.[0-9]+([eE][-+]?[0-9]+)? {
    yylval.d = atof(yytext);
    return NUMBER;
}

[a-zA-Z_][a-zA-Z0-9_]* { yylval.s = lookup(yytext); return NAME; }
[+\-*/=(){};,|]      { return yytext[0]; }
[ \t]              ;  /* Ignorar espacios en blanco */
\n                 { return EOL; }
.                  { printf("Unexpected character: %s\n", yytext); }

%%  
