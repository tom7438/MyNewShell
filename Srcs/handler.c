//
// Created by tom on 22/02/23.
//
#include "handler.h"
#include "csapp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

volatile int sigint_flag = 0;

void sigint_handler(int sig) {
    printf("Ctrl-C pressed\n");
    sigint_flag = 1;
}

void sigchld_handler(int sig) {
    int olderrno = errno;
    //sigset_t mask_all, prev_all;
    pid_t pid;
    //Sigfillset(&mask_all);
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG|WUNTRACED)) > 0) { /* Reap child */
#if DEBUG
        if(WIFEXITED(status)){
            printf("Child %d terminated normally with status=%d\n", pid, WEXITSTATUS(status));
        } else if(WIFSIGNALED(status)){
            printf("Child %d terminated by signal %d\n", pid, WTERMSIG(status));
        } else if(WIFSTOPPED(status)){
            printf("Child %d stopped by signal %d\n", pid, WSTOPSIG(status));
        }
#endif
        //Sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
        //deletejob(pid); /* Delete the child from the job list */
        //Sigprocmask(SIG_SETMASK, &prev_all, NULL);
    }
    /*
    if (errno != ECHILD)
        Sio_error("waitpid error");
        */
    errno = olderrno;
}