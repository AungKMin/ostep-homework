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

4. Now, while running your memory-user program, also (in a different terminal window, but on the same machine) run the free
tool. How do the memory usage totals change when your program
is running? How about when you kill the memory-user program?
Do the numbers match your expectations? Try this for different
amounts of memory usage. What happens when you use really
large amounts of memory?

    Running `./memory-user 50 10`: resulted in memory used only going up by only ~10 Mi.

    ```
                total        used        free      shared     buffers       cache   available
    Mem:           24Gi       989Mi        22Gi       401Mi        25Mi       1.0Gi        23Gi
    Swap:         7.0Gi          0B       7.0Gi

                total        used        free      shared     buffers       cache   available
    Mem:           24Gi       1.0Gi        22Gi       401Mi        25Mi       1.0Gi        23Gi
    Swap:         7.0Gi          0B       7.0Gi
    ```

    But running `./memory-user 100 10`: resulted in memory used going up by the 100 Mi. So it probably is a precision issue.

    ```
                total        used        free      shared     buffers       cache   available
    Mem:           24Gi       1.1Gi        22Gi       401Mi        26Mi       1.1Gi        23Gi
    Swap:         7.0Gi          0B       7.0Gi

                total        used        free      shared     buffers       cache   available
    Mem:           24Gi       1.2Gi        22Gi       401Mi        26Mi       1.1Gi        23Gi
    Swap:         7.0Gi          0B       7.0Gi
    ```

    If we try to allocate too much memory, seg fault (on my computer, doesn't seg fault; just doesn't allocate any memory). 

5. Let’s try one more tool, known as pmap. Spend some time, and read
the pmap manual page in detail.

    ```
    man pmap
    ```

6.  To use pmap, you have to know the process ID of the process you’re
interested in. Thus, first run ps auxw to see a list of all processes;
then, pick an interesting one, such as a browser. You can also use
your memory-user program in this case (indeed, you can even
have that program call getpid() and print out its PID for your
convenience).

7. Now run pmap on some of these processes, using various flags (like
-X) to reveal many details about the process. What do you see?
How many different entities make up a modern address space, as
opposed to our simple conception of code/stack/heap?


    Note that pmap returns a text file, which we feed into `more` so that we can read the text file within the terminal. The `-x` option is to show more information. Note that most processes have a lot of entities making up the address space (chrome has a bunch of processes running and each one has a bunch of entities running). Note that since I'm using WSL, I can't see chrome with WSL Ubuntu (so can't run pmap on it).
    
    ```
    ps auxw
    ```

    ```
    sudo pmap <pid> -x | more
    ```

8. Finally, let’s run pmap on your memory-user program, with different amounts of used memory. What do you see here? Does the
output from pmap match your expectations?

    There are a lot more entities (mostly C library shared object files).

    ```
    sudo pmap 7719 -x | more
    7719:   ./memory-user 100 100
    Address           Kbytes     RSS   Dirty Mode  Mapping
    0000556f0f88d000       4       4       0 r---- memory-user
    0000556f0f88e000       4       4       0 r-x-- memory-user
    0000556f0f88f000       4       4       0 r---- memory-user
    0000556f0f890000       4       4       4 r---- memory-user
    0000556f0f891000       4       4       4 rw--- memory-user
    0000556f0fb3f000     132       4       4 rw---   [ anon ]
    00007f7256613000  102404  102404  102404 rw---   [ anon ]
    00007f725ca14000     148     144       0 r---- libc-2.31.so
    00007f725ca39000    1504     716       0 r-x-- libc-2.31.so
    00007f725cbb1000     296     124       0 r---- libc-2.31.so
    00007f725cbfb000       4       0       0 ----- libc-2.31.so
    00007f725cbfc000      12      12      12 r---- libc-2.31.so
    00007f725cbff000      12      12      12 rw--- libc-2.31.so
    00007f725cc02000      24      24      24 rw---   [ anon ]
    00007f725cc11000       4       4       0 r---- ld-2.31.so
    00007f725cc12000     140     140       0 r-x-- ld-2.31.so
    00007f725cc35000      32      32       0 r---- ld-2.31.so
    00007f725cc3e000       4       4       4 r---- ld-2.31.so
    00007f725cc3f000       4       4       4 rw--- ld-2.31.so
    00007f725cc40000       4       4       4 rw---   [ anon ]
    00007ffc574b6000     136      16      16 rw---   [ stack ]
    00007ffc575b9000      12       0       0 r----   [ anon ]
    00007ffc575bc000       4       4       0 r-x--   [ anon ]
    ---------------- ------- ------- ------- 
    total kB          104896  103668  102492`
    ```

