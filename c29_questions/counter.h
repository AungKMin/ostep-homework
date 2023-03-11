#ifndef COUNTER_H
#define COUNTER_H

#include "wrappers.h"

typedef struct __Counter { 
    int value;
    pthread_mutex_t lock;
} Counter;

void counter_init(Counter* counter) { 
    counter->value = 0;
    Pthread_mutex_init(&counter->lock, NULL);
}

void counter_increment(Counter* counter) { 
    Pthread_mutex_lock(&counter->lock);
    ++counter->value;
    Pthread_mutex_unlock(&counter->lock);
}

#endif // COUNTER_H