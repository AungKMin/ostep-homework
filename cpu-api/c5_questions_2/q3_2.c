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
