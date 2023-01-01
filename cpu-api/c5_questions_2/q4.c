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
