1. First build main-race.c. Examine the code so you can see the (hopefully
obvious) data race in the code. Now run helgrind (by typing valgrind
--tool=helgrind main-race) to see how it reports the race. Does it
point to the right lines of code? What other information does it give to you?

    - Code:
    ```c++
    #include <stdio.h>

    #include "common_threads.h"

    int balance = 0;

    void* worker(void* arg) {
        balance++; // unprotected access 
        return NULL;
    }

    int main(int argc, char *argv[]) {
        pthread_t p;
        Pthread_create(&p, NULL, worker, NULL);
        balance++; // unprotected access
        Pthread_join(p, NULL);
        printf("%d", balance);
        return 0;
    }

    ```
    - Yes. 
    - Additionally, the address of the unprotected data access and the size of the data are given.
    ```
    valgrind --tool=helgrind ./main-race
    ==779== Helgrind, a thread error detector
    ==779== Copyright (C) 2007-2017, and GNU GPL'd, by OpenWorks LLP et al.
    ==779== Using Valgrind-3.15.0 and LibVEX; rerun with -h for copyright info
    ==779== Command: ./main-race
    ==779== 
    ==779== ---Thread-Announcement------------------------------------------
    ==779== 
    ==779== Thread #1 is the program's root thread
    ==779== 
    ==779== ---Thread-Announcement------------------------------------------
    ==779== 
    ==779== Thread #2 was created
    ==779==    at 0x499E282: clone (clone.S:71)
    ==779==    by 0x48612EB: create_thread (createthread.c:101)
    ==779==    by 0x4862E0F: pthread_create@@GLIBC_2.2.5 (pthread_create.c:817)
    ==779==    by 0x4842917: ??? (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_helgrind-amd64-linux.so)
    ==779==    by 0x109209: main (main-race.c:14)
    ==779== 
    ==779== ----------------------------------------------------------------
    ==779== 
    ==779== Possible data race during read of size 4 at 0x10C014 by thread #1
    ==779== Locks held: none
    ==779==    at 0x10922D: main (main-race.c:15)
    ==779== 
    ==779== This conflicts with a previous write of size 4 by thread #2
    ==779== Locks held: none
    ==779==    at 0x1091BE: worker (main-race.c:8)
    ==779==    by 0x4842B1A: ??? (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_helgrind-amd64-linux.so)
    ==779==    by 0x4862608: start_thread (pthread_create.c:477)
    ==779==    by 0x499E292: clone (clone.S:95)
    ==779==  Address 0x10c014 is 0 bytes inside data symbol "balance"
    ==779== 
    ==779== ----------------------------------------------------------------
    ==779== 
    ==779== Possible data race during write of size 4 at 0x10C014 by thread #1
    ==779== Locks held: none
    ==779==    at 0x109236: main (main-race.c:15)
    ==779== 
    ==779== This conflicts with a previous write of size 4 by thread #2
    ==779== Locks held: none
    ==779==    at 0x1091BE: worker (main-race.c:8)
    ==779==    by 0x4842B1A: ??? (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_helgrind-amd64-linux.so)
    ==779==    by 0x4862608: start_thread (pthread_create.c:477)
    ==779==    by 0x499E292: clone (clone.S:95)
    ==779==  Address 0x10c014 is 0 bytes inside data symbol "balance"
    ==779== 
    ==779== 
    ==779== Use --history-level=approx or =none to gain increased speed, at
    ==779== the cost of reduced accuracy of conflicting-access information
    ==779== For lists of detected and suppressed errors, rerun with: -s
    ==779== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)
    ``` 

2. What happens when you remove one of the offending lines of code? Now
add a lock around one of the updates to the shared variable, and then around
both. What does helgrind report in each of these cases?
    - When one of the accesses is removed, helgrind doesn't print anything (no races).
    - When we surround one of the memory acceses with a lock, unlock, there is still a possible race since just surrounding one of the accesses doesn't make the two accesses mutually exclusive.
    ```c++
    #include <stdio.h>

    #include "common_threads.h"

    int balance = 0;
    pthread_mutex_t lock;

    void* worker(void* arg) {
        pthread_mutex_lock(&lock);
        balance++; // unprotected access 
        pthread_mutex_unlock(&lock);
        return NULL;
    }

    int main(int argc, char *argv[]) {
        pthread_t p;
        Pthread_create(&p, NULL, worker, NULL);
        balance++; // unprotected access
        Pthread_join(p, NULL);
        printf("%d", balance);
        return 0;
    }
    ```
    - When we surround both of the accesses with the same lock, the race is no longer present.
    ```c++
    #include <stdio.h>

    #include "common_threads.h"

    int balance = 0;
    pthread_mutex_t lock;

    void* worker(void* arg) {
        pthread_mutex_lock(&lock);
        balance++; // unprotected access 
        pthread_mutex_unlock(&lock);
        return NULL;
    }

    int main(int argc, char *argv[]) {
        pthread_t p;
        Pthread_create(&p, NULL, worker, NULL);
        pthread_mutex_lock(&lock);
        balance++; // unprotected access
        pthread_mutex_unlock(&lock);
        Pthread_join(p, NULL);
        printf("%d", balance);
        return 0;
    }
    ```

