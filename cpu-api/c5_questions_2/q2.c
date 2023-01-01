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
