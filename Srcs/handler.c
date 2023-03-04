//
// Created by tom on 22/02/23.
//
#include "handler.h"
#include "csapp.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "jobs.h"

void sigint_handler(int sig) {
    int olderrno = errno;
    sigset_t mask_all, prev_all;
    Sigfillset(&mask_all);
    Sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
    killJobsForeground();
    Sigprocmask(SIG_SETMASK, &prev_all, NULL);
    errno = olderrno;
}

void sigchld_handler(int sig) {
    int olderrno = errno;
    sigset_t mask_all, prev_all;
    pid_t pid;
    int status;
    Sigfillset(&mask_all);
    while ((pid = waitpid(-1, &status, WNOHANG|WUNTRACED)) > 0) { // Reap child
        Sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
        if(WIFEXITED(status)){
            deletejob(pid); // Delete the child from the job list
        }
        else if(WIFSIGNALED(status)){
#ifdef DEBUG
            printf("Job [%d] (%d) terminé par le signal %d\n", numeroJob(pid), pid, WTERMSIG(status));
#endif
            deletejob(pid); // Delete the child from the job list
        }
        else if(WIFSTOPPED(status)){
#ifdef DEBUG
            printf("Job [%d] (%d) suspendu par le signal %d\n", numeroJob(pid), pid, WSTOPSIG(status));
#endif
            updateJobPid(pid, SUSPENDU, getJobPid(pid)->mode);
        }
        Sigprocmask(SIG_SETMASK, &prev_all, NULL);
    }
    /*
    if (errno != ECHILD)
        Sio_error("waitpid error");
    */
    errno = olderrno;
}

void sigtstp_handler(int sig) {
    int olderrno = errno;
    sigset_t mask_all, prev_all;
    Sigfillset(&mask_all);
    Sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
    stopJobsForeground();
    Sigprocmask(SIG_SETMASK, &prev_all, NULL);
    errno = olderrno;
}