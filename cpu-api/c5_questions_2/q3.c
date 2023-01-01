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
