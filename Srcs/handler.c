//
// Created by tom on 22/02/23.
//
#include "handler.h"
#include "csapp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "jobs.h"
#include "pipe.h"

void sigint_handler(int sig) {
    printf("Ctrl-C pressed\n");
    //sigint_flag = 1;
}

void sigchld_handler(int sig) {
    int olderrno = errno;
    sigset_t mask_all, prev_all;
    pid_t pid;
    Sigfillset(&mask_all);
    int status;
    int options = WNOHANG | WUNTRACED;
    while ((pid = waitpid(-1, &status, options)) > 0) { /* Reap child */
        Sigprocmask(SIG_BLOCK, &mask_all, &prev_all); /* Block SIGCHLD */
        if(WIFEXITED(status)){
            // Terminaison normale
            //getJobPid(pid)->status = TERMINE;
            fprintf(stderr, "Job [%d] (%d) terminated normally\n", getJobPid(pid)->numero, pid);
            deletejob(pid);
        } else if(WIFSIGNALED(status)){
            // Terminaison par un signal
            //getJobPid(pid)->status = TERMINE;
            fprintf(stderr, "Job [%d] (%d) terminated by signal %d\n", getJobPid(pid)->numero, pid, WTERMSIG(status));
            deletejob(pid);
        } else if(WIFSTOPPED(status)){
            // Suspendu par un signal
            getJobPid(pid)->status = SUSPENDU;
            fprintf(stderr, "Job [%d] (%d) stopped by signal %d\n", getJobPid(pid)->numero, pid, WSTOPSIG(status));
        }
        printJobs();
        Sigprocmask(SIG_SETMASK, &prev_all, NULL); /* Unblock SIGCHLD */
    }
    /*
    if (errno != ECHILD)
        Sio_error("waitpid error");
    */
    errno = olderrno;
}