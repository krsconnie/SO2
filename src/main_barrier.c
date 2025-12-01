#include "reusable_barrier.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>  

// Valores por defecto
#define DEFAULT_N 5
#define DEFAULT_E 4

// Declaración de la barrera global
reusable_barrier_t barrera_global;

void *funcion_hebra(void *arg)
{
    // El id es el número de la hebra (0, 1, 2, ...)
    long id = (long)arg;
    int num_etapas = barrera_global.N; // El número de etapas se almacena temporalmente en el campo N
                                       // en la estructura para poder recuperarlo aquí (aunque no es el uso
                                       // original, es una forma sencilla de pasarlo). Vamos a asumir que
                                       // el número de etapas se pasa a través de una variable global o struct
                                       // si el diseño fuera más formal. Para este ejemplo, usaremos una variable
                                       // global auxiliar.

    // Como el número de etapas no se guarda en reusable_barrier_t, lo pasamos por la estructura
    // que contendrá los argumentos de main(). Para simplificar, usaremos la variable E_global.
    extern int E_global;
    num_etapas = E_global;

    for (int etapa = 1; etapa <= num_etapas; etapa++)
    {
        // 2. a) Realiza trabajo simulado (usleep con aleatorio)
        // Agregamos un sleep aleatorio para que la secuencia de llegadas varíe
        usleep(rand() % 200000); // 0 a 200 ms

        // 2. b) Imprime antes de esperar: [tid] esperando en etapa e.
        // Usamos el ID de la hebra como [tid]
        printf("[Hebra %ld] esperando en etapa %d.\n", id, etapa);

        // 2. c) Llama a wait()
        int es_ultima = reusable_barrier_wait(&barrera_global);

        // 2. d) Imprime después de pasar: [tid] paso barrera en etapa e.
        // La impresión después de la barrera garantiza la verificación del punto 3.
        printf("[Hebra %ld] paso barrera en etapa %d.\n", id, etapa);

        if (es_ultima)
        {
            // Mensaje opcional para verificar la hebra que libera
            printf("LIBERACIÓN Hebra %ld fue la última.\n", id);
        }

        // Simular más trabajo después de la barrera (para variar el tiempo antes de la siguiente etapa)
        usleep(rand() % 200000);
    }

    // printf("Hebra %ld: Finaliza.\n", id);
    return NULL;
}

// Variable global para almacenar el número de etapas, ya que no es parte de reusable_barrier_t
int E_global;

int main(int argc, char *argv[])
{
    int N = DEFAULT_N; // Número de hebras
    int E = DEFAULT_E; // Número de etapas
    int ret;

    // 1. Procesar parámetros de línea de comandos (N y E)
    if (argc > 1)
    {
        N = atoi(argv[1]);
        if (N <= 0)
            N = DEFAULT_N;
    }
    if (argc > 2)
    {
        E = atoi(argv[2]);
        if (E <= 0)
            E = DEFAULT_E;
    }

    E_global = E;

    // Inicializar el generador de números aleatorios
    srand(time(NULL));

    // Inicialización de la barrera (usa N como parámetro de cuenta)
    if ((ret = reusable_barrier_init(&barrera_global, N)) != 0)
    {
        fprintf(stderr, "Error al inicializar la barrera: %d\n", ret);
        return 1;
    }

    pthread_t hebras[N];

    printf("Iniciando Verificación\n");
    printf("N Hebras: %d, E Etapas: %d\n", N, E);

    // Creación de las hebras
    for (long i = 0; i < N; i++)
    {
        if (pthread_create(&hebras[i], NULL, funcion_hebra, (void *)i) != 0)
        {
            perror("Error al crear la hebra");
            // Manejo de error: destruir barrera y salir
            reusable_barrier_destroy(&barrera_global);
            return 1;
        }
    }

    // Esperar a que todas las hebras terminen
    for (int i = 0; i < N; i++)
    {
        pthread_join(hebras[i], NULL);
    }
    printf("Verificación completada: Todas las hebras han pasado por las %d etapas.\n", E);

    // Destrucción de la barrera
    if ((ret = reusable_barrier_destroy(&barrera_global)) != 0)
    {
        fprintf(stderr, "Error al destruir la barrera: %d\n", ret);
        return 1;
    }

    return 0;
}