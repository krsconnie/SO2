#ifndef REUSABLE_BARRIER_H
#define REUSABLE_BARRIER_H

#include <pthread.h>

// 2. Estado compartido mínimo recomendado
typedef struct
{
    int count; // Número de hebras que han llegado a la barrera en la etapa actual.
    int N;     // Número total de hebras que deben esperar (parámetro de inicialización).
    int etapa; // Identificador de etapa donde operan N hebras.

    // 2. Sincronización
    pthread_mutex_t mutex; // Para la exclusión mutua del estado interno.
    pthread_cond_t cond;   // Para que las hebras esperen hasta que lleguen todas.
} reusable_barrier_t;

// 3. Operaciones
/**
 * Crea e inicializa una barrera.
 * @param barrier Puntero a la estructura de la barrera.
 * @param count_N Número total de hebras que deben sincronizarse.
 * @return 0 si es exitoso, o un código de error de Pthreads.
 */
int reusable_barrier_init(reusable_barrier_t *barrier, int count_N);

/**
 * Destruye los recursos de la barrera.
 * @param barrier Puntero a la estructura de la barrera.
 * @return 0 si es exitoso, o un código de error de Pthreads.
 */
int reusable_barrier_destroy(reusable_barrier_t *barrier);

/**
 * Función principal para la barrera. Bloquea la hebra que la llama hasta que N hebras
 * hayan llamado a esta función en la etapa actual.
 * @param barrier Puntero a la estructura de la barrera.
 * @return 0 si es exitoso, 1 si la hebra que llama es la última en llegar (la que desbloquea).
 */
int reusable_barrier_wait(reusable_barrier_t *barrier);

#endif // REUSABLE_BARRIER_H