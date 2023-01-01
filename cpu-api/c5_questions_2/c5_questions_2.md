1. Write a program that calls fork(). Before calling fork(), have the
main process access a variable (e.g., x) and set its value to something (e.g., 100). What value is the variable in the child process?
What happens to the variable when both the child and parent change
the value of x?

        Ans: Value remains the same in both processes. If you change the value in one process, the value does not change in the other process (the two processes have separate stacks).

    ```c++
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <sys/wait.h>

    int main(int argc, char *argv[])
    {
        int value = 100;
        int rc = fork();
        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            // child
            printf("I am child (pid:%d); value = %d\n", (int)getpid(), value);
            printf("I am child (pid:%d); changing value\n", (int)getpid());
            value = 69;
            printf("I am child (pid:%d); value = %d\n", (int)getpid(), value);
        } else { 
            // parent
            wait(NULL);
            printf("I am parent of %d (pid:%d); value = %d\n", rc, (int)getpid(), value);
        }
        return 0;
    }
    ```

    ```sh
    ./q1.out
    I am child (pid:11094); value = 100
    I am child (pid:11094); changing value
    I am child (pid:11094); value = 69
    I am parent of 11094 (pid:11093); value = 100
    ```

2. Write a program that opens a file (with the open() system call)
and then calls fork() to create a new process. Can both the child
and parent access the file descriptor returned by open()? What
happens when they are writing to the file concurrently, i.e., at the same time?

        Ans: Note that open() returns a file descriptor: an index to an entry in the process's table of open file descriptors.
        Both the child and parent can access the descriptor returned by open. This is because the child copies everything about the parent process, including the file descriptor int and file descriptors table.
        When parent waits for child to complete before writing to file, it's written as expected.
        When parent doesn't wait for child to complete before writing to file, it's also written sequentially as expected depending on the order in which the processes run.

    ```c++
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/wait.h>
    #include <fcntl.h>

    int main(int argc, char *argv[])
    {
        int file_descriptor = open("./q2.output", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
        int rc = fork();
        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            // child
            printf("I am child (pid:%d) - file_descriptor int: %d\n", (int)getpid(), file_descriptor);
            char* child_str = "hello from child\n";
            write(file_descriptor, child_str, strlen(child_str));
        } else { 
            // parent
            int wc = wait(NULL);
            printf("wc: %d\n", wc);
            printf("I am parent of %d (pid:%d) - file_descriptor int: %d\n", rc, (int)getpid(), file_descriptor);
            char* parent_str = "hello from parent\n";
            write(file_descriptor, parent_str, strlen(parent_str));
        }
        return 0;
    }
    ```

    ```sh
    // terminal output:
    ./q2
    I am child (pid:4436) - file_descriptor int: 3
    wc: 4436
    I am parent of 4436 (pid:4435) - file_descriptor int: 3
    ```

    ```sh
    // q2.output
    hello from child
    hello from parent

    ```

    ```c++
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/wait.h>
    #include <fcntl.h>

    int main(int argc, char *argv[])
    {
        int file_descriptor = open("./q2.output", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
        int rc = fork();
        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            // child
            printf("I am child (pid:%d) - file_descriptor int: %d\n", (int)getpid(), file_descriptor);
            char* child_str = "hello from child\n";
            write(file_descriptor, child_str, strlen(child_str));
        } else { 
            // parent
            printf("I am parent of %d (pid:%d) - file_descriptor int: %d\n", rc, (int)getpid(), file_descriptor);
            char* parent_str = "hello from parent\n";
            write(file_descriptor, parent_str, strlen(parent_str));
        }
        return 0;
    }
    ```

    ```sh
    // terminal output
    ./q2
    I am parent of 5055 (pid:5054) - file_descriptor int: 3
    I am child (pid:5055) - file_descriptor int: 3
    ```

    ```sh
    // q2.output
    hello from parent
    hello from child

    ```

