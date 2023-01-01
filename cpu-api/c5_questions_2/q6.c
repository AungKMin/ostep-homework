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
