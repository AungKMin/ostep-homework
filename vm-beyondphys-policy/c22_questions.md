1. Generate random addresses with the following arguments: -s 0
-n 10, -s 1 -n 10, and -s 2 -n 10. Change the policy from
FIFO, to LRU, to OPT. Compute whether each access in said address
traces are hits or misses.

    - ```

        // FIFO
        python3 paging-policy.py -s 0 -n 10 --policy=FIFO -c
        ARG addresses -1
        ARG addressfile 
        ARG numaddrs 10
        ARG policy FIFO
        ARG clockbits 2
        ARG cachesize 3
        ARG maxpage 10
        ARG seed 0
        ARG notrace False

        Solving...

        Access: 8  MISS FirstIn ->          [8] <- Lastin  Replaced:- [Hits:0 Misses:1]
        Access: 7  MISS FirstIn ->       [8, 7] <- Lastin  Replaced:- [Hits:0 Misses:2]
        Access: 4  MISS FirstIn ->    [8, 7, 4] <- Lastin  Replaced:- [Hits:0 Misses:3]
        Access: 2  MISS FirstIn ->    [7, 4, 2] <- Lastin  Replaced:8 [Hits:0 Misses:4]
        Access: 5  MISS FirstIn ->    [4, 2, 5] <- Lastin  Replaced:7 [Hits:0 Misses:5]
        Access: 4  HIT  FirstIn ->    [4, 2, 5] <- Lastin  Replaced:- [Hits:1 Misses:5]
        Access: 7  MISS FirstIn ->    [2, 5, 7] <- Lastin  Replaced:4 [Hits:1 Misses:6]
        Access: 3  MISS FirstIn ->    [5, 7, 3] <- Lastin  Replaced:2 [Hits:1 Misses:7]
        Access: 4  MISS FirstIn ->    [7, 3, 4] <- Lastin  Replaced:5 [Hits:1 Misses:8]
        Access: 5  MISS FirstIn ->    [3, 4, 5] <- Lastin  Replaced:7 [Hits:1 Misses:9]

        FINALSTATS hits 1   misses 9   hitrate 10.00
    

        // LRU
        python3 paging-policy.py -s 0 -n 10 --policy=LRU -c
        ARG addresses -1
        ARG addressfile 
        ARG numaddrs 10
        ARG policy LRU
        ARG clockbits 2
        ARG cachesize 3
        ARG maxpage 10
        ARG seed 0
        ARG notrace False

        Solving...

        Access: 8  MISS LRU ->          [8] <- MRU Replaced:- [Hits:0 Misses:1]
        Access: 7  MISS LRU ->       [8, 7] <- MRU Replaced:- [Hits:0 Misses:2]
        Access: 4  MISS LRU ->    [8, 7, 4] <- MRU Replaced:- [Hits:0 Misses:3]
        Access: 2  MISS LRU ->    [7, 4, 2] <- MRU Replaced:8 [Hits:0 Misses:4]
        Access: 5  MISS LRU ->    [4, 2, 5] <- MRU Replaced:7 [Hits:0 Misses:5]
        Access: 4  HIT  LRU ->    [2, 5, 4] <- MRU Replaced:- [Hits:1 Misses:5]
        Access: 7  MISS LRU ->    [5, 4, 7] <- MRU Replaced:2 [Hits:1 Misses:6]
        Access: 3  MISS LRU ->    [4, 7, 3] <- MRU Replaced:5 [Hits:1 Misses:7]
        Access: 4  HIT  LRU ->    [7, 3, 4] <- MRU Replaced:- [Hits:2 Misses:7]
        Access: 5  MISS LRU ->    [3, 4, 5] <- MRU Replaced:7 [Hits:2 Misses:8]

        FINALSTATS hits 2   misses 8   hitrate 20.00


        // OPT
        python3 paging-policy.py -s 0 -n 10 --policy=OPT -c
        ARG addresses -1
        ARG addressfile 
        ARG numaddrs 10
        ARG policy OPT
        ARG clockbits 2
        ARG cachesize 3
        ARG maxpage 10
        ARG seed 0
        ARG notrace False

        Solving...

        Access: 8  MISS Left  ->          [8] <- Right Replaced:- [Hits:0 Misses:1]
        Access: 7  MISS Left  ->       [8, 7] <- Right Replaced:- [Hits:0 Misses:2]
        Access: 4  MISS Left  ->    [8, 7, 4] <- Right Replaced:- [Hits:0 Misses:3]
        Access: 2  MISS Left  ->    [7, 4, 2] <- Right Replaced:8 [Hits:0 Misses:4]
        Access: 5  MISS Left  ->    [7, 4, 5] <- Right Replaced:2 [Hits:0 Misses:5]
        Access: 4  HIT  Left  ->    [7, 4, 5] <- Right Replaced:- [Hits:1 Misses:5]
        Access: 7  HIT  Left  ->    [7, 4, 5] <- Right Replaced:- [Hits:2 Misses:5]
        Access: 3  MISS Left  ->    [4, 5, 3] <- Right Replaced:7 [Hits:2 Misses:6]
        Access: 4  HIT  Left  ->    [4, 5, 3] <- Right Replaced:- [Hits:3 Misses:6]
        Access: 5  HIT  Left  ->    [4, 5, 3] <- Right Replaced:- [Hits:4 Misses:6]

        FINALSTATS hits 4   misses 6   hitrate 40.00
    ```

2. For a cache of size 5, generate worst-case address reference streams
for each of the following policies: FIFO, LRU, and MRU (worst-case
reference streams cause the most misses possible. For the worst case
reference streams, how much bigger of a cache is needed to improve
performance dramatically and approach OPT?

      - FIFO: 1 2 3 4 5 6 1 2 3 4 5 6 ... -> 0% hit rate
      - LRU: 1 2 3 4 5 6 1 2 3 4 5 6 ... -> 0% hit rate
      - MRU: 1 2 3 4 5 6 5 6 5 6 ... -> 0% hit rate
      - For the FIFO and LRU cases, we just need a cache size that is one larger to have a major improvement (approach 100% hit rate as we continue the stream without bound)
      - For the MRU case, no improvement in cache size will improve hit rate

1. Generate a random trace (use python or perl). How would you
expect the different policies to perform on such a trace?
    - On a random trace, only OPT will have an advantage. All other policies will have the same performance (since no locality is present).

4. Now generate a trace with some locality. How can you generate
such a trace? How does LRU perform on it? How much better than
RAND is LRU? How does CLOCK do? How about CLOCK with
different numbers of clock bits?
   -  LRU would perform the best on a trace with some locality. CLOCK would perform the next best (as the number of clock bits increase, performance increases as well). Then FIFO would perform the next best. Then RAND. 

5. Use a program like valgrind to instrument a real application and
generate a virtual page reference stream. For example, running
valgrind --tool=lackey --trace-mem=yes ls will output
a nearly-complete reference trace of every instruction and data reference made by the program ls. To make this useful for the simulator above, you’ll have to first transform each virtual memory
reference into a virtual page-number reference (done by masking
off the offset and shifting the resulting bits downward). How big
of a cache is needed for your application trace in order to satisfy a
large fraction of requests? Plot a graph of its working set as the size
of the cache increases.

   - expected order from good to bad: OPT, LRU, CLOCK, FIFO, RAND. MRU for ones with locality 