3. Write another program using fork(). The child process should
print “hello”; the parent process should print “goodbye”. You should
try to ensure that the child process always prints first; can you do
this without calling wait() in the parent?

     --- DOESN'T WORK ---
     Since child might go first, in which case, the signal would be sent to the parent before parent has had the chance to set up the handler. AND even if we set up the handler before the fork, if child goes first, it'll send the signal before parent even pauses, meaning the parent will just pause forever. We can really see this failing when we try to make child print goodbye instead.
    
    Ans: Use `sigaction` to define a signal handler for the parent. Use `pause` to pause the parent process until a signal is caught. Send a signal from the child process to the parent process using `kill`. We use the `SIGUSR1` signal.

    ```c++
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/wait.h>
    #include <fcntl.h>
    #include <signal.h>

    void catch_signal(int signal_num);

    void catch_signal(int signal_num) 
    { 
        // reset the signal handler again to catch signal next time 
        // (since in some systems, after a signal is caught, the handler is reset to the default one) 
        signal(SIGUSR1, catch_signal);
    }

    int main(int argc, char *argv[])
    {
        int rc = fork();
        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            // child
            printf("I am child (pid:%d)\n", (int)getpid());
            printf("hello\n");
            kill(getppid(), SIGUSR1);
        } else {
            // parent
            printf("I am parent of %d (pid:%d)\n", rc, (int)getpid());
            
            printf("Setting signal handler\n");
            struct sigaction sigaction1;
            sigaction1.sa_handler = catch_signal;
            sigaction(SIGUSR1, &sigaction1, NULL);

            printf("Pausing now\n");
            pause();

            printf("goodbye\n");
        }
        return 0;
    }
    ```

    ```sh
    I am parent of 2100 (pid:2099)
    Setting signal handler
    Pausing now
    I am child (pid:2100)
    hello
    goodbye
    ```

    ```c++
    // When we try to make child print goodbye instead:
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/wait.h>
    #include <fcntl.h>
    #include <signal.h>

    void catch_signal(int signal_num);

    void catch_signal(int signal_num) 
    { 
        // reset the signal handler again to catch signal next time 
        // (since in some systems, after a signal is caught, the handler is reset to the default one) 
        signal(SIGUSR1, catch_signal);
    }

    int main(int argc, char *argv[])
    {
        int rc = fork();
        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            // child
            printf("I am child (pid:%d)\n", (int)getpid());
            printf("Setting signal handler\n");

            struct sigaction sigaction1;
            sigaction1.sa_handler = catch_signal;
            sigaction(SIGUSR1, &sigaction1, NULL);
            
            printf("Pausing now\n");
            pause();

            printf("goodbye\n");
        } else {
            // parent
            printf("I am parent of %d (pid:%d)\n", rc, (int)getpid());
            printf("hello\n");
            kill(rc, SIGUSR1);
        }
        return 0;
    }
    ```

    ```sh
    // output for when child prints goodbye instead 
    ./q3
    I am parent of 4580 (pid:4579)
    hello
    ```

    ```c++ 
    // even when we set signal handler before fork:
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/wait.h>
    #include <fcntl.h>
    #include <signal.h>

    void catch_signal(int signal_num);

    void catch_signal(int signal_num) 
    { 
        // reset the signal handler again to catch signal next time 
        // (since in some systems, after a signal is caught, the handler is reset to the default one) 
        signal(SIGUSR1, catch_signal);
    }

    int main(int argc, char *argv[])
    {
        struct sigaction sigaction1;
        sigaction1.sa_handler = catch_signal;
        sigaction(SIGUSR1, &sigaction1, NULL);
        int rc = fork();
        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            // child
            printf("I am child (pid:%d)\n", (int)getpid());
            printf("Setting signal handler\n");

            
            printf("Pausing now\n");
            pause();

            printf("goodbye\n");
        } else {
            // parent
            printf("I am parent of %d (pid:%d)\n", rc, (int)getpid());
            printf("hello\n");
            kill(rc, SIGUSR1);
        }
        return 0;
    }
    ```

    ```sh
    ./q3
    I am parent of 4652 (pid:4651)
    hello
    I am child (pid:4652)
    Setting signal handler
    Pausing now
    ```

    Ans: instead: use a flag to track if the signal has been caught before:

    ```c++ 
    // when parent prints goodbye:
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/wait.h>
    #include <fcntl.h>
    #include <signal.h>

    void catch_signal(int signal_num);

    sig_atomic_t sig_flag = 0;
    void catch_signal(int signal_num) 
    { 
        // reset the signal handler again to catch signal next time 
        // (since in some systems, after a signal is caught, the handler is reset to the default one) 
        // but don't really need it here!
        struct sigaction sigaction1;
        sigaction1.sa_handler = catch_signal;
        sigaction(SIGUSR1, &sigaction1, NULL);

        sig_flag = 1;
    }

    int main(int argc, char *argv[])
    {
        printf("Setting signal handler in both parent and child\n");
        struct sigaction sigaction1;
        struct sigaction old;
        sigaction1.sa_handler = catch_signal;
        sigaction(SIGUSR1, &sigaction1, &old);

        int rc = fork();
        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            // child
            printf("I am child (pid:%d)\n", (int)getpid());

            // remove signal handler (set back to default)
            sigaction(SIGUSR1, &old, NULL);
            
            printf("hello\n");
            kill(getppid(), SIGUSR1);
        } else {
            // parent
            printf("I am parent of %d (pid:%d)\n", rc, (int)getpid());

            printf("Pausing now\n");
            while (sig_flag == 0) { 

            }

            printf("goodbye\n");
        }
        return 0;
    }
    ```

    ```sh
    ./q3
    Setting signal handler in both parent and child
    I am parent of 5064 (pid:5063)
    Pausing now
    I am child (pid:5064)
    hello
    goodbye
    ```

    ```c++
    // when child prints goodbye:
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/wait.h>
    #include <fcntl.h>
    #include <signal.h>

    void catch_signal(int signal_num);

    sig_atomic_t sig_flag = 0;
    void catch_signal(int signal_num) 
    { 
        // reset the signal handler again to catch signal next time 
        // (since in some systems, after a signal is caught, the handler is reset to the default one) 
        // but don't really need it here!
        struct sigaction sigaction1;
        sigaction1.sa_handler = catch_signal;
        sigaction(SIGUSR1, &sigaction1, NULL);

        sig_flag = 1;
    }

    int main(int argc, char *argv[])
    {
        printf("Setting signal handler in both parent and child\n");
        struct sigaction sigaction1;
        struct sigaction old;
        sigaction1.sa_handler = catch_signal;
        sigaction(SIGUSR1, &sigaction1, &old);

        int rc = fork();
        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            // child
            printf("I am child (pid:%d)\n", (int)getpid());
            
            printf("Pausing now\n");
            while (sig_flag == 0) { 

            }

            printf("goodbye\n");
        } else {
            // parent
            printf("I am parent of %d (pid:%d)\n", rc, (int)getpid());
            
            // remove signal handler (set back to default)
            sigaction(SIGUSR1, &old, NULL);
            
            printf("hello\n");
            kill(rc, SIGUSR1);
        }
        return 0;
    }
    ```

    ```sh
    ./q3
    Setting signal handler in both parent and child
    I am parent of 4957 (pid:4956)
    hello
    I am child (pid:4957)
    Pausing now
    goodbye
    ```

    Ans: using pipes instead - key is that if a pipe is empty, reading from the pipe will block the process until some data is written to the pipe (using write). So we call read in the parent process and write in the child process. If parent runs before child, the parent process will be blocked until child process runs and writes to pipe, unblocking the parent process.

    ```c++
    // parent prints goodbye
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/wait.h>

    int main(int argc, char *argv[])
    {
        int pipefd[2];
        // error with pipe
        if (pipe(pipefd) == -1) { 
            fprintf(stderr, "pipe failed\n");
            exit(1);
        }

        int rc = fork();
        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            // child
            printf("I am child (pid:%d)\n", (int)getpid());
            
            // close unnecessary file descriptor of the read pipe in the child process
            close(pipefd[0]);
            printf("hello\n");
            // write to the pipe, unblocking the pipe in the parent process
            write(pipefd[1], "x", 1);
            close(pipefd[1]);
        } else {
            // parent
            printf("I am parent of %d (pid:%d)\n", rc, (int)getpid());
            // close unnecessary file descriptor of the write pipe in the child process
            close(pipefd[1]);
            // read from the pipe, which would have nothing if child process hasn't ran yet => this will block parent process until something is written to pipe (done by child process after printing hello)
            char c;
            read(pipefd[0], &c, 1);
            printf("goodbye\n");
        }
        return 0;
    }
    ```

    ```sh
    ./q3_with_pipes
    I am parent of 8715 (pid:8714)
    I am child (pid:8715)
    hello
    goodbye
    ```

    ```c++
    // child prints goodbye
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/wait.h>

    int main(int argc, char *argv[])
    {
        int pipefd[2];
        // error with pipe
        if (pipe(pipefd) == -1) { 
            fprintf(stderr, "pipe failed\n");
            exit(1);
        }

        int rc = fork();
        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            // child
            printf("I am child (pid:%d)\n", (int)getpid());
            
            close(pipefd[1]);
            char c;
            read(pipefd[0], &c, 1);
            printf("goodbye\n");
        } else {
            // parent
            printf("I am parent of %d (pid:%d)\n", rc, (int)getpid());

            close(pipefd[0]);
            printf("hello\n");
            write(pipefd[1], "x", 1);
            close(pipefd[1]);
        }
        return 0;
    }
    ```

    ```sh
    ./q3_with_pipes 
    I am parent of 9212 (pid:9211)
    I am child (pid:9212)
    hello
    goodbye
    ```

