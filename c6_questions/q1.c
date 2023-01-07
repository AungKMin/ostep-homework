#define _GNU_SOURCE
#include <sched.h>
#include <sys/time.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {

    // testing back-to-back gettimeofday calls: 
    struct timeval start, end;
    gettimeofday(&start, NULL);
    gettimeofday(&end, NULL);

    printf("start: %ld seconds; \t %ld microseconds\n", start.tv_sec, start.tv_usec);
    printf("end: %ld seconds; \t %ld microseconds\n", end.tv_sec, end.tv_usec);
    printf("duration: %ld seconds; \t\t  %ld microseconds\n", end.tv_sec - start.tv_sec, end.tv_usec - start.tv_usec);
    printf("\n");

    // measure system call
    const int NLOOPS = 1000000;
    int fd = open("./q1.txt", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
    gettimeofday(&start, NULL);
    for (size_t i = 0; i < NLOOPS; ++i) { 
        read(fd, NULL, 0);
    }
    gettimeofday(&end, NULL);
    printf("system call (0-character reads): %f microseconds\n\n", (float)(end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec) / NLOOPS);
    close(fd);

    // measure system call: reading larger stringtest
    fd = open("./q1_full.txt", O_RDONLY, S_IRWXU);
    gettimeofday(&start, NULL);
    const int string_len = 1000;
    char* s = (char*) malloc(string_len);
    for (size_t i = 0; i < NLOOPS; ++i) { 
        read(fd, s, string_len);
        // printf("%s", s);
    }
    gettimeofday(&end, NULL);
    printf("system call (%d-character reads): %f microseconds\n\n", string_len, (float)(end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec) / NLOOPS);
    close(fd);

    // measure context switch
    // -- measure time it takes for pipe reads/writes to run in just one process:
    int pipefd1[2];
    int pipefd2[2];
    if (pipe(pipefd1) == -1) { 
        fprintf(stderr, "pipe1");
        exit(1);
    }
    if (pipe(pipefd2) == -1) { 
        fprintf(stderr, "pipe2");
        exit(1);
    }
    gettimeofday(&start, NULL);
    for (int i = 0; i < NLOOPS; ++i) { 
        write(pipefd1[1], NULL, 0);
        read(pipefd1[0], NULL, 0);
        write(pipefd2[1], NULL, 0);
        read(pipefd2[0], NULL, 0);
    }
    gettimeofday(&end, NULL);
    int time_for_pipes_only = end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec;
    // -- 

    // -- measure total time (including pipe reads/writes)
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);

    int rc = fork();

    if (rc == -1) { 
        fprintf(stderr, "fork");
        exit(1);
    } else if (rc == 0) { 
        // child
        if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &set) == -1) { 
            fprintf(stderr, "setaffinity in child");
            exit(1); 
        }

        for (int i = 0; i < NLOOPS; ++i) { 
            // note that parent is Process A (this [child] is Process B)
            read(pipefd1[0], NULL, 0);
            write(pipefd2[1], NULL, 0);
        }
    } else  { 
        // parent
        if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &set) == -1) { 
            fprintf(stderr, "setaffinity in parent");
            exit(1);
        }

        gettimeofday(&start, NULL);
        for (int i = 0; i < NLOOPS; ++i) { 
            write(pipefd1[1], NULL, 0);
            read(pipefd2[0], NULL, 0);
        }
        gettimeofday(&end, NULL);
        int time_for_context_switch = end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec - time_for_pipes_only;
        printf("context switch: %f microseconds\n", (float)time_for_context_switch / NLOOPS);
        printf("context switch without subtracting pipes: %f microseconds\n", (float)(end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec) / NLOOPS);
        printf("pipe time in one processor: %f microseconds\n", (float)(time_for_pipes_only) / NLOOPS);
    }

}