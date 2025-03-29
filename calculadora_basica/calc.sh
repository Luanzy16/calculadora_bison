flex fb3-1.l
bison -d fb3-1.y
gcc -o calc fb3-1.c fb3-1.tab.c lex.yy.c -lm
