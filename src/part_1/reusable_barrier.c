#include "reusable_barrier.h"
#include <stdio.h>

// Implementación de inicialización
int reusable_barrier_init(reusable_barrier_t *barrier, int count_N)
{
    if (count_N <= 0)
    {
        // La barrera requiere al menos una hebra
        return -1;
    }

    barrier->N = count_N;
    barrier->count = 0;
    barrier->etapa = 0;

    int ret;
    // Inicializar Mutex
    if ((ret = pthread_mutex_init(&barrier->mutex, NULL)) != 0)
    {
        return ret;
    }
    // Inicializar Variable de Condición
    if ((ret = pthread_cond_init(&barrier->cond, NULL)) != 0)
    {
        // Si falla la condición, se debe liberar el mutex ya inicializado
        pthread_mutex_destroy(&barrier->mutex);
        return ret;
    }

    return 0;
}

// Implementación de destrucción
int reusable_barrier_destroy(reusable_barrier_t *barrier)
{
    int ret;
    // Destruir la variable de condición
    if ((ret = pthread_cond_destroy(&barrier->cond)) != 0)
    {
        return ret;
    }
    // Destruir el mutex
    if ((ret = pthread_mutex_destroy(&barrier->mutex)) != 0)
    {
        return ret;
    }
    return 0;
}

// Implementación de espera (Wait)
int reusable_barrier_wait(reusable_barrier_t *barrier)
{
    // 5. Pistas de diseño: Lock -> modificar estado / decidir -> wait/broadcast -> unlock.

    pthread_mutex_lock(&barrier->mutex);

    // 5. Capturar la etapa actual en una variable local y esperar mientras no cambie.
    // Esto es crucial para la reutilización. Las hebras que acaban de despertar
    // deben esperar en el while(etapa_local == barrier->etapa) de la *siguiente* etapa.
    int etapa_local = barrier->etapa;

    barrier->count++; // Incrementar el contador de hebras que han llegado

    int es_ultima = (barrier->count == barrier->N);

    if (es_ultima)
    {
        // La última en llegar debe: (i) incrementar etapa, (ii) resetear count, (iii) broadcast
        // para despertar a todas, (iv) desbloquear.

        barrier->etapa++;
        barrier->count = 0;

        // Despertar a todas las hebras esperando en la variable de condición
        pthread_cond_broadcast(&barrier->cond);

        // La hebra final desbloquea y retorna
        pthread_mutex_unlock(&barrier->mutex);
        return 1; // Retorna 1 para indicar que fue la última
    }
    else
    {
        // Las N-1 hebras deben esperar.

        // El while() protege contra las activaciones espurias (spurious wakeups)
        // y es fundamental para la reutilización: espera hasta que la etapa actual
        // sea *diferente* a la etapa en la que entraron.
        while (etapa_local == barrier->etapa)
        {
            pthread_cond_wait(&barrier->cond, &barrier->mutex);
        }

        // La hebra desbloquea y retorna
        pthread_mutex_unlock(&barrier->mutex);
        return 0; // Retorna 0 para indicar que no fue la última
    }
}