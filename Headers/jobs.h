//
// Created by tom on 01/03/23.
//

#ifndef MYNEWSHELL_JOBS_H
#define MYNEWSHELL_JOBS_H

#include "csapp.h"

#define MAXJOBS 10

typedef enum {
    EN_COURS,
    TERMINE,
    SUSPENDU,
} Status;

typedef enum {
    FOREGROUND,
    BACKGROUND,
    LIBRE
} Mode;

typedef struct {
    pid_t pid;
    int numero;
    Status status;
    char commande[MAXLINE];
    Mode mode;
} Job;

int initJobs();

int joinCmd(char *cmd, char **seq);

int addJob(pid_t pid, char **seq, Mode mode);

int numeroJob(pid_t pid);

Job *getJobPid(pid_t pid);

int printJobs();

int deletejob(pid_t pid);

int nombreForeground();

#endif //MYNEWSHELL_JOBS_H
