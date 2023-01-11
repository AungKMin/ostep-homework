#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main (int argc, char *argv[]) { 
    if (argc != 3) { 
        fprintf(stderr, "usage: memory-user <memory> <time>\n");
        exit(1);
    }

    // note that a megabyte is 1024 kilobytes; a kilobyte is 1024 bytes
    int memory_in_bytes = atoi(argv[1]) * 1024 * 1024;
    int num_ints = (int)(memory_in_bytes / sizeof(int));
    int run_time = atoi(argv[2]);

    int *arr = malloc(memory_in_bytes);
    clock_t start = clock();
    
    while ((double)(clock() - start) / CLOCKS_PER_SEC < run_time)
    {
        for (int i = 0; i < num_ints; ++i) { 
            arr[i] += 1;
        }   
    }

    free(arr);

    return 0; 
}