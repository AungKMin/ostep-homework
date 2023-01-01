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