3. Now let’s look at main-deadlock.c. Examine the code. This code has a
problem known as deadlock (which we discuss in much more depth in a
forthcoming chapter). Can you see what problem it might have?
    - The problem is that `p1` could obtain `m1` and `p2` could obtain `m2`. In order to run the next bit of code in `p1`, `m2` must be released. Similarly, `m1` needs to be released before running the next bit of code in `p2`. Code:
    ```c++
    #include <stdio.h>

    #include "common_threads.h"

    pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;

    void* worker(void* arg) {
        if ((long long) arg == 0) {
        Pthread_mutex_lock(&m1);
        Pthread_mutex_lock(&m2);
        } else {
        Pthread_mutex_lock(&m2);
        Pthread_mutex_lock(&m1);
        }
        Pthread_mutex_unlock(&m1);
        Pthread_mutex_unlock(&m2);
        return NULL;
    }

    int main(int argc, char *argv[]) {
        pthread_t p1, p2;
        Pthread_create(&p1, NULL, worker, (void *) (long long) 0);
        Pthread_create(&p2, NULL, worker, (void *) (long long) 1);
        Pthread_join(p1, NULL);
        Pthread_join(p2, NULL);
        return 0;
    }
    ```

4. Now run helgrind on this code. What does helgrind report?
    - lock order violated: `p2` tries to obtain `m2` then `m1` but this violates the required order established by `p1`: `m1` then `m2`.

5. Now run helgrind on main-deadlock-global.c. Examine the code;
does it have the same problem that main-deadlock.c has? Should helgrind
be reporting the same error? What does this tell you about tools like helgrind?
   - It shouldn't have the same problem since the global lock provides mutual exclusion between the two threads.
   - But valgrind/helgrind still reports a deadlock error, just based on the order of the lock acquisition.
   - So helgrind isn't always correct.
   - Code:
    ```c++
    #include <stdio.h>

    #include "common_threads.h"

    pthread_mutex_t g = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;

    void* worker(void* arg) {
        Pthread_mutex_lock(&g);
        if ((long long) arg == 0) {
        Pthread_mutex_lock(&m1);
        Pthread_mutex_lock(&m2);
        } else {
        Pthread_mutex_lock(&m2);
        Pthread_mutex_lock(&m1);
        }
        Pthread_mutex_unlock(&m1);
        Pthread_mutex_unlock(&m2);
        Pthread_mutex_unlock(&g);
        return NULL;
    }

    int main(int argc, char *argv[]) {
        pthread_t p1, p2;
        Pthread_create(&p1, NULL, worker, (void *) (long long) 0);
        Pthread_create(&p2, NULL, worker, (void *) (long long) 1);
        Pthread_join(p1, NULL);
        Pthread_join(p2, NULL);
        return 0;
    }
    ```

6. Let’s next look at main-signal.c. This code uses a variable (done) to
signal that the child is done and that the parent can now continue. Why is
this code inefficient? (what does the parent end up spending its time doing,
particularly if the child thread takes a long time to complete?)
    - This code is inefficient since the parent thread is just doing nothing during its time slices. Code: 
    ```c++
    #include <stdio.h>

    #include "common_threads.h"

    int done = 0;

    void* worker(void* arg) {
        printf("this should print first\n");
        done = 1;
        return NULL;
    }

    int main(int argc, char *argv[]) {
        pthread_t p;
        Pthread_create(&p, NULL, worker, NULL);
        while (done == 0)
        ;
        printf("this should print last\n");
        return 0;
    }
    ```

7. Now run helgrind on this program. What does it report? Is the code
correct?
    - helgrind says that there is a data race with done and print.
    - but the code is correct in the sense that "this should print first" will always print before "this should print last" since even if there is an interupt in the middle of the `done = 1` or `done == 0` instruction, `done = 1` needs to execute fully before the while loop exits
  
8. Now look at a slightly modified version of the code, which is found in
main-signal-cv.c. This version uses a condition variable to do the signaling (and associated lock). Why is this code preferred to the previous
version? Is it correctness, or performance, or both?
    - The code is more correct since there are no races
    - The code also performs better since we're not spin waiting anymore
    - Code:
    ```c++
    #include <stdio.h>

    #include "common_threads.h"

    // 
    // simple synchronizer: allows one thread to wait for another
    // structure "synchronizer_t" has all the needed data
    // methods are:
    //   init (called by one thread)
    //   wait (to wait for a thread)
    //   done (to indicate thread is done)
    // 
    typedef struct __synchronizer_t {
        pthread_mutex_t lock;
        pthread_cond_t cond;
        int done;
    } synchronizer_t;

    synchronizer_t s;

    void signal_init(synchronizer_t *s) {
        Pthread_mutex_init(&s->lock, NULL);
        Pthread_cond_init(&s->cond, NULL);
        s->done = 0;
    }

    void signal_done(synchronizer_t *s) {
        Pthread_mutex_lock(&s->lock);
        s->done = 1;
        Pthread_cond_signal(&s->cond);
        Pthread_mutex_unlock(&s->lock);
    }

    void signal_wait(synchronizer_t *s) {
        Pthread_mutex_lock(&s->lock);
        while (s->done == 0)
        Pthread_cond_wait(&s->cond, &s->lock);
        Pthread_mutex_unlock(&s->lock);
    }

    void* worker(void* arg) {
        printf("this should print first\n");
        signal_done(&s);
        return NULL;
    }

    int main(int argc, char *argv[]) {
        pthread_t p;
        signal_init(&s);
        Pthread_create(&p, NULL, worker, NULL);
        signal_wait(&s);
        printf("this should print last\n");

        return 0;
    }
    ```

1. Once again run helgrind on main-signal-cv. Does it report any errors?
    - helgrind reports no errors