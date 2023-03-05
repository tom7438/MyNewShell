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
    /* Bloque tous les signaux */
    sigset_t mask_all, prev_all;
    Sigfillset(&mask_all);
    Sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
    /* Kill all jobs in foreground */
    /* Section critique */
    killJobsForeground();
    /* Fin section critique */
    /* Débloque tous les signaux */
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
        /* Bloque tous les signaux */
        Sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
        /* Section critique */
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
        /* Fin section critique */
        /* Débloque tous les signaux */
        Sigprocmask(SIG_SETMASK, &prev_all, NULL);
    }
    errno = olderrno;
}

void sigtstp_handler(int sig) {
    int olderrno = errno;
    /* Bloque tous les signaux */
    sigset_t mask_all, prev_all;
    Sigfillset(&mask_all);
    Sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
    /* Section critique */
    /* Stop tous les jobs en foreground */
    stopJobsForeground();
    /* Fin section critique */
    /* Débloque tous les signaux */
    Sigprocmask(SIG_SETMASK, &prev_all, NULL);
    errno = olderrno;
}