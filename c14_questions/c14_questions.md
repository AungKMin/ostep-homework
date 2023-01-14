1. First, write a simple program called null.c that creates a pointer
to an integer, sets it to NULL, and then tries to dereference it. Compile this into an executable called null. What happens when you
run this program?

    Deferencing the NULL pointer (getting the value that the pointer is pointing to) results in `Segementation Fault` but `free`ing the pointer runs just fine.

    ```c++
    #include <stdlib.h>
    #include <stdio.h>

    int main () { 
        int* pointer;
        pointer = NULL;
        printf("%d\n", *pointer);
        free(pointer);
        return 0;
    }
    ```

2. Next, compile this program with symbol information included (with
the -g flag). Doing so let’s put more information into the executable, enabling the debugger to access more useful information
about variable names and the like. Run the program under the debugger by typing gdb null and then, once gdb is running, typing
run. What does gdb show you?

    ```
    Starting program: /home/aung/OSTEP/ostep-homework/c14_questions/q1 

    Program received signal SIGSEGV, Segmentation fault.
    0x0000555555555181 in main () at q1.c:7
    7           printf("%d\n", *pointer);
    ```

3. Finally, use the valgrind tool on this program. We’ll use the memcheck
tool that is a part of valgrind to analyze what happens. Run
this by typing in the following: valgrind --leak-check=yes
null. What happens when you run this? Can you interpret the
output from the tool?

    ```
    ==1587== Invalid read of size 4
    ==1587==    at 0x109181: main (q1.c:7)
    ==1587==  Address 0x0 is not stack'd, malloc'd or (recently) free'd
    ==1587== 
    ==1587== 
    ==1587== Process terminating with default action of signal 11 (SIGSEGV)
    ==1587==  Access not within mapped region at address 0x0
    ==1587==    at 0x109181: main (q1.c:7)
    ==1587==  If you believe this happened as a result of a stack
    ==1587==  overflow in your program's main thread (unlikely but
    ==1587==  possible), you can try to increase the size of the
    ==1587==  main thread stack using the --main-stacksize= flag.
    ==1587==  The main thread stack size used in this run was 8388608.
    ==1587== 
    ==1587== HEAP SUMMARY:
    ==1587==     in use at exit: 0 bytes in 0 blocks
    ==1587==   total heap usage: 0 allocs, 0 frees, 0 bytes allocated
    ==1587== 
    ==1587== All heap blocks were freed -- no leaks are possible
    ==1587== 
    ==1587== For lists of detected and suppressed errors, rerun with: -s
    ==1587== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
    Segmentation fault
    ```

4. Write a simple program that allocates memory using malloc() but
forgets to free it before exiting. What happens when this program
runs? Can you use gdb to find any problems with it? How about
valgrind (again with the --leak-check=yes flag)?

    It runs without any errors, but there is a memory leak.
    
    ```c++
    #include <stdlib.h>

    int main () { 
        int* pointer = (int*)malloc(sizeof(int));
        return 0;
    }
    ```

    ```
    ==1967== HEAP SUMMARY:
    ==1967==     in use at exit: 4 bytes in 1 blocks
    ==1967==   total heap usage: 1 allocs, 0 frees, 4 bytes allocated
    ==1967== 
    ==1967== 4 bytes in 1 blocks are definitely lost in loss record 1 of 1
    ==1967==    at 0x483B7F3: malloc (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
    ==1967==    by 0x10915E: main (q4.c:4)
    ==1967== 
    ==1967== LEAK SUMMARY:
    ==1967==    definitely lost: 4 bytes in 1 blocks
    ==1967==    indirectly lost: 0 bytes in 0 blocks
    ==1967==      possibly lost: 0 bytes in 0 blocks
    ==1967==    still reachable: 0 bytes in 0 blocks
    ==1967==         suppressed: 0 bytes in 0 blocks
    ==1967== 
    ==1967== For lists of detected and suppressed errors, rerun with: -s
    ==1967== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
    ```
