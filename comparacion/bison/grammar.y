%{
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>
#include <string.h>

int yylex();
void yyerror(const char *s);
extern void yy_scan_string(const char *str);
extern void yylex_destroy();

FILE *output_file;
clock_t start, end;
struct rusage usage;
%}

%token a b

%%

S : a 
  | b 
  | S S; /* Concatenación de dos subcadenas válidas */

%%

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s input.txt output.csv\n", argv[0]);
        return 1;
    }

    FILE *input_file = fopen(argv[1], "r");
    if (!input_file) {
        perror("Error abriendo input.txt");
        return 1;
    }

    output_file = fopen(argv[2], "w");
    if (!output_file) {
        perror("Error creando output.csv");
        return 1;
    }

    // Escribir encabezado en CSV
    fprintf(output_file, "Cadena,Tiempo (s),Memoria (bytes),Resultado\n");

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), input_file)) {
        buffer[strcspn(buffer, "\n")] = 0;  // Eliminar el salto de línea
        yy_scan_string(buffer);

        start = clock();
        int result = yyparse();
        end = clock();

        getrusage(RUSAGE_SELF, &usage);
        double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
        long memory_used = usage.ru_maxrss;

        // Guardar en CSV con formato correcto
        if (result == 0) {
            fprintf(output_file, "%s,%.6f,%ld,ACEPTADA\n", buffer, time_taken, memory_used);
        } else {
            fprintf(output_file, "%s,%.6f,%ld,RECHAZADA\n", buffer, time_taken, memory_used);
        }

        yylex_destroy();
    }

    fclose(input_file);
    fclose(output_file);

    printf("Análisis completado. Resultados guardados en %s\n", argv[2]);
    return 0;
}

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
