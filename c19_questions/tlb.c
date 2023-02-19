#include <stdio.h> // printf, fprintf
#include <unistd.h> // sysconf
#include <stdlib.h> // exit
#include <time.h> // clock_gettime

int main(int argc, char *argv[]) { 
    if (argc < 3) { 
        fprintf(stderr, "usage: %s <pages> <trials> \n", argv[0]);
        exit(1);
    }

    int numPages = atoi(argv[1]);
    // number of trials (number of tiimes that we repeat the experiment)
    int numTrials = atoi(argv[2]);

    printf("numPages: %d\n", numPages);
    printf("numTrials: %d\n", numTrials);

    long PAGESIZE = sysconf(_SC_PAGESIZE); // 4096
    // how many ints are in a page
    long JUMP = PAGESIZE / sizeof(int); // 4096 / 8 = 1024
       
    // get precision
    struct timespec t;
    if (clock_getres(CLOCK_PROCESS_CPUTIME_ID, &t) == -1) { 
        fprintf(stderr, "clock_getres failed\n");
        exit(1);
    }
    printf("Precision of clock: %ld seconds, %ld nanoseconds\n", t.tv_sec, t.tv_nsec); // 1 nanosecond

    // array of pages
    int *arr = (int*)calloc(numPages, PAGESIZE);

    // save start time
    struct timespec start;
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start) == -1) { 
        fprintf(stderr, "clock_gettime failed\n");
        exit(1);
    }

    // run trials
    for (int i = 0; i < numTrials; ++i) { 
        // note that although we allocated for an array of "PAGESIZE"-sized elements
        // , we are really accessing the memory block as if it was an array of ints 
        // since the variable type is int*
        for (int j = 0; j < numPages * JUMP; j += JUMP) { 
            arr[j] += 1;
        }
    }

    // save end time
    struct timespec end;
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end) == -1) { 
        fprintf(stderr, "clock_gettime failed\n");
        exit(1);
    }

    // average time over "numTrials" trials to make each access in nanoseconds
    printf("%f\n", ((end.tv_sec - start.tv_sec) * 1E9 + (end.tv_nsec - start.tv_nsec)) / (numTrials * numPages));
    free(arr); 

    return 0;
}


