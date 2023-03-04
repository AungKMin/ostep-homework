1. Examine flag.s. This code “implements” locking with a single memory
flag. Can you understand the assembly?

   - The code adds 1 to `count` for the number of times defined in `%bx`.
   - The lock surrounding the critical section of incrementing `count` is a simple spin lock
   - Note that the lock doesn't work since the locked/unlocked check and set to 1 together are not atomic (if an interrupt occurs between the)

1. When you run with the defaults, does flag.s work? Use the -M and -R
flags to trace variables and registers (and turn on -c to see their values).
Can you predict what value will end up in flag?
    - For the default interrupt interval, it works fine since the entire code runs before interrupting to run the other thread
    - `flag` will have the value of 1 (locked) during the critical section and value of 0 (unlocked) in all other sections
    - `count` will end up at value 2
    - `python3 x86.py -p flag.s -M flag,count -R ax,bx -c`

3. Change the value of the register %bx with the -a flag (e.g., -a bx=2,bx=2
if you are running just two threads). What does the code do? How does it
change your answer for the question above?
-   No thread interleaving so the code executes as expected (`count` = 4).
-   `python3 x86.py -p flag.s -M flag,count -R ax,bx -a bx=2,bx=2 -c`

4. Set bx to a high value for each thread, and then use the -i flag to generate
different interrupt frequencies; what values lead to a bad outcomes? Which
lead to good outcomes?
   - A bad outcome occurs whenever an interrupt occurs during the locking code (so the lock doesn't work and the critical section gets interrupted)
   - e.g. `python3 x86.py -p flag.s -M flag,count -R ax,bx -a bx=2,bx=2 -i 3 -c` results in final `count` being 3 (it should be 4)
   - If the interrupt interval is a multiple of 11, the final `count` value will be correct

1. Now let’s look at the program test-and-set.s. First, try to understand
the code, which uses the xchg instruction to build a simple locking primitive. How is the lock acquire written? How about lock release?
   - Note that the lock works since the locked/unlocked check and set to 1 together together are atomic in the form of the `xchg` instruction (so there can be no interruption between the fetching of the flag value and the set)

1. Now run the code, changing the value of the interrupt interval (-i) again,
and making sure to loop for a number of times. Does the code always work
as expected? Does it sometimes lead to an inefficient use of the CPU? How
could you quantify that?
   - Yes. The code always works.
   - Sometimes it leads to an inefficient use of the CPU since the test and set lock spins whenever the lock is not available
   - For example, `python3 x86.py -p test-and-set.s -M count -R ax,bx -a bx=2,bx=2 -i 3 -c` results 66 clock cycles whereas the normal uninterrupted run is 22 clock cycles.
   - Note that any interrupt interval multiple of 11 will result in no clock cycle waste

7. Use the -P flag to generate specific tests of the locking code. For example,
run a schedule that grabs the lock in the first thread, but then tries to acquire
it in the second. Does the right thing happen? What else should you test?
   - We try interrupting right after the `xchg` instruction. If the instruction wasn't atomic, this could result in both threads setting the lock to 1 and then entering the critical section at the same time.
   - `python3 x86.py -p test-and-set.s -M count -R ax,bx -a bx=0,bx=0 -P 00111111 -c`
     - This works:
      ```
      count      ax    bx          Thread 0                Thread 1         

         0       0     0   
         0       1     0   1000 mov  $1, %ax
         0       0     0   1001 xchg %ax, mutex
         0       0     0   ------ Interrupt ------  ------ Interrupt ------  
         0       1     0                            1000 mov  $1, %ax
         0       1     0                            1001 xchg %ax, mutex
         0       1     0                            1002 test $0, %ax
         0       1     0                            1003 jne  .acquire
         0       1     0                            1000 mov  $1, %ax
         0       1     0                            1001 xchg %ax, mutex
      ```
8. Now let’s look at the code in peterson.s, which implements Peterson’s
algorithm (mentioned in a sidebar in the text). Study the code and see if
you can make sense of it.

   - Notes about Peterson's algorithm:
     - if you just use turn (just set one variable to 0 in thread 0 and 1 in thread 1), the threads could easily enter the critical sections at the same time
     - if you just use locks (lock for 0 and 1), both threads could spin forever (both locks get set to 1)
     - so you need both the turn and locks
     - note that you need the thread to assign turn to the OTHER thread instead of itself since imagine the other thread is in its critical section; if the current thread keeps looping, it will pass the spin loop and go into the critical section too.
  
  9. Now run the code with different values of -i. What kinds of different behavior do you see? Make sure to set the thread IDs appropriately (using -a
bx=0,bx=1 for example) as the code assumes it.
      - The code runs correctly; however, the amount of time running depends on how often the interrupt occurs (not a linear relationship)
      - **Note:** I don't think you need line 51 (`mov %cx, turn`); you don't need to set the turn when unlocking
      - `python3 x86.py -p peterson.s -a bx=0,bx=1 -R ax,cx -M flag,turn,count -c -i 2`

10. Can you control the scheduling (with the -P flag) to “prove” that the code
works? What are the different cases you should show hold? Think about
mutual exclusion and deadlock avoidance.
    - Thread 1 runs until just before entering the spin wait (000 000 000)
    - Thread 2 tries to enter the critical section (111 111 111 111 1)
    - Thread 1 runs until half the critical section (00)
    - Thread 2 tries to enter the critical section (111 111)
    - Thread 1 finishes the critical section and the unlock code (000)
    - Thread 2 enters the critical section (111 111)
    - `python3 x86.py -p peterson.s -a bx=0,bx=1 -R ax,cx -M flag,turn,count -c -P 000000000111111111111100111111000111111`

11. Now study the code for the ticket lock in ticket.s. Does it match the code
in the chapter? Then run with the following flags: -a bx=1000,bx=1000
(causing each thread to loop through the critical section 1000 times). Watch
what happens; do the threads spend much time spin-waiting for the lock?
    - `python3 x86.py -p ticket.s -a bx=1000,bx=1000 -R ax,bx -M ticket,turn,count -c`
    - Yes. A lot of time is spent spin-waiting since the interrupt interval is high. 

12. How does the code behave as you add more threads?
    - `python3 x86.py -p ticket.s -R ax,bx -M ticket,turn,count -t 5 -i 2 -c`
    - Works but most threads can spend their time spin-waiting depending on the interrupt interval

13. Now examine yield.s, in which a yield instruction enables one thread
to yield control of the CPU (realistically, this would be an OS primitive, but
for the simplicity, we assume an instruction does the task). Find a scenario
where test-and-set.s wastes cycles spinning, but yield.s does not.
How many instructions are saved? In what scenarios do these savings arise?
   - `yield.s` saves cycles compared to `test-and-set.s` in the cases when a thread is interrupted during its critical section and the other thread must spin-wait for its time slice
   - the yield instruction would result in the CPU switching threads instead of spin waiting
   - test-and-set: `python3 x86.py -p test-and-set.s -a bx=5,bx=5 -R ax -M count -i 6 -c | wc -l 220`
   - yield: `python3 x86.py -p yield.s -a bx=5,bx=5 -R ax -M count -i 6 -c | wc -l 215`

14. Finally, examine test-and-test-and-set.s. What does this lock do?
What kind of savings does it introduce as compared to test-and-set.s?
   -  `test-and-test-and-set.s` first tests to see if mutex == 0 before test-and-setting mutex. 
   -  Althugh the mutex == 0 test doesn't guarantee mutex == 0 in a multi-threaded program (since an interrupt could occur between the check and the `xchg`), it still reduces write traffic