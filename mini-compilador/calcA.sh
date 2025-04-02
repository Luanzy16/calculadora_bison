flex fb3-2.l
bison -d fb3-2.y
gcc -o calc fb3-2.c fb3-2.tab.c lex.yy.c -lm
