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
