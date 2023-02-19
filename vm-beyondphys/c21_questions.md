1. First, open two separate terminal connections to the same machine, so that
you can easily run something in one window and the other.
Now, in one window, run vmstat 1, which shows statistics about machine
usage every second. Read the man page, the associated README, and any
other information you need so that you can understand its output. Leave
this window running vmstat for the rest of the exercises below.
Now, we will run the program mem.c but with very little memory usage.
This can be accomplished by typing ./mem 1 (which uses only 1 MB of
memory). How do the CPU usage statistics change when running mem? Do
the numbers in the user time column make sense? How does this change
when running more than one instance of mem at once?

    - `user time (us)` increases (`idle time (id)` is also supposed to decrease but actually increases for me)
    - `number of processes waiting for run time: r` is now 1
    - running `./mem 1 & ./mem 1 &` -> runs mem twice: 
      - `number of processes waiting for run time: r` is now 2 
      - `user time (us)` increases (`idle time (id)` is also supposed to decrease but actually increases for me)

2. Let’s now start looking at some of the memory statistics while running mem.
We’ll focus on two columns: swpd (the amount of virtual memory used) and
free (the amount of idle memory). Run ./mem 1024 (which allocates 1024
MB) and watch how these values change. Then kill the running program
(by typing control-c) and watch again how the values change. What do you
notice about the values? In particular, how does the free column change
when the program exits? Does the a
    - The `free` column decreases by about 1024 MB when the program starts running (as expected).

3. We’ll next look at the swap columns (si and so), which indicate how much
swapping is taking place to and from the disk. Of course, to activate these,
you’ll need to run mem with large amounts of memory. First, examine how
much free memory is on your Linux system (for example, by typing cat
/proc/meminfo; type man proc for details on the /proc file system and
the types of information you can find there). One of the first entries in
/proc/meminfo is the total amount of memory in your system. Let’s assume it’s something like 8 GB of memory; if so, start by running mem 4000
(about 4 GB) and watching the swap in/out columns. Do they ever give
non-zero values? Then, try with 5000, 6000, etc. What happens to these
values as the program enters the second loop (and beyond), as compared to
the first loop? How much data (total) are swapped in and out during the
second, third, and subsequent loops? (do the numbers make sense?)
   - Subsequent loops are faster since more of the pages are in cache / memory.
   - At about 24000 KB, `swap in (si)` and `swap out (so)` become non-zero. 
     - ```
        ./mem 24000
        allocating 25165824000 bytes (24000.00 MB)
        number of integers in array: 6291456000
        loop 0 in 7958.34 ms (bandwidth: 3015.71 MB/s)
        loop 1 in 2803.83 ms (bandwidth: 8559.72 MB/s)
        loop 2 in 2794.55 ms (bandwidth: 8588.15 MB/s)
        loop 3 in 3045.86 ms (bandwidth: 7879.55 MB/s)
        loop 4 in 2642.51 ms (bandwidth: 9082.27 MB/s)
        loop 5 in 2883.88 ms (bandwidth: 8322.11 MB/s)
        ``` 

1. Do the same experiments as above, but now watch the other statistics (such
as CPU utilization, and block I/O statistics). How do they change when
mem is running?
   - `blocks-in (bi)` and `blocks-out (bo)` also become non-zero.
   - `user time (us)` increases but is about 10x lower than the values it had when running just `./mem 1`
     - this is probably since less of the time is spent by the CPU processing, and more time is spent swapping from disk. 

5. Now let’s examine performance. Pick an input for mem that comfortably
fits in memory (say 4000 if the amount of memory on the system is 8 GB).
How long does loop 0 take (and subsequent loops 1, 2, etc.)? Now pick a size
comfortably beyond the size of memory (say 12000 again assuming 8 GB of
OPERATING
SYSTEMS
[VERSION 1.00] WWW.OSTEP.ORG
BEYOND PHYSICAL MEMORY: MECHANISMS 11
memory). How long do the loops take here? How do the bandwidth numbers compare? How different is performance when constantly swapping
versus fitting everything comfortably in memory? Can you make a graph,
with the size of memory used by mem on the x-axis, and the bandwidth of
accessing said memory on the y-axis? Finally, how does the performance of
the first loop compare to that of subsequent loops, for both the case where
everything fits in memory and where it doesn’t?
   - Swapping results in lower bandwidth. 
   - ```
        ./mem 1024
        allocating 1073741824 bytes (1024.00 MB)
        number of integers in array: 268435456
        loop 0 in 260.54 ms (bandwidth: 3930.36 MB/s)
        loop 2 in 108.09 ms (bandwidth: 9473.51 MB/s)
        loop 4 in 105.94 ms (bandwidth: 9665.95 MB/s)
        loop 6 in 105.45 ms (bandwidth: 9710.77 MB/s)
        loop 8 in 110.59 ms (bandwidth: 9259.51 MB/s)
        loop 10 in 115.77 ms (bandwidth: 8845.04 MB/s)
        ```
    - ```
        ./mem 24000
        allocating 25165824000 bytes (24000.00 MB)
        number of integers in array: 6291456000
        loop 0 in 7958.34 ms (bandwidth: 3015.71 MB/s)
        loop 1 in 2803.83 ms (bandwidth: 8559.72 MB/s)
        loop 2 in 2794.55 ms (bandwidth: 8588.15 MB/s)
        loop 3 in 3045.86 ms (bandwidth: 7879.55 MB/s)
        loop 4 in 2642.51 ms (bandwidth: 9082.27 MB/s)
        loop 5 in 2883.88 ms (bandwidth: 8322.11 MB/s)
        loop 6 in 3048.97 ms (bandwidth: 7871.51 MB/s)
     ```
    - For both cases, the first loop has a lower bandwidth than the subsequent loops since the first loop puts the pages in cache / memory.

6. Swap space isn’t infinite. You can use the tool swapon with the -s flag to
see how much swap space is available. What happens if you try to run mem
with increasingly large values, beyond what seems to be available in swap?
At what point does the memory allocation fail?

   - ```
        swapon -s
        Filename                                Type            Size    Used    Priority
        /swap/file                              file            7340032 396032  -2
    ```
   - Somewhere between 30000 (30GB) and 31000 (31GB), the process is killed. Note that a little less than 23GB of memory is available, and a little more than 7GB of swap space is available. Seems like that the limit is memory + swap space availability. 
   - ```
        ./mem 31000
        allocating 32505856000 bytes (31000.00 MB)
        number of integers in array: 8126464000
        Killed
    ```
   - ```
        ./mem 30000
        allocating 31457280000 bytes (30000.00 MB)
        number of integers in array: 7864320000
        loop 0 in 17203.54 ms (bandwidth: 1743.83 MB/s)
    ```
7. Finally, if you’re advanced, you can configure your system to use different
swap devices using swapon and swapoff. Read the man pages for details.
If you have access to different hardware, see how the performance of swapping changes when swapping to a classic hard drive, a flash-based SSD, and
even a RAID array. How much can swapping performance be improved via
newer devices? How close can you get to in-memory performance?