5. Write a program that creates an array of integers called data of size
100 using malloc; then, set data[100] to zero. What happens
when you run this program? What happens when you run this
program using valgrind? Is the program correct?

    Running it: behaves as expected. With Valgrind: Valgrind points out the error (invalid write).

    ```c++
    #include <stdlib.h>
    #include <stdio.h>

    int main () {
        int *data = (int *)malloc(100 * sizeof(int));
        data[100] = 0; 
        printf("%d\n", data[100]);
        free(data);
        return 0;
    }
    ```

    ```
    0
    ```
    ```
    // Valgrind
    ==2400== Command: ./q5
    ==2400== 
    ==2400== Invalid write of size 4
    ==2400==    at 0x1091AD: main (q5.c:6)
    ==2400==  Address 0x4a481d0 is 0 bytes after a block of size 400 alloc'd
    ==2400==    at 0x483B7F3: malloc (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
    ==2400==    by 0x10919E: main (q5.c:5)
    ==2400== 
    ==2400== Invalid read of size 4
    ==2400==    at 0x1091BD: main (q5.c:7)
    ==2400==  Address 0x4a481d0 is 0 bytes after a block of size 400 alloc'd
    ==2400==    at 0x483B7F3: malloc (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
    ==2400==    by 0x10919E: main (q5.c:5)
    ==2400== 
    0
    ==2400== 
    ==2400== HEAP SUMMARY:
    ==2400==     in use at exit: 0 bytes in 0 blocks
    ==2400==   total heap usage: 2 allocs, 2 frees, 1,424 bytes allocated
    ==2400== 
    ==2400== All heap blocks were freed -- no leaks are possible
    ==2400== 
    ==2400== For lists of detected and suppressed errors, rerun with: -s
    ==2400== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)
    ```

6. Create a program that allocates an array of integers (as above), frees
them, and then tries to print the value of one of the elements of
the array. Does the program run? What happens when you use
valgrind on it?

    Just running it: the data is still in the array, and accessing it works (no error). Using valgrind: points out the error (invalid read).

    ```c++
    #include <stdio.h>
    #include <stdlib.h>

    int main () {
        int * data = (int *)malloc(100 * sizeof(int));
        data[0] = 0;
        printf("%d\n", data[0]);
        free(data);
        printf("%d\n", data[0]);
        return 0;
    }
    ```

7. Now pass a funny value to free (e.g., a pointer in the middle of the
array you allocated above). What happens? Do you need tools to
find this type of problem?

    `free()` complains about an invalid pointer. Valgrind points out where this free error is.

    ```c++
    #include <stdlib.h>

    int main () {
        int * data = (int *)malloc(100 * sizeof(int));
        free(data + 4);
        return 0;
    }
    ```

    ```
    ./q7
    free(): invalid pointer
    Aborted
    ```

    ```
    // valgrind
    ==3132== Command: ./q7
    ==3132== 
    ==3132== Invalid free() / delete / delete[] / realloc()
    ==3132==    at 0x483CA3F: free (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
    ==3132==    by 0x109192: main (q7.c:5)
    ==3132==  Address 0x4a48050 is 16 bytes inside a block of size 400 alloc'd
    ==3132==    at 0x483B7F3: malloc (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
    ==3132==    by 0x10917E: main (q7.c:4)
    ==3132== 
    ```

8. Try out some of the other interfaces to memory allocation. For example, create a simple vector-like data structure and related routines that use realloc() to manage the vector. Use an array to
store the vectors elements; when a user adds an entry to the vector, use realloc() to allocate more space for it. How well does
such a vector perform? How does it compare to a linked list? Use
valgrind to help you find bugs.

    If we double the allocation everytime the vector becomes full and half the allocation everytime the number of actual elements become small enough compared to the current allocation, the ammortized time for pushing to the array and deleting from the array would be O(1).

9. Spend more time and read about using gdb and valgrind. Knowing your tools is critical; spend the time and learn how to become
an expert debugger in the UNIX and C environment.