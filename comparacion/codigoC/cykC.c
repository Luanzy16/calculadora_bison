#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include <stdbool.h>


#define MAX_LEN 100
#define MAX_RULES 10
#define MAX_NON_TERMINALS 10

// Estructura para almacenar reglas binarias
typedef struct {
    int lhs, rhs1, rhs2;
} BinaryRule;

// Estructura para almacenar reglas unitarias (terminales)
typedef struct {
    int lhs;
    char terminal;
} UnitRule;

// Estructura para el retroapuntador
typedef struct {
    int partition, left, right;
} BackPointer;

bool P[MAX_LEN][MAX_LEN][MAX_NON_TERMINALS];
BackPointer back[MAX_LEN][MAX_LEN][MAX_NON_TERMINALS];
BinaryRule binaryRules[MAX_RULES];
UnitRule unitRules[MAX_RULES];
int numBinaryRules = 0, numUnitRules = 0;
int startSymbol = 0; // R1 es el símbolo inicial

// Función para agregar una regla unitaria
void addUnitRule(int lhs, char terminal) {
    unitRules[numUnitRules].lhs = lhs;
    unitRules[numUnitRules].terminal = terminal;
    numUnitRules++;
}

// Función para agregar una regla binaria
void addBinaryRule(int lhs, int rhs1, int rhs2) {
    binaryRules[numBinaryRules].lhs = lhs;
    binaryRules[numBinaryRules].rhs1 = rhs1;
    binaryRules[numBinaryRules].rhs2 = rhs2;
    numBinaryRules++;
}

// Algoritmo CYK con retroapuntadores
bool cyk(char *input, int length) {
    memset(P, false, sizeof(P));
    memset(back, -1, sizeof(back));
    
    // Inicialización: llenar tabla con reglas unitarias
    for (int s = 0; s < length; s++) {
        for (int r = 0; r < numUnitRules; r++) {
            if (unitRules[r].terminal == input[s]) {
                P[1][s][unitRules[r].lhs] = true;
            }
        }
    }
    
    // Relleno de la tabla CYK
    for (int l = 2; l <= length; l++) { // Longitud del tramo
        for (int s = 0; s <= length - l; s++) { // Inicio del tramo
            for (int p = 1; p < l; p++) { // Partición del tramo
                for (int r = 0; r < numBinaryRules; r++) {
                    int A = binaryRules[r].lhs;
                    int B = binaryRules[r].rhs1;
                    int C = binaryRules[r].rhs2;
                    
                    if (P[p][s][B] && P[l - p][s + p][C]) {
                        P[l][s][A] = true;
                        back[l][s][A] = (BackPointer){p, B, C};
                    }
                }
            }
        }
    }
    
    return P[length][0][startSymbol];
}


int main() {
    FILE *input_file = fopen("input.txt", "r");
    FILE *output_file = fopen("datoscyk.csv", "w");

    if (!input_file || !output_file) {
        perror("Error al abrir archivos");
        return 1;
    }

    fprintf(output_file, "Cadena,Tiempo (s),Memoria (bytes),Resultado\n");

    // Definir la gramática (ejemplo para aceptar combinaciones de 'a' y 'b')
    addUnitRule(0, 'a'); // S → a
    addUnitRule(0, 'b'); // S → b
    addBinaryRule(0, 0, 0); // S → SS

    char input[MAX_LEN];
    while (fscanf(input_file, "%s", input) != EOF) {
        int length = strlen(input);

        struct rusage usage;
        clock_t start = clock();
        int resultado = cyk(input, length);
        clock_t end = clock();
        getrusage(RUSAGE_SELF, &usage);

        double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
        long memory_used = usage.ru_maxrss * 1024; 

        fprintf(output_file, "%s,%.6f,%ld,%s\n", input, time_taken, memory_used, resultado ? "ACEPTADA" : "RECHAZADA");
    }

    fclose(input_file);
    fclose(output_file);
    printf("Procesamiento completado. Revisa 'datoscyk.csv'.\n");
    return 0;
}
