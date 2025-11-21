#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> // Necesario para usar true/false
#include "simulador.h"

int main(int argc, char *argv[]) {
    
    if (argc < 4) {
        fprintf(stderr, "Uso: %s <Nmarcos> <tam_marco> [--verbose] <traces.txt>\n", argv[0]);
        return 1;
    }

    int nMarcos = atoi(argv[1]);
    int tamPag = atoi(argv[2]);
    
    bool verbose = false; // En caso de no específicar 
    
    // Por defecto asumimos que el archivo es el 3er argumento
    char *filename = argv[3];

    // Si el tercer argumento es la bandera --verbose
    if (strcmp(argv[3], "--verbose") == 0) {
        verbose = true; // modo detallado
        
        // Verifica que tras la flag exista el archivo de traces
        if (argc < 5) {
            fprintf(stderr, "Error: falta el archivo de traces después de --verbose.\n");
            return 1;
        }

        filename = argv[4];
    }

    // Ejecución de la simulación
    init_simulacion(nMarcos, tamPag, verbose);
    process_trace_file(filename);
    imprimir();
    limpiar();

    return 0;
}