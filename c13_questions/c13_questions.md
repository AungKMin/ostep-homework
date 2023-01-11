1. The first Linux tool you should check out is the very simple tool
free. First, type man free and read its entire manual page; it’s
short, don’t worry

    ``` 
    man free
    ```

2. Now, run free, perhaps using some of the arguments that might
be useful (e.g., -m, to display memory totals in megabytes). How
much memory is in your system? How much is free? Do these
numbers match your intuition?

    ```
    free -h -w

                total        used        free      shared     buffers       cache   available
    Mem:           24Gi       883Mi        22Gi       401Mi        27Mi       1.0Gi        23Gi
    Swap:         7.0Gi          0B       7.0Gi
    ```

3. Next, create a little program that uses a certain amount of memory,
called memory-user.c. This program should take one commandline argument: the number of megabytes of memory it will use.
When run, it should allocate an array, and constantly stream through
the array, touching each entry. The program should do this indefinitely, or, perhaps, for a certain amount of time also specified at the
command line.

    ```c++
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
    ```