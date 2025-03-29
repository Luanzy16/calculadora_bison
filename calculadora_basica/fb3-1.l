%{
    #include "fb3-1.h"
    #include <stdio.h>
    #include "fb3-1.tab.h"
%}

%%

[0-9]+(\.[0-9]*)? {
    yylval.d = atof(yytext);
    return NUMBER;
}

[+\-*/]    return yytext[0];
\n         return EOL;
[ \t]      ; /* Ignore whitespace */

.          printf("Unexpected character: %s\n", yytext);

%%

int yywrap(void) {
    return 1;
}
