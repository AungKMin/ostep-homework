1.  Run a few randomly-generated problems with just two jobs and
two queues; compute the MLFQ execution trace for each. Make
your life easier by limiting the length of each job and turning off
I/Os.

    ```
    python3 mlfq.py -s 1 -n 2 -j 2 -m 20 -M 0 -c
    Here is the list of inputs:
    OPTIONS jobs 2
    OPTIONS queues 2
    OPTIONS allotments for queue  1 is   1
    OPTIONS quantum length for queue  1 is  10
    OPTIONS allotments for queue  0 is   1
    OPTIONS quantum length for queue  0 is  10
    OPTIONS boost 0
    OPTIONS ioTime 5
    OPTIONS stayAfterIO False
    OPTIONS iobump False


    For each job, three defining characteristics are given:
    startTime : at what time does the job enter the system
    runTime   : the total CPU time needed by the job to finish
    ioFreq    : every ioFreq time units, the job issues an I/O
                (the I/O takes ioTime units to complete)

    Job List:
    Job  0: startTime   0 - runTime   3 - ioFreq   0
    Job  1: startTime   0 - runTime  15 - ioFreq   0


    Execution Trace:

    [ time 0 ] JOB BEGINS by JOB 0
    [ time 0 ] JOB BEGINS by JOB 1
    [ time 0 ] Run JOB 0 at PRIORITY 1 [ TICKS 9 ALLOT 1 TIME 2 (of 3) ]
    [ time 1 ] Run JOB 0 at PRIORITY 1 [ TICKS 8 ALLOT 1 TIME 1 (of 3) ]
    [ time 2 ] Run JOB 0 at PRIORITY 1 [ TICKS 7 ALLOT 1 TIME 0 (of 3) ]
    [ time 3 ] FINISHED JOB 0
    [ time 3 ] Run JOB 1 at PRIORITY 1 [ TICKS 9 ALLOT 1 TIME 14 (of 15) ]
    [ time 4 ] Run JOB 1 at PRIORITY 1 [ TICKS 8 ALLOT 1 TIME 13 (of 15) ]
    [ time 5 ] Run JOB 1 at PRIORITY 1 [ TICKS 7 ALLOT 1 TIME 12 (of 15) ]
    [ time 6 ] Run JOB 1 at PRIORITY 1 [ TICKS 6 ALLOT 1 TIME 11 (of 15) ]
    [ time 7 ] Run JOB 1 at PRIORITY 1 [ TICKS 5 ALLOT 1 TIME 10 (of 15) ]
    [ time 8 ] Run JOB 1 at PRIORITY 1 [ TICKS 4 ALLOT 1 TIME 9 (of 15) ]
    [ time 9 ] Run JOB 1 at PRIORITY 1 [ TICKS 3 ALLOT 1 TIME 8 (of 15) ]
    [ time 10 ] Run JOB 1 at PRIORITY 1 [ TICKS 2 ALLOT 1 TIME 7 (of 15) ]
    [ time 11 ] Run JOB 1 at PRIORITY 1 [ TICKS 1 ALLOT 1 TIME 6 (of 15) ]
    [ time 12 ] Run JOB 1 at PRIORITY 1 [ TICKS 0 ALLOT 1 TIME 5 (of 15) ]
    [ time 13 ] Run JOB 1 at PRIORITY 0 [ TICKS 9 ALLOT 1 TIME 4 (of 15) ]
    [ time 14 ] Run JOB 1 at PRIORITY 0 [ TICKS 8 ALLOT 1 TIME 3 (of 15) ]
    [ time 15 ] Run JOB 1 at PRIORITY 0 [ TICKS 7 ALLOT 1 TIME 2 (of 15) ]
    [ time 16 ] Run JOB 1 at PRIORITY 0 [ TICKS 6 ALLOT 1 TIME 1 (of 15) ]
    [ time 17 ] Run JOB 1 at PRIORITY 0 [ TICKS 5 ALLOT 1 TIME 0 (of 15) ]
    [ time 18 ] FINISHED JOB 1

    Final statistics:
    Job  0: startTime   0 - response   0 - turnaround   3
    Job  1: startTime   0 - response   3 - turnaround  18

    Avg  1: startTime n/a - response 1.50 - turnaround 10.50
    ```

    ```
    python3 mlfq.py -s 2 -n 2 -j 2 -m 20 -M 0 -c
    Here is the list of inputs:
    OPTIONS jobs 2
    OPTIONS queues 2
    OPTIONS allotments for queue  1 is   1
    OPTIONS quantum length for queue  1 is  10
    OPTIONS allotments for queue  0 is   1
    OPTIONS quantum length for queue  0 is  10
    OPTIONS boost 0
    OPTIONS ioTime 5
    OPTIONS stayAfterIO False
    OPTIONS iobump False


    For each job, three defining characteristics are given:
    startTime : at what time does the job enter the system
    runTime   : the total CPU time needed by the job to finish
    ioFreq    : every ioFreq time units, the job issues an I/O
                (the I/O takes ioTime units to complete)

    Job List:
    Job  0: startTime   0 - runTime  19 - ioFreq   0
    Job  1: startTime   0 - runTime   2 - ioFreq   0


    Execution Trace:

    [ time 0 ] JOB BEGINS by JOB 0
    [ time 0 ] JOB BEGINS by JOB 1
    [ time 0 ] Run JOB 0 at PRIORITY 1 [ TICKS 9 ALLOT 1 TIME 18 (of 19) ]
    [ time 1 ] Run JOB 0 at PRIORITY 1 [ TICKS 8 ALLOT 1 TIME 17 (of 19) ]
    [ time 2 ] Run JOB 0 at PRIORITY 1 [ TICKS 7 ALLOT 1 TIME 16 (of 19) ]
    [ time 3 ] Run JOB 0 at PRIORITY 1 [ TICKS 6 ALLOT 1 TIME 15 (of 19) ]
    [ time 4 ] Run JOB 0 at PRIORITY 1 [ TICKS 5 ALLOT 1 TIME 14 (of 19) ]
    [ time 5 ] Run JOB 0 at PRIORITY 1 [ TICKS 4 ALLOT 1 TIME 13 (of 19) ]
    [ time 6 ] Run JOB 0 at PRIORITY 1 [ TICKS 3 ALLOT 1 TIME 12 (of 19) ]
    [ time 7 ] Run JOB 0 at PRIORITY 1 [ TICKS 2 ALLOT 1 TIME 11 (of 19) ]
    [ time 8 ] Run JOB 0 at PRIORITY 1 [ TICKS 1 ALLOT 1 TIME 10 (of 19) ]
    [ time 9 ] Run JOB 0 at PRIORITY 1 [ TICKS 0 ALLOT 1 TIME 9 (of 19) ]
    [ time 10 ] Run JOB 1 at PRIORITY 1 [ TICKS 9 ALLOT 1 TIME 1 (of 2) ]
    [ time 11 ] Run JOB 1 at PRIORITY 1 [ TICKS 8 ALLOT 1 TIME 0 (of 2) ]
    [ time 12 ] FINISHED JOB 1
    [ time 12 ] Run JOB 0 at PRIORITY 0 [ TICKS 9 ALLOT 1 TIME 8 (of 19) ]
    [ time 13 ] Run JOB 0 at PRIORITY 0 [ TICKS 8 ALLOT 1 TIME 7 (of 19) ]
    [ time 14 ] Run JOB 0 at PRIORITY 0 [ TICKS 7 ALLOT 1 TIME 6 (of 19) ]
    [ time 15 ] Run JOB 0 at PRIORITY 0 [ TICKS 6 ALLOT 1 TIME 5 (of 19) ]
    [ time 16 ] Run JOB 0 at PRIORITY 0 [ TICKS 5 ALLOT 1 TIME 4 (of 19) ]
    [ time 17 ] Run JOB 0 at PRIORITY 0 [ TICKS 4 ALLOT 1 TIME 3 (of 19) ]
    [ time 18 ] Run JOB 0 at PRIORITY 0 [ TICKS 3 ALLOT 1 TIME 2 (of 19) ]
    [ time 19 ] Run JOB 0 at PRIORITY 0 [ TICKS 2 ALLOT 1 TIME 1 (of 19) ]
    [ time 20 ] Run JOB 0 at PRIORITY 0 [ TICKS 1 ALLOT 1 TIME 0 (of 19) ]
    [ time 21 ] FINISHED JOB 0

    Final statistics:
    Job  0: startTime   0 - response   0 - turnaround  21
    Job  1: startTime   0 - response  10 - turnaround  12

    Avg  1: startTime n/a - response 5.00 - turnaround 16.50
    ```