4. Write a program that calls fork() and then calls some form of
exec() to run the program /bin/ls. See if you can try all of the
variants of exec(), including (on Linux) execl(), execle(),
execlp(), execv(), execvp(), and execvpe(). Why do
you think there are so many variants of the same basic call?

    Other notes:
     - We run `exec()` in the child process, which replaces the entire child process with the ls file execution. This is why the child process doesn't run any of the code after the block.
     - Note that there is no `execlpe`. Only `execvpe` is defined. BUT: you can't define a path via environment variables ("e") and find the file without specifying path ("p") since from `exec` man pages: *execvpe() searches for the program using the value of PATH from the caller's environment, not from the envp argument. If the specified filename includes a slash character, then PATH is ignored, and the file at the specified pathname is executed.*
     - We can use `fexec` to execute a file corresponding to a file descriptor
  
    Flags:
      - in each of the functions, the first argument is path of the file to execute
      - the first arugment after that is the filename of the file being executed
      - l: "l" means list the parameters of the command like a variadic function
      - v: "v" means pass the arguments as an array of pointers to strings
      - p: "p" means if the name of the file to execute doesn't include slashes ("/"), it will act like the terminal in trying to find the executable file
      - e: "e" means we pass in environment variables for this process image (if we don't use "e", the process by default uses the environment variables specified by `environ`)
        - we use the PATH environment variable in the code: which tells where to look for the file to execute if a path is not specified and only the file name is specified [this needs to be paired with "p" to be used]
        - documentation for environment variables: https://man7.org/linux/man-pages/man7/environ.7.html

    ```c++
    #define _GNU_SOURCE // for execvpe()
    #if __has_include(<features.h>)
    #  include <features.h> // __GNU_LIBRARY__
    #endif
    #include <fcntl.h> 
    #include <stdio.h> 
    #include <stdlib.h> 
    #include <sys/wait.h>
    #include <unistd.h> 

    int main(int argc, char *argv[])
    {

        char *exec_argv[] = {"ls", "-a", (char* )NULL};
        // doesn't actually do anything in this case
        char *envp[] = {"PATH=/bin:/usr/bin", NULL};

        // execl
        int rc = fork();
        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            // child
            printf("I am child (pid:%d)\n", (int)getpid());
            printf("Child execl:\n");
            // "l" means list the parameters of the command like a variadic function
            // the string after the first arugment is the name of the file to execute
            if (execl("/bin/ls", "ls", "-a", (char*)NULL) < 0) { 
                fprintf(stderr, "execl failed\n");
            }
        } else {
            // parent
            printf("I am parent of %d (pid:%d)\n", rc, (int)getpid());
        }


        // wait until child process (which has become ls by now) terminates
        if (wait(NULL) < 1) { 
            fprintf(stderr, "wait failed\n");
            exit(1);
        }
        printf("\n");

        // execlp
        rc = fork();
        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            // child
            printf("I am child (pid:%d)\n", (int)getpid());
            printf("Child execlp:\n");
            // "l" means list the parameters of the command like a variadic function
            // "p" means if the name of the file to execute doesn't include slashes ("/"), it will act like the terminal in trying to find the executable file
            // the string after the first arugment is the name of the file to execute
            if (execlp("ls", "ls", "-a", (char*)NULL) < 0) { 
                fprintf(stderr, "execlp failed\n");
            }
        } else {
            // parent
            printf("I am parent of %d (pid:%d)\n", rc, (int)getpid());
        }


        // wait until child process (which has become ls by now) terminates
        if (wait(NULL) < 1) { 
            fprintf(stderr, "wait failed\n");
            exit(1);
        }
        printf("\n");

        // execle
        rc = fork();
        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            // child
            printf("I am child (pid:%d)\n", (int)getpid());
            printf("Child execle:\n");
            // "l" means list the parameters of the command like a variadic function
            // "e" means we pass in environment variables for this process image (if we don't use "e", the process by default uses the environment variables specified by `environ`)
            // the string after the first arugment is the name of the file to execute
            if (execle("/bin/ls", "ls", "-a", (char*)NULL, envp) < 0) { 
                fprintf(stderr, "execle failed\n");
            }
        } else {
            // parent
            printf("I am parent of %d (pid:%d)\n", rc, (int)getpid());
        }


        // wait until child process (which has become ls by now) terminates
        if (wait(NULL) < 1) { 
            fprintf(stderr, "wait failed\n");
            exit(1);
        }
        printf("\n");

        // execv
        rc = fork();
        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            // child
            printf("I am child (pid:%d)\n", (int)getpid());
            printf("Child execv:\n");
            // "v" means pass the arguments as an array of pointers to strings
            // the string after the first arugment is the name of the file to execute
            if (execvp("/bin/ls", exec_argv) < 0) { 
                fprintf(stderr, "execvp failed\n");
            }
        } else {
            // parent
            printf("I am parent of %d (pid:%d)\n", rc, (int)getpid());
        }


        // wait until child process (which has become ls by now) terminates
        if (wait(NULL) < 1) { 
            fprintf(stderr, "wait failed\n");
            exit(1);
        }
        printf("\n");

        // execvp
        rc = fork();
        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            // child
            printf("I am child (pid:%d)\n", (int)getpid());
            printf("Child execvp:\n");
            // "v" means pass the arguments as an array of pointers to strings
            // "p" means if the name of the file to execute doesn't include slashes ("/"), it will act like the terminal in trying to find the executable file
            // the string after the first arugment is the name of the file to execute
            if (execvp("ls", exec_argv) < 0) { 
                fprintf(stderr, "execvp failed\n");
            }
        } else {
            // parent
            printf("I am parent of %d (pid:%d)\n", rc, (int)getpid());
        }


        // wait until child process (which has become ls by now) terminates
        if (wait(NULL) < 1) { 
            fprintf(stderr, "wait failed\n");
            exit(1);
        }
        printf("\n");

        // execve
        rc = fork();
        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            // child
            printf("I am child (pid:%d)\n", (int)getpid());
            printf("Child execve:\n");
            // "v" means pass the arguments as an array of pointers to strings
            // "e" means we pass in environment variables for this process image (if we don't use "e", the process by default uses the environment variables specified by `environ`)
            // the string after the first arugment is the name of the file to execute
            if (execve("/bin/ls", exec_argv, envp) < 0) { 
                fprintf(stderr, "execvpe failed\n");
            }
        } else {
            // parent
            printf("I am parent of %d (pid:%d)\n", rc, (int)getpid());
        }


        // wait until child process (which has become ls by now) terminates
        if (wait(NULL) < 1) { 
            fprintf(stderr, "wait failed\n");
            exit(1);
        }
        printf("\n");

        // execvpe
        rc = fork();
        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            // child
            printf("I am child (pid:%d)\n", (int)getpid());
            printf("Child execvpe:\n");
            // "v" means pass the arguments as an array of pointers to strings
            // "p" means if the name of the file to execute doesn't include slashes ("/"), it will act like the terminal in trying to find the executable file        
            // "e" means we pass in environment variables for this process image (if we don't use "e", the process by default uses the environment variables specified by `environ`)
            // the string after the first arugment is the name of the file to execute
            if (execvpe("ls", exec_argv, envp) < 0) { 
                fprintf(stderr, "execvpe failed\n");
            }
        } else {
            // parent
            printf("I am parent of %d (pid:%d)\n", rc, (int)getpid());
        }
        

        // wait until child process (which has become ls by now) terminates
        if (wait(NULL) < 1) { 
            fprintf(stderr, "wait failed\n");
            exit(1);
        }
        printf("\n");

        // fexecve
        rc = fork();
        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            // child
            printf("I am child (pid:%d)\n", (int)getpid());
            printf("Child fexecve:\n");
            // "f" means we indicate the file to execute using a file descriptor instead
            // "v" means pass the arguments as an array of pointers to strings
            // "e" means we pass in environment variables for this process image (if we don't use "e", the process by default uses the environment variables specified by `environ`)
            // the string after the first arugment is the name of the file to execute
            int fd;
            if ((fd = open("/bin/ls", O_PATH)) < 0) { 
                fprintf(stderr, "open failed\n");
                exit(1);
            }
            if (fexecve(fd, exec_argv, envp) < 0) { 
                fprintf(stderr, "fexecve failed\n");
            }
        } else {
            // parent
            printf("I am parent of %d (pid:%d)\n", rc, (int)getpid());
        }

        return 0;
    }
    ```

    ```sh
    ./q4
    I am parent of 25592 (pid:25591)
    I am child (pid:25592)
    Child execl:
    .  ..  Makefile  c5_questions_2.md  q1  q1.c  q2  q2.c  q2.output  q3  q3.c  q3_2.c  q3_with_pipes  q3_with_pipes.c  q4  q4.c  q5  q5.c  q6  q6.c

    I am parent of 25593 (pid:25591)
    I am child (pid:25593)
    Child execlp:
    .  ..  Makefile  c5_questions_2.md  q1  q1.c  q2  q2.c  q2.output  q3  q3.c  q3_2.c  q3_with_pipes  q3_with_pipes.c  q4  q4.c  q5  q5.c  q6  q6.c

    I am parent of 25594 (pid:25591)
    I am child (pid:25594)
    Child execle:
    .  ..  Makefile  c5_questions_2.md  q1  q1.c  q2  q2.c  q2.output  q3  q3.c  q3_2.c  q3_with_pipes  q3_with_pipes.c  q4  q4.c  q5  q5.c  q6  q6.c

    I am parent of 25595 (pid:25591)
    I am child (pid:25595)
    Child execv:
    .  ..  Makefile  c5_questions_2.md  q1  q1.c  q2  q2.c  q2.output  q3  q3.c  q3_2.c  q3_with_pipes  q3_with_pipes.c  q4  q4.c  q5  q5.c  q6  q6.c

    I am parent of 25596 (pid:25591)
    I am child (pid:25596)
    Child execvp:
    .  ..  Makefile  c5_questions_2.md  q1  q1.c  q2  q2.c  q2.output  q3  q3.c  q3_2.c  q3_with_pipes  q3_with_pipes.c  q4  q4.c  q5  q5.c  q6  q6.c

    I am parent of 25597 (pid:25591)
    I am child (pid:25597)
    Child execve:
    .  ..  Makefile  c5_questions_2.md  q1  q1.c  q2  q2.c  q2.output  q3  q3.c  q3_2.c  q3_with_pipes  q3_with_pipes.c  q4  q4.c  q5  q5.c  q6  q6.c

    I am parent of 25598 (pid:25591)
    I am child (pid:25598)
    Child execvpe:
    .  ..  Makefile  c5_questions_2.md  q1  q1.c  q2  q2.c  q2.output  q3  q3.c  q3_2.c  q3_with_pipes  q3_with_pipes.c  q4  q4.c  q5  q5.c  q6  q6.c

    I am parent of 25599 (pid:25591)
    I am child (pid:25599)
    Child fexecve:
    .  ..  Makefile  c5_questions_2.md  q1  q1.c  q2  q2.c  q2.output  q3  q3.c  q3_2.c  q3_with_pipes  q3_with_pipes.c  q4  q4.c  q5  q5.c  q6  q6.c
    ```


1. Now write a program that uses wait() to wait for the child process
to finish in the parent. What does wait() return? What happens if
you use wait() in the child?

    Ans: `wait()` returns the pid of the terminated child process. Calling `wait()` in the child process (which has no children processes) results in an error and thus calling `wait()` returns -1.

    ```c++
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/wait.h>

    int main(int argc, char *argv[])
    {
        int rc = fork();
        if (rc < 0) {
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) {
            // child (new process)
            printf("hello, I am child (pid:%d)\n", (int) getpid());
            printf("Child: calling wait:\n");
            int wc = wait(NULL);
            printf("child's wait wc: %d\n", wc);
        } else {
            // parent goes down this path (original process)
            int wc = wait(NULL);
            printf("hello, I am parent of %d (wc:%d) (pid:%d)\n",
            rc, wc, (int) getpid());
        }
        return 0;
    }
    ```

    ```sh
    ./q5
    hello, I am child (pid:25693)
    Child: calling wait:
    child's wait wc: -1
    hello, I am parent of 25693 (wc:25693) (pid:25692)
    ```

1. Write a slight modification of the previous program, this time using waitpid() instead of wait(). When would waitpid() be
useful?

    Ans: `waitpid()` allows you to specify which child process to wait upon and also what state changes to wait for (via the *options* argument). 

    ```c++
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/wait.h>

    int main(int argc, char *argv[])
    {
        int rc = fork();
        if (rc < 0) {
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) {
            // child (new process)
            printf("hello, I am child (pid:%d)\n", (int) getpid());
            printf("Child: calling wait:\n");
            // -1 means wait for any child process
            int wc = waitpid(-1, NULL, 0);
            printf("child's wait wc: %d\n", wc);
        } else {
            // parent goes down this path (original process)
            // wait for the child process we just forked specifically
            int wc = waitpid(rc, NULL, 0);
            printf("hello, I am parent of %d (wc:%d) (pid:%d)\n",
            rc, wc, (int) getpid());
        }
        return 0;
    }
    ```
    
    ```sh
    ./q6
    hello, I am child (pid:25774)
    Child: calling wait:
    child's wait wc: -1
    hello, I am parent of 25774 (wc:25774) (pid:25773)
    ```

1. Write a program that creates a child process, and then in the child
closes standard output (STDOUT FILENO). What happens if the child
calls printf() to print some output after closing the descriptor?

    Ans: `printf()` just does nothing. 

    ```c++
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/wait.h>
    #include <fcntl.h>

    int main(int argc, char *argv[])
    {
        int rc = fork();
        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            // child
            printf("I am child (pid:%d)\n", (int)getpid());
            printf("Closing STDOUT_FILENO\n");
            close(STDOUT_FILENO);
            printf("Child trying to printf\n");
        } else { 
            // parent
            printf("I am parent of %d (pid:%d)\n", rc, (int)getpid());
        }
        return 0;
    }
    ```

    ```sh
    ./q7
    I am parent of 26657 (pid:26656)
    I am child (pid:26657)
    Closing STDOUT_FILENO
    ```

8.  Write a program that creates two children, and connects the standard output of one to the standard input of the other, using the
pipe() system call.

    Ans: We can use `dup2()` to make the file **descriptors** `STDOUT_FILENO` and `pipefd[1]`, the write end, point to the same file **description**: the pipe write end. Likewise, we also make `STDIN_FILENO` and `pipefd[0]`, the read end, point to the same file **description**: the pipe read end. Note that `dup2()` silently closes the `STDIN_FILENO` file descriptor before reassigning it to point to the new file description. 
    
    To Note: `close()` closes the file **descriptor** so that it no longer refers to a file **description**. It doesn't close the file itself. Only when we have closed the last file descriptor referring to the underlying file description that the resources associated with the open file description are freed.

    To Note: `BUFSIZ` is the size of the standard input/output stream buffer. Since we used printf in child1, the data in the pipe buffer can only be as big as `BUFSIZ`. 

    ```c++
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/wait.h>

    int main(int argc, char *argv[])
    {
        int pipefd[2];
        // error with pipe
        if (pipe(pipefd) == -1) { 
            fprintf(stderr, "pipe failed\n");
            exit(1);
        }

        int rc[2];

        rc[0] = fork();
        if (rc[0] < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork 1 failed\n");
            exit(1);
        } else if (rc[0] == 0) { 
            // child 1: will write to pipe
            printf("I am child 1 (pid:%d)\n", (int)getpid());
            
            // close unnecessary file descriptor of the read pipe in the child process
            close(pipefd[0]);

            // make STDOUT_FILENO, a file descriptor, point to the same file description as pipefd[1]
            if (dup2(pipefd[1], STDOUT_FILENO) != STDOUT_FILENO) { 
                fprintf(stderr, "dup2 failed in child 1\n");
                exit(1);
            }
            // NOTE: close() closes the file descriptor, not the file description itself
            close(pipefd[1]);

            printf("Child 1 (pid:%d): hello", (int)getpid());
        } else {
            // parent
            printf("I am parent of %d (pid:%d)\n", rc[0], (int)getpid());
            rc[1] = fork();
            if (rc[1] < 0) { 
                // fork failed; exit
                fprintf(stderr, "fork 2 failed\n");
                exit(1);
            } else if (rc[1] == 0) { 
                // child 2: will read from pipe
                printf("I am child 2 (pid:%d)\n", (int)getpid());

                // close unnecessary file descriptor of the write pipe in the child process
                close(pipefd[1]); 

                // make STDIN_FILENO, a file descriptor, point to the same file description as pipefd[0]
                if (dup2(pipefd[0], STDIN_FILENO) != STDIN_FILENO) { 
                    fprintf(stderr, "dup2 failed in child 2\n");
                    exit(1);
                }          
                // NOTE: close() closes the file descriptor, not the file description itself
                close(pipefd[0]);

                // BUFSIZ is the size of the standard input/output stream buffer 
                // since we used printf in child1, the pipe buffer can only be as big as BUFSIZ
                char buf[BUFSIZ];
                read(STDIN_FILENO, buf, BUFSIZ);
                printf("Child 2 (pid:%d): hello; Child 1 told me this: \"%s\"\n", getppid(), buf);
            } else { 
                printf("I am parent of %d (pid:%d)\n", rc[1], (int)getpid());

                if (waitpid(rc[0], NULL, 0) < 0) { 
                    fprintf(stderr, "Child 1 wait failed\n");
                    exit(1);
                }
                if (waitpid(rc[1], NULL, 0) < 0) { 
                    fprintf(stderr, "Child 2 wait failed\n");
                    exit(1);
                }
            }
        }
        return 0;
    }
    ```

    ```sh
    ./q8
    I am parent of 673 (pid:672)
    I am child 1 (pid:673)
    I am parent of 674 (pid:672)
    I am child 2 (pid:674)
    Child 2 (pid:672): hello; Child 1 told me this: "Child 1 (pid:673): hello"
    ```
