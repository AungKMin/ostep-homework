#ifndef WRAPPERS_H
#define WRAPPERS_H

#define _GNU_SOURCE
#include <pthread.h>
#include <assert.h>

void Pthread_create(pthread_t* thread, const pthread_attr_t* attr, void *(*start_routine)(void *), void* arg) { 
    int rc = pthread_create(thread, attr, start_routine, arg);
    assert(rc == 0);
}

void Pthread_mutex_init(pthread_mutex_t* lock, pthread_mutexattr_t* attr) { 
    int rc = pthread_mutex_init(lock, attr);
    assert(rc == 0);
}

void Pthread_mutex_lock(pthread_mutex_t* lock) { 
    int rc = pthread_mutex_lock(lock);
    assert(rc == 0);
}

void Pthread_mutex_unlock(pthread_mutex_t* lock) { 
    int rc = pthread_mutex_unlock(lock);
    assert(rc == 0);
}

void Pthread_join(pthread_t thread, void** retval) { 
    int rc = pthread_join(thread, retval);
    assert(rc == 0);
}

void Pthread_setaffinity_np(pthread_t thread, size_t cpusetsize, const cpu_set_t* cpuset) { 
    int rc = pthread_setaffinity_np(thread, cpusetsize, cpuset);
    assert(rc == 0);
}

#endif // WRAPPERS_H