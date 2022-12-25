1. Run process-run.py with the following flags: -l 5:100,5:100.
What should the CPU utilization be (e.g., the percent of time the
CPU is in use?) Why do you know this? Use the -c and -p flags to
see if you were right.

        Ans: 100% since no IO

    ``` sh
    python3 process-run.py -l 5:100,5:100 -c -p
    Time        PID: 0        PID: 1           CPU           IOs
    1        RUN:cpu         READY             1          
    2        RUN:cpu         READY             1          
    3        RUN:cpu         READY             1          
    4        RUN:cpu         READY             1          
    5        RUN:cpu         READY             1          
    6           DONE       RUN:cpu             1          
    7           DONE       RUN:cpu             1          
    8           DONE       RUN:cpu             1          
    9           DONE       RUN:cpu             1          
    10           DONE       RUN:cpu            1          

    Stats: Total Time 10
    Stats: CPU Busy 10 (100.00%)
    Stats: IO Busy  0 (0.00%)
    ```

2. Now run with these flags: ./process-run.py -l 4:100,1:0.
These flags specify one process with 4 instructions (all to use the
CPU), and one that simply issues an I/O and waits for it to be done.
How long does it take to complete both processes? Use -c and -p
to find out if you were right.

        Ans: 4 instructions for p0 + (1 cycle for RUN:io + 5 cycles for IO + 1 cycle for RUN:io_done) for p1 = 11 cycles

    ```sh
    python3 process-run.py -l 4:100,1:0 -c -p
    Time        PID: 0        PID: 1           CPU           IOs
    1        RUN:cpu         READY             1          
    2        RUN:cpu         READY             1          
    3        RUN:cpu         READY             1          
    4        RUN:cpu         READY             1          
    5           DONE        RUN:io             1          
    6           DONE       BLOCKED                           1
    7           DONE       BLOCKED                           1
    8           DONE       BLOCKED                           1
    9           DONE       BLOCKED                           1
    10           DONE      BLOCKED                           1
    11*          DONE   RUN:io_done             1          

    Stats: Total Time 11
    Stats: CPU Busy 6 (54.55%)
    Stats: IO Busy  5 (45.45%)
    ```

