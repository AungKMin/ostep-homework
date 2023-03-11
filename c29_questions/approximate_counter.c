#define _GNU_SOURCE
#include "approximate_counter.h"
#include <sched.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>

// arguments for worker
typedef struct __Args {
    int loops;
    ApproxCounter* counter;
    // which cpu the thread should run on
    int cpu_index;
} Args;

// worker increments counter loops number of times
void* worker(void *arg) { 
    Args* args = (Args*) arg;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(args->cpu_index, &cpuset);
    Pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    for (int i = 0; i < args->loops; ++i) {
        counter_update(args->counter, args->cpu_index, 1);
    }
    return NULL;
}

int main (int argc, char *argv[]) { 
    if (argc < 5) { 
        printf("usage: %s <int: number of threads> <int: number of loops> <int: threshold for local counter to global counter transfer> <int: number of cpus>", argv[0]);
        exit(1);
    }

    int num_loops = strtol(argv[2], NULL, 10);
    int threshold = strtol(argv[3], NULL, 10);

    // create the threads
    int num_threads;
    num_threads = strtol(argv[1], NULL, 10);
    pthread_t threads[num_threads];

    struct timeval start, end;

    // start the timer
    int rc = gettimeofday(&start, NULL);
    if (rc != 0) { 
        fprintf(stderr, "gettimeofday failed\n");
        exit(1);
    }

    int num_cpus = strtol(argv[4], NULL, 10);
    
    // make a counter (note that the global and local values are handled within the counter)
    ApproxCounter* counter = (ApproxCounter*)malloc(sizeof(ApproxCounter));
    counter_init(counter, num_cpus, threshold);

    Args* args[num_threads];
    // start all the threads
    for (int i = 0; i < num_threads; ++i) { 
        // set up the argument to worker
        args[i] = (Args*)malloc(sizeof(Args));
        args[i]->loops = num_loops;
        args[i]->counter = counter;
        args[i]->cpu_index = i % num_cpus;
        Pthread_create(&threads[i], NULL, worker, args[i]);
    }

    // wait for all the threads to finish
    for (int i = 0; i < num_threads; ++i) {
        Pthread_join(threads[i], NULL);
    }

    rc = gettimeofday(&end, NULL);
    if (rc != 0) { 
        fprintf(stderr, "gettimeofday failed\n");
        exit(1);
    }

    long int total_microseconds = (end.tv_sec - start.tv_sec) * 1E6 + (end.tv_usec - start.tv_usec);

    // print duration in microseconds
    printf("%ld\n", total_microseconds);

    return 0;    
}