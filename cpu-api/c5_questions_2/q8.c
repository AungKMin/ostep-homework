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
