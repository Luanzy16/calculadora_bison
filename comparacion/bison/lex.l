%{
#include "grammar.tab.h"  
%}

%%

"a"     { return a; }
"b"     { return b; }
[\t\n]  { /* Ignorar espacios y saltos de línea */ }
.        { return yytext[0]; } /* Retorna cualquier otro carácter */

%%

int yywrap() {
    return 1;
}