3. Switch the order of the processes: -l 1:0,4:100. What happens
now? Does switching the order matter? Why? (As always, use -c
and -p to see if you were right)

        Ans: It does matter, since now, CPU can do p1 while p0 is blocked by IO. So takes only 11 - (4 cycles of p1 that can happen concurrently with p2's IO cycles) = 7 cycles  

    ``` sh
    python3 process-run.py -l 1:0,4:100 -c -p
    Time        PID: 0        PID: 1           CPU           IOs
    1         RUN:io         READY             1          
    2        BLOCKED       RUN:cpu             1             1
    3        BLOCKED       RUN:cpu             1             1
    4        BLOCKED       RUN:cpu             1             1
    5        BLOCKED       RUN:cpu             1             1
    6        BLOCKED          DONE                           1
    7*   RUN:io_done          DONE             1          

    Stats: Total Time 7
    Stats: CPU Busy 6 (85.71%)
    Stats: IO Busy  5 (71.43%)
    ```

4. We’ll now explore some of the other flags. One important flag is
-S, which determines how the system reacts when a process issues an I/O. With the flag set to SWITCH ON END, the system
will NOT switch to another process while one is doing I/O, instead waiting until the process is completely finished. What happens when you run the following two processes (-l 1:0,4:100
-c -S SWITCH ON END), one doing I/O and the other doing CPU
work?

        Ans: With SWITCH_ON_END, we can't do the CPU and IO concurrently, which results in higher latency (11).

    ``` sh
    python3 process-run.py -l 1:0,4:100 -c -p -S SWITCH_ON_END
    Time        PID: 0        PID: 1           CPU           IOs
    1         RUN:io         READY             1          
    2        BLOCKED         READY                           1
    3        BLOCKED         READY                           1
    4        BLOCKED         READY                           1
    5        BLOCKED         READY                           1
    6        BLOCKED         READY                           1
    7*   RUN:io_done         READY             1          
    8           DONE       RUN:cpu             1          
    9           DONE       RUN:cpu             1          
    10           DONE       RUN:cpu             1          
    11           DONE       RUN:cpu             1          

    Stats: Total Time 11
    Stats: CPU Busy 6 (54.55%)
    Stats: IO Busy  5 (45.45%)
    ```

5. Now, run the same processes, but with the switching behavior set
to switch to another process whenever one is WAITING for I/O (-l
1:0,4:100 -c -S SWITCH ON IO). What happens now? Use -c
and -p to confirm that you are right.

        Ans: With SWITCH_ON_IO, we do the CPU and IO concurrently, which results in lower overall latency (7).

    ```sh
    python3 process-run.py -l 1:0,4:100 -c -p -S SWITCH_ON_IO
    Time        PID: 0        PID: 1           CPU           IOs
    1         RUN:io         READY             1          
    2        BLOCKED       RUN:cpu             1             1
    3        BLOCKED       RUN:cpu             1             1
    4        BLOCKED       RUN:cpu             1             1
    5        BLOCKED       RUN:cpu             1             1
    6        BLOCKED          DONE                           1
    7*   RUN:io_done          DONE             1          

    Stats: Total Time 7
    Stats: CPU Busy 6 (85.71%)
    Stats: IO Busy  5 (71.43%)
    ```

6.  One other important behavior is what to do when an I/O completes. With -I IO RUN LATER, when an I/O completes, the process that issued it is not necessarily run right away; rather, whatever
was running at the time keeps running. What happens when you
run this combination of processes? (Run ./process-run.py -l
3:0,5:100,5:100,5:100 -S SWITCH ON IO -I IO RUN LATER
-c -p) Are system resources being effectively utilized?

        Ans: With RUN_LATER, the process with IO actions wait until the other processes finish before it starts the next IO action. So, we waste time by not running IO and CPU actions concurrently.
        
    ```sh
    python3 process-run.py -l 3:0,5:100,5:100 -S SWITCH_ON_IO -I IO_RUN_LATER -c -p
    Time        PID: 0        PID: 1        PID: 2           CPU           IOs
    1         RUN:io         READY         READY             1          
    2        BLOCKED       RUN:cpu         READY             1             1
    3        BLOCKED       RUN:cpu         READY             1             1
    4        BLOCKED       RUN:cpu         READY             1             1
    5        BLOCKED       RUN:cpu         READY             1             1
    6        BLOCKED       RUN:cpu         READY             1             1
    7*         READY          DONE       RUN:cpu             1          
    8          READY          DONE       RUN:cpu             1          
    9          READY          DONE       RUN:cpu             1          
    10          READY          DONE       RUN:cpu             1          
    11          READY          DONE       RUN:cpu             1          
    12    RUN:io_done          DONE          DONE             1          
    13         RUN:io          DONE          DONE             1          
    14        BLOCKED          DONE          DONE                           1
    15        BLOCKED          DONE          DONE                           1
    16        BLOCKED          DONE          DONE                           1
    17        BLOCKED          DONE          DONE                           1
    18        BLOCKED          DONE          DONE                           1
    19*   RUN:io_done          DONE          DONE             1          
    20         RUN:io          DONE          DONE             1          
    21        BLOCKED          DONE          DONE                           1
    22        BLOCKED          DONE          DONE                           1
    23        BLOCKED          DONE          DONE                           1
    24        BLOCKED          DONE          DONE                           1
    25        BLOCKED          DONE          DONE                           1
    26*   RUN:io_done          DONE          DONE             1          

    Stats: Total Time 26
    Stats: CPU Busy 16 (61.54%)
    Stats: IO Busy  15 (57.69%)
    ```

7. Now run the same processes, but with -I IO RUN IMMEDIATE set,
which immediately runs the process that issued the I/O. How does
this behavior differ? Why might running a process that just completed an I/O again be a good idea?

        Ans: We do the IO next actions immediately, which incrases the opportunity for us to concurrently perform CPU and IO actions.

    ```sh
    python3 process-run.py -l 3:0,5:100,5:100 -S SWITCH_ON_IO -I IO_RUN_IMMEDIATE -c -p
    Time        PID: 0        PID: 1        PID: 2           CPU           IOs
    1         RUN:io         READY         READY             1          
    2        BLOCKED       RUN:cpu         READY             1             1
    3        BLOCKED       RUN:cpu         READY             1             1
    4        BLOCKED       RUN:cpu         READY             1             1
    5        BLOCKED       RUN:cpu         READY             1             1
    6        BLOCKED       RUN:cpu         READY             1             1
    7*   RUN:io_done          DONE         READY             1          
    8         RUN:io          DONE         READY             1          
    9        BLOCKED          DONE       RUN:cpu             1             1
    10        BLOCKED          DONE       RUN:cpu             1             1
    11        BLOCKED          DONE       RUN:cpu             1             1
    12        BLOCKED          DONE       RUN:cpu             1             1
    13        BLOCKED          DONE       RUN:cpu             1             1
    14*   RUN:io_done          DONE          DONE             1          
    15         RUN:io          DONE          DONE             1          
    16        BLOCKED          DONE          DONE                           1
    17        BLOCKED          DONE          DONE                           1
    18        BLOCKED          DONE          DONE                           1
    19        BLOCKED          DONE          DONE                           1
    20        BLOCKED          DONE          DONE                           1
    21*   RUN:io_done          DONE          DONE             1          

    Stats: Total Time 21
    Stats: CPU Busy 16 (76.19%)
    Stats: IO Busy  15 (71.43%)
    ```

8. Now run with some randomly generated processes: -s 1 -l 3:50,3:50
or -s 2 -l 3:50,3:50 or -s 3 -l 3:50,3:50. See if you can
predict how the trace will turn out. What happens when you use
the flag -I IO RUN IMMEDIATE vs. -I IO RUN LATER? What happens when you use -S SWITCH ON IO vs. -S SWITCH ON END?

        seed 1: 

    ```sh 
    python3 process-run.py -s 1 -l 3:50,3:50
    Produce a trace of what would happen when you run these processes:
    Process 0
    cpu
    io
    io_done
    io
    io_done

    Process 1
    cpu
    cpu
    cpu

    Important behaviors:
    System will switch when the current process is FINISHED or ISSUES AN IO
    After IOs, the process issuing the IO will run LATER (when it is its turn)
    ```

    ```sh
    python3 process-run.py -s 1 -l 3:50,3:50 -I IO_RUN_IMMEDIATE -S SWITCH_ON_IO -c -p
    Time        PID: 0        PID: 1           CPU           IOs
    1        RUN:cpu         READY             1          
    2         RUN:io         READY             1          
    3        BLOCKED       RUN:cpu             1             1
    4        BLOCKED       RUN:cpu             1             1
    5        BLOCKED       RUN:cpu             1             1
    6        BLOCKED          DONE                           1
    7        BLOCKED          DONE                           1
    8*   RUN:io_done          DONE             1          
    9         RUN:io          DONE             1          
    10        BLOCKED          DONE                           1
    11        BLOCKED          DONE                           1
    12        BLOCKED          DONE                           1
    13        BLOCKED          DONE                           1
    14        BLOCKED          DONE                           1
    15*   RUN:io_done          DONE             1          

    Stats: Total Time 15
    Stats: CPU Busy 8 (53.33%)
    Stats: IO Busy  10 (66.67%)
    ```

    ```sh
    python3 process-run.py -s 1 -l 3:50,3:50 -I IO_RUN_IMMEDIATE -S SWITCH_ON_END -c -p
    Time        PID: 0        PID: 1           CPU           IOs
    1        RUN:cpu         READY             1          
    2         RUN:io         READY             1          
    3        BLOCKED         READY                           1
    4        BLOCKED         READY                           1
    5        BLOCKED         READY                           1
    6        BLOCKED         READY                           1
    7        BLOCKED         READY                           1
    8*   RUN:io_done         READY             1          
    9         RUN:io         READY             1          
    10        BLOCKED         READY                           1
    11        BLOCKED         READY                           1
    12        BLOCKED         READY                           1
    13        BLOCKED         READY                           1
    14        BLOCKED         READY                           1
    15*   RUN:io_done         READY             1          
    16           DONE       RUN:cpu             1          
    17           DONE       RUN:cpu             1          
    18           DONE       RUN:cpu             1          

    Stats: Total Time 18
    Stats: CPU Busy 8 (44.44%)
    Stats: IO Busy  10 (55.56%)
    ```

    ```sh
    python3 process-run.py -s 1 -l 3:50,3:50 -I IO_RUN_LATER -S SWITCH_ON_IO -c -p
    Time        PID: 0        PID: 1           CPU           IOs
    1        RUN:cpu         READY             1          
    2         RUN:io         READY             1          
    3        BLOCKED       RUN:cpu             1             1
    4        BLOCKED       RUN:cpu             1             1
    5        BLOCKED       RUN:cpu             1             1
    6        BLOCKED          DONE                           1
    7        BLOCKED          DONE                           1
    8*   RUN:io_done          DONE             1          
    9         RUN:io          DONE             1          
    10        BLOCKED          DONE                           1
    11        BLOCKED          DONE                           1
    12        BLOCKED          DONE                           1
    13        BLOCKED          DONE                           1
    14        BLOCKED          DONE                           1
    15*   RUN:io_done          DONE             1          

    Stats: Total Time 15
    Stats: CPU Busy 8 (53.33%)
    Stats: IO Busy  10 (66.67%)
    ```

    ```sh
    python3 process-run.py -s 1 -l 3:50,3:50 -I IO_RUN_LATER -S SWITCH_ON_END -c -p
    Time        PID: 0        PID: 1           CPU           IOs
    1        RUN:cpu         READY             1          
    2         RUN:io         READY             1          
    3        BLOCKED         READY                           1
    4        BLOCKED         READY                           1
    5        BLOCKED         READY                           1
    6        BLOCKED         READY                           1
    7        BLOCKED         READY                           1
    8*   RUN:io_done         READY             1          
    9         RUN:io         READY             1          
    10        BLOCKED         READY                           1
    11        BLOCKED         READY                           1
    12        BLOCKED         READY                           1
    13        BLOCKED         READY                           1
    14        BLOCKED         READY                           1
    15*   RUN:io_done         READY             1          
    16           DONE       RUN:cpu             1          
    17           DONE       RUN:cpu             1          
    18           DONE       RUN:cpu             1          

    Stats: Total Time 18
    Stats: CPU Busy 8 (44.44%)
    Stats: IO Busy  10 (55.56%)
    ```

        seed 2:

    ```sh
    python3 process-run.py -s 2 -l 3:50,3:50
    Produce a trace of what would happen when you run these processes:
    Process 0
    io
    io_done
    io
    io_done
    cpu

    Process 1
    cpu
    io
    io_done
    io
    io_done

    Important behaviors:
    System will switch when the current process is FINISHED or ISSUES AN IO
    After IOs, the process issuing the IO will run LATER (when it is its turn)
    ```

    ```sh
    python3 process-run.py -s 2 -l 3:50,3:50 -I IO_RUN_IMMEDIATE -S SWITCH_ON_IO -c -p
    Time        PID: 0        PID: 1           CPU           IOs
    1         RUN:io         READY             1          
    2        BLOCKED       RUN:cpu             1             1
    3        BLOCKED        RUN:io             1             1
    4        BLOCKED       BLOCKED                           2
    5        BLOCKED       BLOCKED                           2
    6        BLOCKED       BLOCKED                           2
    7*   RUN:io_done       BLOCKED             1             1
    8         RUN:io       BLOCKED             1             1
    9*       BLOCKED   RUN:io_done             1             1
    10        BLOCKED        RUN:io             1             1
    11        BLOCKED       BLOCKED                           2
    12        BLOCKED       BLOCKED                           2
    13        BLOCKED       BLOCKED                           2
    14*   RUN:io_done       BLOCKED             1             1
    15        RUN:cpu       BLOCKED             1             1
    16*          DONE   RUN:io_done             1          

    Stats: Total Time 16
    Stats: CPU Busy 10 (62.50%)
    Stats: IO Busy  14 (87.50%)
    ```

    ```sh
    python3 process-run.py -s 2 -l 3:50,3:50 -I IO_RUN_IMMEDIATE -S SWITCH_ON_END -c -p
    Time        PID: 0        PID: 1           CPU           IOs
    1         RUN:io         READY             1          
    2        BLOCKED         READY                           1
    3        BLOCKED         READY                           1
    4        BLOCKED         READY                           1
    5        BLOCKED         READY                           1
    6        BLOCKED         READY                           1
    7*   RUN:io_done         READY             1          
    8         RUN:io         READY             1          
    9        BLOCKED         READY                           1
    10        BLOCKED         READY                           1
    11        BLOCKED         READY                           1
    12        BLOCKED         READY                           1
    13        BLOCKED         READY                           1
    14*   RUN:io_done         READY             1          
    15        RUN:cpu         READY             1          
    16           DONE       RUN:cpu             1          
    17           DONE        RUN:io             1          
    18           DONE       BLOCKED                           1
    19           DONE       BLOCKED                           1
    20           DONE       BLOCKED                           1
    21           DONE       BLOCKED                           1
    22           DONE       BLOCKED                           1
    23*          DONE   RUN:io_done             1          
    24           DONE        RUN:io             1          
    25           DONE       BLOCKED                           1
    26           DONE       BLOCKED                           1
    27           DONE       BLOCKED                           1
    28           DONE       BLOCKED                           1
    29           DONE       BLOCKED                           1
    30*          DONE   RUN:io_done             1          

    Stats: Total Time 30
    Stats: CPU Busy 10 (33.33%)
    Stats: IO Busy  20 (66.67%)
    ```

    ```sh
    python3 process-run.py -s 2 -l 3:50,3:50 -I IO_RUN_LATER -S SWITCH_ON_IO -c -p
    Time        PID: 0        PID: 1           CPU           IOs
    1         RUN:io         READY             1          
    2        BLOCKED       RUN:cpu             1             1
    3        BLOCKED        RUN:io             1             1
    4        BLOCKED       BLOCKED                           2
    5        BLOCKED       BLOCKED                           2
    6        BLOCKED       BLOCKED                           2
    7*   RUN:io_done       BLOCKED             1             1
    8         RUN:io       BLOCKED             1             1
    9*       BLOCKED   RUN:io_done             1             1
    10        BLOCKED        RUN:io             1             1
    11        BLOCKED       BLOCKED                           2
    12        BLOCKED       BLOCKED                           2
    13        BLOCKED       BLOCKED                           2
    14*   RUN:io_done       BLOCKED             1             1
    15        RUN:cpu       BLOCKED             1             1
    16*          DONE   RUN:io_done             1          

    Stats: Total Time 16
    Stats: CPU Busy 10 (62.50%)
    Stats: IO Busy  14 (87.50%)
    ```

    ```sh
    python3 process-run.py -s 2 -l 3:50,3:50 -I IO_RUN_LATER -S SWITCH_ON_END -c -p
    Time        PID: 0        PID: 1           CPU           IOs
    1         RUN:io         READY             1          
    2        BLOCKED         READY                           1
    3        BLOCKED         READY                           1
    4        BLOCKED         READY                           1
    5        BLOCKED         READY                           1
    6        BLOCKED         READY                           1
    7*   RUN:io_done         READY             1          
    8         RUN:io         READY             1          
    9        BLOCKED         READY                           1
    10        BLOCKED         READY                           1
    11        BLOCKED         READY                           1
    12        BLOCKED         READY                           1
    13        BLOCKED         READY                           1
    14*   RUN:io_done         READY             1          
    15        RUN:cpu         READY             1          
    16           DONE       RUN:cpu             1          
    17           DONE        RUN:io             1          
    18           DONE       BLOCKED                           1
    19           DONE       BLOCKED                           1
    20           DONE       BLOCKED                           1
    21           DONE       BLOCKED                           1
    22           DONE       BLOCKED                           1
    23*          DONE   RUN:io_done             1          
    24           DONE        RUN:io             1          
    25           DONE       BLOCKED                           1
    26           DONE       BLOCKED                           1
    27           DONE       BLOCKED                           1
    28           DONE       BLOCKED                           1
    29           DONE       BLOCKED                           1
    30*          DONE   RUN:io_done             1          

    Stats: Total Time 30
    Stats: CPU Busy 10 (33.33%)
    Stats: IO Busy  20 (66.67%)
    ```

        seed 3:

    ```sh
    python3 process-run.py -s 3 -l 3:50,3:50
    Produce a trace of what would happen when you run these processes:
    Process 0
    cpu
    io
    io_done
    cpu

    Process 1
    io
    io_done
    io
    io_done
    cpu

    Important behaviors:
    System will switch when the current process is FINISHED or ISSUES AN IO
    After IOs, the process issuing the IO will run LATER (when it is its turn)
    ```

    ```sh
    python3 process-run.py -s 3 -l 3:50,3:50 -I IO_RUN_IMMEDIATE -S SWITCH_ON_IO -c -p
    Time        PID: 0        PID: 1           CPU           IOs
    1        RUN:cpu         READY             1          
    2         RUN:io         READY             1          
    3        BLOCKED        RUN:io             1             1
    4        BLOCKED       BLOCKED                           2
    5        BLOCKED       BLOCKED                           2
    6        BLOCKED       BLOCKED                           2
    7        BLOCKED       BLOCKED                           2
    8*   RUN:io_done       BLOCKED             1             1
    9*         READY   RUN:io_done             1          
    10          READY        RUN:io             1          
    11        RUN:cpu       BLOCKED             1             1
    12           DONE       BLOCKED                           1
    13           DONE       BLOCKED                           1
    14           DONE       BLOCKED                           1
    15           DONE       BLOCKED                           1
    16*          DONE   RUN:io_done             1          
    17           DONE       RUN:cpu             1          

    Stats: Total Time 17
    Stats: CPU Busy 9 (52.94%)
    Stats: IO Busy  11 (64.71%)
    ```
    
    ```sh
    python3 process-run.py -s 3 -l 3:50,3:50 -I IO_RUN_IMMEDIATE -S SWITCH_ON_END -c -p
    Time        PID: 0        PID: 1           CPU           IOs
    1        RUN:cpu         READY             1          
    2         RUN:io         READY             1          
    3        BLOCKED         READY                           1
    4        BLOCKED         READY                           1
    5        BLOCKED         READY                           1
    6        BLOCKED         READY                           1
    7        BLOCKED         READY                           1
    8*   RUN:io_done         READY             1          
    9        RUN:cpu         READY             1          
    10           DONE        RUN:io             1          
    11           DONE       BLOCKED                           1
    12           DONE       BLOCKED                           1
    13           DONE       BLOCKED                           1
    14           DONE       BLOCKED                           1
    15           DONE       BLOCKED                           1
    16*          DONE   RUN:io_done             1          
    17           DONE        RUN:io             1          
    18           DONE       BLOCKED                           1
    19           DONE       BLOCKED                           1
    20           DONE       BLOCKED                           1
    21           DONE       BLOCKED                           1
    22           DONE       BLOCKED                           1
    23*          DONE   RUN:io_done             1          
    24           DONE       RUN:cpu             1          

    Stats: Total Time 24
    Stats: CPU Busy 9 (37.50%)
    Stats: IO Busy  15 (62.50%)
    ```

    ```sh
    python3 process-run.py -s 3 -l 3:50,3:50 -I IO_RUN_LATER -S SWITCH_ON_IO -c -p
    Time        PID: 0        PID: 1           CPU           IOs
    1        RUN:cpu         READY             1          
    2         RUN:io         READY             1          
    3        BLOCKED        RUN:io             1             1
    4        BLOCKED       BLOCKED                           2
    5        BLOCKED       BLOCKED                           2
    6        BLOCKED       BLOCKED                           2
    7        BLOCKED       BLOCKED                           2
    8*   RUN:io_done       BLOCKED             1             1
    9*       RUN:cpu         READY             1          
    10           DONE   RUN:io_done             1          
    11           DONE        RUN:io             1          
    12           DONE       BLOCKED                           1
    13           DONE       BLOCKED                           1
    14           DONE       BLOCKED                           1
    15           DONE       BLOCKED                           1
    16           DONE       BLOCKED                           1
    17*          DONE   RUN:io_done             1          
    18           DONE       RUN:cpu             1          

    Stats: Total Time 18
    Stats: CPU Busy 9 (50.00%)
    ```

    ```sh
    python3 process-run.py -s 3 -l 3:50,3:50 -I IO_RUN_LATER -S SWITCH_ON_END -c -p
    Time        PID: 0        PID: 1           CPU           IOs
    1        RUN:cpu         READY             1          
    2         RUN:io         READY             1          
    3        BLOCKED         READY                           1
    4        BLOCKED         READY                           1
    5        BLOCKED         READY                           1
    6        BLOCKED         READY                           1
    7        BLOCKED         READY                           1
    8*   RUN:io_done         READY             1          
    9        RUN:cpu         READY             1          
    10           DONE        RUN:io             1          
    11           DONE       BLOCKED                           1
    12           DONE       BLOCKED                           1
    13           DONE       BLOCKED                           1
    14           DONE       BLOCKED                           1
    15           DONE       BLOCKED                           1
    16*          DONE   RUN:io_done             1          
    17           DONE        RUN:io             1          
    18           DONE       BLOCKED                           1
    19           DONE       BLOCKED                           1
    20           DONE       BLOCKED                           1
    21           DONE       BLOCKED                           1
    22           DONE       BLOCKED                           1
    23*          DONE   RUN:io_done             1          
    24           DONE       RUN:cpu             1          

    Stats: Total Time 24
    Stats: CPU Busy 9 (37.50%)
    Stats: IO Busy  15 (62.50%)
    ```

Notes:
- Regardless of IO_RUN_LATER or IO_RUN_IMMEDIATE, if SWITCH_ON_END, the time is the same since we wait until the ENTIRE process; is finished performing its actions.
- IO_RUN_IMMEDIATE is never worse than IO_RUN_LATER in our cases, but it could be worse. For example, if you take the CPU away from a process that is about to get to IO actions for a bunch of CPU actions, we miss out on concurrently doing those IO actions with the CPU actions.
- Two+ IO actions can run concurrently.