1. Compute the solutions for simulations with 3 jobs and random seeds of 1, 2, and 3.

    ```
    python3 lottery.py -s 1 -c

    Here is the job list, with the run time of each job: 
    Job 0 ( length = 1, tickets = 84 )
    Job 1 ( length = 7, tickets = 25 )
    Job 2 ( length = 4, tickets = 44 )

    Here is the set of random numbers you will need (at most):
    Random 651593
    Random 788724
    Random 93859
    Random 28347
    Random 835765
    Random 432767
    Random 762280
    Random 2106
    Random 445387
    Random 721540
    Random 228762
    Random 945271

    ** Solutions **

    Random 651593 -> Winning ticket 119 (of 153) -> Run 2
    Jobs:
    (  job:0 timeleft:1 tix:84 )  (  job:1 timeleft:7 tix:25 )  (* job:2 timeleft:4 tix:44 ) 
    Random 788724 -> Winning ticket 9 (of 153) -> Run 0
    Jobs:
    (* job:0 timeleft:1 tix:84 )  (  job:1 timeleft:7 tix:25 )  (  job:2 timeleft:3 tix:44 ) 
    --> JOB 0 DONE at time 2
    Random 93859 -> Winning ticket 19 (of 69) -> Run 1
    Jobs:
    (  job:0 timeleft:0 tix:--- )  (* job:1 timeleft:7 tix:25 )  (  job:2 timeleft:3 tix:44 ) 
    Random 28347 -> Winning ticket 57 (of 69) -> Run 2
    Jobs:
    (  job:0 timeleft:0 tix:--- )  (  job:1 timeleft:6 tix:25 )  (* job:2 timeleft:3 tix:44 ) 
    Random 835765 -> Winning ticket 37 (of 69) -> Run 2
    Jobs:
    (  job:0 timeleft:0 tix:--- )  (  job:1 timeleft:6 tix:25 )  (* job:2 timeleft:2 tix:44 ) 
    Random 432767 -> Winning ticket 68 (of 69) -> Run 2
    Jobs:
    (  job:0 timeleft:0 tix:--- )  (  job:1 timeleft:6 tix:25 )  (* job:2 timeleft:1 tix:44 ) 
    --> JOB 2 DONE at time 6
    Random 762280 -> Winning ticket 5 (of 25) -> Run 1
    Jobs:
    (  job:0 timeleft:0 tix:--- )  (* job:1 timeleft:6 tix:25 )  (  job:2 timeleft:0 tix:--- ) 
    Random 2106 -> Winning ticket 6 (of 25) -> Run 1
    Jobs:
    (  job:0 timeleft:0 tix:--- )  (* job:1 timeleft:5 tix:25 )  (  job:2 timeleft:0 tix:--- ) 
    Random 445387 -> Winning ticket 12 (of 25) -> Run 1
    Jobs:
    (  job:0 timeleft:0 tix:--- )  (* job:1 timeleft:4 tix:25 )  (  job:2 timeleft:0 tix:--- ) 
    Random 721540 -> Winning ticket 15 (of 25) -> Run 1
    Jobs:
    (  job:0 timeleft:0 tix:--- )  (* job:1 timeleft:3 tix:25 )  (  job:2 timeleft:0 tix:--- ) 
    Random 228762 -> Winning ticket 12 (of 25) -> Run 1
    Jobs:
    (  job:0 timeleft:0 tix:--- )  (* job:1 timeleft:2 tix:25 )  (  job:2 timeleft:0 tix:--- ) 
    Random 945271 -> Winning ticket 21 (of 25) -> Run 1
    Jobs:
    (  job:0 timeleft:0 tix:--- )  (* job:1 timeleft:1 tix:25 )  (  job:2 timeleft:0 tix:--- ) 
    --> JOB 1 DONE at time 12
    ```

    etc...

2.  Now run with two specific jobs: each of length 10, but one (job 0)
with just 1 ticket and the other (job 1) with 100 (e.g., -l 10:1,10:100).
What happens when the number of tickets is so imbalanced? Will
job 0 ever run before job 1 completes? How often? In general, what
does such a ticket imbalance do to the behavior of lottery scheduling?

    ```
    python3 lottery.py -l 10:1,10:100 -c
    ```

    Before Job 1 finishes, Job 0 doesn't even run once. Unless Job 1 takes many cycles (much more than 100) to complete, Job 0 will likely only run once or not even once before Job 1 finishes.

    Thus, Job 1 will likely finish before Job 0 (unless Job 1 takes longer than 100 * (Job 0 time) to run).

3. When running with two jobs of length 100 and equal ticket allocations of 100 (-l 100:100,100:100), how unfair is the scheduler?
Run with some different random seeds to determine the (probabilistic) answer; let unfairness be determined by how much earlier one
job finishes than the other.

4. How does your answer to the previous question change as the quantum size (-q) gets larger?


    Ans: If the quantum size is small-ish (10), the fairness is around 0.75 (quite high). If the quantum size is big (50, 100), the fairness fluctuates a lot more.

    ```
    python3 lottery.py -l 100:100,100:100 -q 10 -s 0 -c

    // Fairness = early job finish time / late job finish time = 150 / 200 = 0.75
    ```

    ```
    python3 lottery.py -l 100:100,100:100 -q 10 -s 1 -c

    // Fairness = early job finish time / late job finish time = 160 / 200 = 0.80
    ```

    ```
    python3 lottery.py -l 100:100,100:100 -q 50 -s 0 -c

    // Fairness = early job finish time / late job finish time = 150 / 200 = 0.75
    ```

    ```
    python3 lottery.py -l 100:100,100:100 -q 100 -s 0 -c

    // Fairness = early job finish time / late job finish time = 100 / 200 = 0.5
    ```

5. Can you make a version of the graph that is found in the chapter?
What else would be worth exploring? How would the graph look
with a stride scheduler?

    If the length of jobs change while the quantum size remains constant, longer jobs will result in highe fairness since the relative quantum size decreases.

    If we used a strude scheduler, the graph will always have fairness = 1 since a stride scheduler is 100% fair always (it's deterministic).


    