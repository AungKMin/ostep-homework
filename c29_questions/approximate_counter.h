#ifndef APPROX_COUNTER_H
#define APPROX_COUNTER_H

#include "wrappers.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct __ApproxCounter { 
    int global_value;
    int* local_values;
    pthread_mutex_t global_lock;
    pthread_mutex_t* local_locks;
    int threshold;
    int num_cpus;
} ApproxCounter;

void counter_init(ApproxCounter* counter, int num_cpus, int threshold) {
    counter->num_cpus = num_cpus;
    counter->global_value = 0;
    counter->local_values = (int*)malloc(sizeof(int) * num_cpus);
    Pthread_mutex_init(&counter->global_lock, NULL);
    counter->local_locks = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t) * num_cpus);
    for (int i = 0; i < num_cpus; ++i) { 
        Pthread_mutex_init(&counter->local_locks[i], NULL);
    }
    counter->threshold = threshold;
}

void counter_update(ApproxCounter* counter, int cpu_index, int amount) {
    assert(counter->num_cpus > cpu_index);
    Pthread_mutex_lock(&counter->local_locks[cpu_index]);
    counter->local_values[cpu_index] += amount;
    // if the threshold has been hit, transfer the value to the global value
    if (counter->local_values[cpu_index] >= counter->threshold ) { 
        Pthread_mutex_lock(&counter->global_lock);
        counter->global_value += counter->local_values[cpu_index];
        Pthread_mutex_unlock(&counter->global_lock);
        counter->local_values[cpu_index] = 0;
    }
    Pthread_mutex_unlock(&counter->local_locks[cpu_index]); 
}

#endif // APPROX_COUNTER_H