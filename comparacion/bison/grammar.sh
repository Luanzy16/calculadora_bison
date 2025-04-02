bison -d grammar.y
flex lex.l
gcc grammar.tab.c lex.yy.c -o analizador
