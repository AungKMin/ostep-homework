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