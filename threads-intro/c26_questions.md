1. Let’s examine a simple program, “loop.s”. First, just read and understand it. Then, run it with these arguments (./x86.py -p loop.s
-t 1 -i 100 -R dx) This specifies a single thread, an interrupt
every 100 instructions, and tracing of register %dx. What will %dx
be during the run? Use the -c flag to check your answers; the answers, on the left, show the value of the register (or memory value)
after the instruction on the right has run.

   - Since dx starts at 0, we only subtract one. dx at the end equals -1.

    ```
    python3 x86.py -p loop.s -t 1 -i 100 -R dx -c

    dx          Thread 0         
        0   
    -1   1000 sub  $1,%dx
    -1   1001 test $0,%dx
    -1   1002 jgte .top
    -1   1003 halt
    ```

2. Same code, different flags: (./x86.py -p loop.s -t 2 -i 100
-a
dx=3,dx=3 -R dx) This specifies two threads, and initializes each
%dx to 3. What values will %dx see? Run with -c to check. Does
the presence of multiple threads affect your calculations? Is there a
race in this code?

    - Although there are two threads, interupts don't occur frequently enough to affect execution. (No race condition)
    - dx would start at 3 and go to -1 in the first and second thread.
    - **since each thread has its own set of registers, the second thread also starts at 3 and goes down to -1**

3. Run this: ./x86.py -p loop.s -t 2 -i 3 -r -a dx=3,dx=3
-R dx This makes the interrupt interval small/random; use different seeds (-s) to see different interleavings. Does the interrupt
frequency change anything?
   -  The number of operations does not change. Each thread runs as normal since each thread has its own set of registers, and the program only interacts with registers.

4. Now, a different program, looping-race-nolock.s, which accesses a shared variable located at address 2000; we’ll call this variable value. Run it with a single thread to confirm your understanding: ./x86.py -p
looping-race-nolock.s -t 1 -M 2000 What is value (i.e.,
at memory address 2000) throughout the run? Use -c to check.

    ```
    python3 x86.py -p looping-race-nolock.s -t 1 -M 2000 -R ax,bx -c

    2000      ax    bx          Thread 0         
        0       0     0   
        0       0     0   1000 mov 2000, %ax
        0       1     0   1001 add $1, %ax
        1       1     0   1002 mov %ax, 2000
        1       1    -1   1003 sub  $1, %bx
        1       1    -1   1004 test $0, %bx
        1       1    -1   1005 jgt .top
        1       1    -1   1006 halt
    ```

5. Run with multiple iterations/threads: ./x86.py -p
looping-race-nolock.s -t 2 -a bx=3 -M 2000 Why does
each thread loop three times? What is final value of value?

    - because bx is initialized to 3

6. Run with random interrupt intervals: ./x86.py -p
looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 0 with
different seeds (-s 1, -s 2, etc.) Can you tell by looking at the
thread interleaving what the final value of value will be? Does the
timing of the interrupt matter? Where can it safely occur? Where
not? In other words, where is the critical section exactly?

   - the critical section is the first three lines of code
   - if an interrupt occurs during any one of these three lines and the scheduler runs one of the three lines in the other thread too, we will miss one increment
   - when interrupt occurs somewhere else: value at 2000 is 2 as expected 
    ```
    python3 x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 0
    2000      ax    bx          Thread 0                Thread 1         
        0       0     0   
        0       0     0   1000 mov 2000, %ax
        0       1     0   1001 add $1, %ax
        1       1     0   1002 mov %ax, 2000
        1       1    -1   1003 sub  $1, %bx
        1       0     0   ------ Interrupt ------  ------ Interrupt ------  
        1       1     0                            1000 mov 2000, %ax
        1       2     0                            1001 add $1, %ax
        2       2     0                            1002 mov %ax, 2000
        2       2    -1                            1003 sub  $1, %bx
        2       1    -1   ------ Interrupt ------  ------ Interrupt ------  
        2       1    -1   1004 test $0, %bx
        2       1    -1   1005 jgt .top
        2       2    -1   ------ Interrupt ------  ------ Interrupt ------  
        2       2    -1                            1004 test $0, %bx
        2       2    -1                            1005 jgt .top
        2       1    -1   ------ Interrupt ------  ------ Interrupt ------  
        2       1    -1   1006 halt
        2       2    -1   ----- Halt;Switch -----  ----- Halt;Switch -----  
        2       2    -1                            1006 halt
    ```
    - when intercept occurs in the critical section: value at 2000 is 1
    - note that regardless of how many times interrupts occur, the value will always be 1 when it occurs in the critical section (since only one run of the critical section).
    - generally, in a loop, the worst case is that the value will receive only half the number of increments that it should have if increments occur frequently (I think)
    ```
    python3 x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 1 -R ax,bx -c
    2000      ax    bx          Thread 0                Thread 1         
        0       0     0   
        0       0     0   1000 mov 2000, %ax
        0       0     0   ------ Interrupt ------  ------ Interrupt ------  
        0       0     0                            1000 mov 2000, %ax
        0       1     0                            1001 add $1, %ax
        1       1     0                            1002 mov %ax, 2000
        1       1    -1                            1003 sub  $1, %bx
        1       0     0   ------ Interrupt ------  ------ Interrupt ------  
        1       1     0   1001 add $1, %ax
        1       1     0   1002 mov %ax, 2000
        1       1    -1   1003 sub  $1, %bx
        1       1    -1   1004 test $0, %bx
        1       1    -1   ------ Interrupt ------  ------ Interrupt ------  
        1       1    -1                            1004 test $0, %bx
        1       1    -1                            1005 jgt .top
        1       1    -1   ------ Interrupt ------  ------ Interrupt ------  
        1       1    -1   1005 jgt .top
        1       1    -1   1006 halt
        1       1    -1   ----- Halt;Switch -----  ----- Halt;Switch -----  
        1       1    -1   ------ Interrupt ------  ------ Interrupt ------  
        1       1    -1                            1006 halt
    ```

