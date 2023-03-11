#include <sys/time.h>
#include <stdio.h>

int main() {

    // testing back-to-back gettimeofday calls: 
    long int total_microseconds = 0;
    int tries = 1000000;

    for (int i = 0; i < tries; ++i) { 
        struct timeval start, end;
        gettimeofday(&start, NULL);
        gettimeofday(&end, NULL);

        total_microseconds += (end.tv_sec - start.tv_sec) * 1E6 + (end.tv_usec - start.tv_usec);
    }

    printf("average duration: %ld microseconds\n", total_microseconds / tries);

    return 0;
 }