2. How would you run the scheduler to reproduce each of the examples in the chapter?

    Ans: 
    ```
    - 8.2 Long-running Job Over Time:
        - python3 mlfq.py -n 3 -j 1 -q 10 -l 0,200,0 -c
    - 8.3 Along Came An Interactive Job:
        - python3 mlfq.py -n 3 -j 2 -q 10 -l 0,180,0:100,20,0 -c
    - 8.4 A Mixed I/O-intensive and CPU-intensive Workload:
        - python3 mlfq.py -n 3 -j 2 -q 10 -l 0,175,0:50,25,1 -i 5 -S -c
        - // the CPU-intensive process is chosen to be 175 so that the interactive process would be 25 and this makes the interactive process alternate with the CPU-intensive process up to time = 200.
    - 8.5 Without (Left) Priority Boost
        - python3 mlfq.py -n 3 -j 3 -q 10 -l 0,120,0:100,50,1:100,50,1 -i 1 -S -c
        - // the CPU-intensive process being 120: 120 is just some arbiratrary time longer than 100
    - 8.5 With (Right) Priority Boost
        - python3 mlfq.py -n 3 -j 3 -q 10 -l 0,120,0:100,50,1:100,50,1 -i 1 -B 50 -c
        - // the only difference is that if the priority boost happens every 50 seconds, the CPU-intensive process will boost to the top queue even if no other processes are running. 
        - // there might be a bug with the simulator where the CPU-intensive process only stays in Q2 for one clock cycle before demoting to Q1 after priority boost => might also just be that since the CPU-intensive process has already used up its time slice in Q2 before the priority boost, it automatically demotes to Q1 (but its time usage is reset in Q1) 
    - 8.6 Without (Left) Gaming Tolerance
        -  python3 mlfq.py -n 3 -j 2 -q 10 -l 0,150,0:80,100,9 -i 1 -S -c
    - 8.6 With (Right) Gaming Tolerance
        - python3 mlfq.py -n 3 -j 2 -q 10 -l 0,150,0:80,100,9 -i 1 -c
    - 8.7 Lower Priority, Longer Quanta
        - python3 mlfq.py -n 3 -j 2 -Q 10,20,40 -l 0,280,0:0,280,0 -a 2 -c
        - // the -a 2 means a process gets demoted after 2 time slices instead of 1
    ```