1. Now examine fixed interrupt intervals: ./x86.py -p
looping-race-nolock.s -a bx=1 -t 2 -M 2000 -i 1 What
will the final value of the shared variable value be? What about
when you change -i 2, -i 3, etc.? For which interrupt intervals
does the program give the “correct” answer?

   - regardless of the interval of interrupts, as long as there is an interrupt somewhere in the critical section to run somewhere in the critical section in the other thread, the result will be that the value at 2000 misses one increment
   ```
        python3 x86.py -p looping-race-nolock.s -a bx=1 -t 2 -M 2000 -i 1 -c
        2000          Thread 0                Thread 1         
            0   
            0   1000 mov 2000, %ax
            0   ------ Interrupt ------  ------ Interrupt ------  
            0                            1000 mov 2000, %ax
            0   ------ Interrupt ------  ------ Interrupt ------  
            0   1001 add $1, %ax
            0   ------ Interrupt ------  ------ Interrupt ------  
            0                            1001 add $1, %ax
            0   ------ Interrupt ------  ------ Interrupt ------  
            1   1002 mov %ax, 2000
            1   ------ Interrupt ------  ------ Interrupt ------  
            1                            1002 mov %ax, 2000
            1   ------ Interrupt ------  ------ Interrupt ------  
            1   1003 sub  $1, %bx
            1   ------ Interrupt ------  ------ Interrupt ------  
            1                            1003 sub  $1, %bx
            1   ------ Interrupt ------  ------ Interrupt ------  
            1   1004 test $0, %bx
            1   ------ Interrupt ------  ------ Interrupt ------  
            1                            1004 test $0, %bx
            1   ------ Interrupt ------  ------ Interrupt ------  
            1   1005 jgt .top
            1   ------ Interrupt ------  ------ Interrupt ------  
            1                            1005 jgt .top
            1   ------ Interrupt ------  ------ Interrupt ------  
            1   1006 halt
            1   ----- Halt;Switch -----  ----- Halt;Switch -----  
            1   ------ Interrupt ------  ------ Interrupt ------  
            1                            1006 halt
    ```
   - when interrupt interval >= 3, the result is 2 ("correct")  
    ```
        python3 x86.py -p looping-race-nolock.s -a bx=1 -t 2 -M 2000 -i 3 -c

        2000          Thread 0                Thread 1         
            0   
            0   1000 mov 2000, %ax
            0   1001 add $1, %ax
            1   1002 mov %ax, 2000
            1   ------ Interrupt ------  ------ Interrupt ------  
            1                            1000 mov 2000, %ax
            1                            1001 add $1, %ax
            2                            1002 mov %ax, 2000
            2   ------ Interrupt ------  ------ Interrupt ------  
            2   1003 sub  $1, %bx
            2   1004 test $0, %bx
            2   1005 jgt .top
            2   ------ Interrupt ------  ------ Interrupt ------  
            2                            1003 sub  $1, %bx
            2                            1004 test $0, %bx
            2                            1005 jgt .top
            2   ------ Interrupt ------  ------ Interrupt ------  
            2   1006 halt
            2   ----- Halt;Switch -----  ----- Halt;Switch -----  
            2                            1006 halt
    ```

8. Run the same for more loops (e.g., set -a bx=100). What interrupt intervals (-i) lead to a correct outcome? Which intervals are
surprising?

   - Interval = 3 * n for some integer  n will lead to the correct outcome since we interrupt exactly at the points between the critical section and the non-critical section.

9. One last program: wait-for-me.s. Run: ./x86.py -p
wait-for-me.s -a ax=1,ax=0 -R ax -M 2000 This sets the
%ax register to 1 for thread 0, and 0 for thread 1, and watches %ax
and memory location 2000. How should the code behave? How is
the value at location 2000 being used by the threads? What will its
final value be?
   - Thread 0 sets the value at 2000 to be 1 (the "signaller"). Thread 1 waits until the value at 2000 is equal to 1 (the "waiter")
   - The final value of the value at 2000 should be 1
    ```
    2000      ax          Thread 0                Thread 1         
        0       1   
        0       1   1000 test $1, %ax
        0       1   1001 je .signaller
        1       1   1006 mov  $1, 2000
        1       1   1007 halt
        1       0   ----- Halt;Switch -----  ----- Halt;Switch -----  
        1       0                            1000 test $1, %ax
        1       0                            1001 je .signaller
        1       0                            1002 mov  2000, %cx
        1       0                            1003 test $1, %cx
        1       0                            1004 jne .waiter
        1       0                            1005 halt
    ```

1.  Now switch the inputs: ./x86.py -p wait-for-me.s -a
ax=0,ax=1 -R ax -M 2000 How do the threads behave? What
is thread 0 doing? How would changing the interrupt interval (e.g.,
-i 1000, or perhaps to use random intervals) change the trace outcome? Is the program efficiently using the CPU?
    -  Thread 0 now is the waiter. It will not exit the loop until the signaller has ran. Since there is no interrupt for 50 cycles, thread 0 runs until then. 
    -  If the interrupt is larger, there would be even more wasted processing (waiting) by thread 0.
    -  The program does not efficiently use the CPU. 

