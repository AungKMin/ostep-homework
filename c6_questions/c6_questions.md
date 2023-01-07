1. Measure time (cost) of system call.
    
    Ans: For measuring system call time, just do a system call that doesn't take long to do(like reading a 0-character string from a file). Note that system calls that take long by their nature will take longer (for example, reading a long string from a file). 

    Notes while testing with reading more text:
    - after reading in the entire file, read will just give the last part of the text file (for example, the last three characters over and over again if you're reading three characters at a time).
    - reading more text doesn't take too much longer, but printing the text read takes much longer

    For measuring context switch, have two pipes and two processes. Make sure to set both processes to run on the same CPU/processor using `sched_setaffinity()`. Process A writes to pipe 1 and reads from pipe 2, which is empty. Since Process A is waiting to read from pipe 2, OS will switch to another process. Process B reads from pipe 1 and writes to pipe 2. The read from pipe 1 consumes the data written in Process A, and the write to pipe 2 unblocks Process A.

    Note that if Process B goes first, OS would block on Process B's read from pipe 1 instead. Since we repeat each iteration a fixed number of times, the overall time should be the same. But since we start the clock (by setting start) in Process A, if Process B does go first, we would undercount by one context switch.

    **This doesn't actually work (sometimes the actual time ends up being negative after subtracting): I think it's because when you wait on a read, the context switch happens immediately, meaning the read and the context switch happens simulatenously and so the pipe times don't actually add to the total time. Or it could just be that the pipe times are so insignificant that it ends up not mattering and whether the actual value is positive or negative is just due to random system noise.** Note that to measure only the time of the context switch and not include the time it takes for reading and writing from/to pipes, we measure the time it takes for the reads/writes to run in one process and subtract this time from the time we got above.

    ```c++
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
    ```

    ```sh
    // sometimes the context switch time result is positive
    ./q1
    start: 1673054445 seconds;       601247 microseconds
    end: 1673054445 seconds;         601247 microseconds
    duration: 0 seconds;              0 microseconds

    \system call (0-character reads): 0.046673 microseconds

    system call (1000-character reads): 0.091942 microseconds

    context switch: 0.004560 microseconds
    context switch without subtracting pipes: 0.219173 microseconds
    pipe time in one processor: 0.214613 microseconds
    ```

    ```sh
    // sometimes the context switch time result is negative
    ./q1
    start: 1673054511 seconds;       83011 microseconds
    end: 1673054511 seconds;         83011 microseconds
    duration: 0 seconds;              0 microseconds

    system call (0-character reads): 0.045812 microseconds

    system call (1000-character reads): 0.091120 microseconds

    context switch: -0.005511 microseconds
    context switch without subtracting pipes: 0.206730 microseconds
    pipe time in one processor: 0.212241 microseconds
    ```