3.  How would you configure the scheduler parameters to behave just
like a round-robin scheduler?
    
    Ans: Round-robin is different from MLFQ in this way:
    - MLFQ has many queues; but round-robin only has one queue
    - within each queue, MLFQ uses round-robin
    
    So the solution is to just have one queue (`-n`).

4. Craft a workload with two jobs and scheduler parameters so that
one job takes advantage of the older Rules 4a and 4b (turned on
with the -S flag) to game the scheduler and obtain 99% of the CPU
over a particular time interval.

    ```
    python3 mlfq.py -n 3 -j 2 -q 10 -l 0,50,0:0,50,9 -i 1 -S -c
    ```

5. Given a system with a quantum length of 10 ms in its highest queue,
how often would you have to boost jobs back to the highest priority
level (with the -B flag) in order to guarantee that a single longrunning (and potentially-starving) job gets at least 5% of the CPU?

    Ans: Assuming this means 5% of the CPU time, worst case is where after the 10ms in the highest queue, the longrunning job gets no CPU time until the next boost. So: 10ms / (boost time) = 0.05 => (boost time) = 10ms / 0.05 = **200ms** 

    ```
    python3 mlfq.py -n 3 -j 3 -q 10 -l 0,200,0:0,200,1:0,200,1 -i 1 -S -B 200 -c
    ```

6. One question that arises in scheduling is which end of a queue to
add a job that just finished I/O; the -I flag changes this behavior
for this scheduling simulator. Play around with some workloads
and see if you can see the effect of this flag.

    Ans: Adding `-I` makes it so that the process that just finished I/O gets added to the front of the queue. 

    ```
    python3 mlfq.py -n 2 -j 2 -q 10 -l 0,50,0:0,50,11 -i 1 -S -c

    Final statistics:
    Job  0: startTime   0 - response   0 - turnaround  72
    Job  1: startTime   0 - response  10 - turnaround 102

    Avg  1: startTime n/a - response 5.00 - turnaround 87.00
    ```

    ```
    python3 mlfq.py -n 2 -j 2 -q 10 -l 0,50,0:0,50,11 -i 1 -S -I -c

    Final statistics:
    Job  0: startTime   0 - response   0 - turnaround  93
    Job  1: startTime   0 - response  10 - turnaround 101

    Avg  1: startTime n/a - response 5.00 - turnaround 97.00
    ```

    In this case, adding `-I` reduces the turnaround of Job 1 (that does the I/O) as expected. However, the increase in turnaround of Job 2 results in the average turnaround being higher with the `-I` tag. 

    But in other cases, adding `-I` reduces the average turnaround time:

    ```
    python3 mlfq.py -n 2 -j 2 -q 10 -l 0,50,0:0,50,10 -i 1 -S -c

    Final statistics:
    Job  0: startTime   0 - response   0 - turnaround  90
    Job  1: startTime   0 - response  10 - turnaround 100

    Avg  1: startTime n/a - response 5.00 - turnaround 95.00
    ```

    ```
    python3 mlfq.py -n 2 -j 2 -q 10 -l 0,50,0:0,50,10 -i 1 -S -I -c

    Final statistics:
    Job  0: startTime   0 - response   0 - turnaround 100
    Job  1: startTime   0 - response  10 - turnaround  64

    Avg  1: startTime n/a - response 5.00 - turnaround 82.00
    ```
