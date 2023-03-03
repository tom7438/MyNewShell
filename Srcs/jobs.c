//
// Created by tom on 01/03/23.
//

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/types.h"
#include "jobs.h"
#include "csapp.h"
#include "string.h"

Job *jobs;

int initJobs(){
    jobs = malloc(sizeof(Job)*MAXJOBS);
    int i;
    for(i=0; i<MAXJOBS; i++){
        jobs[i].pid = 0;
        jobs[i].numero = 0;
        jobs[i].status = TERMINE;
        strcpy(jobs[i].commande, "");
        jobs[i].mode = LIBRE;
    }
    return 0;
}

int joinCmd(char *cmd, char **seq) {
    strcpy(cmd, "");
    while(*seq != NULL){
        strcat(cmd, *seq);
        strcat(cmd, " ");
        seq++;
    }
    return 0;
}

int addJob(pid_t pid, char **seq, Mode mode){
    int i;
    char cmd[MAXLINE];
    joinCmd(cmd, seq);
    printJobs();
#ifdef DEBUG
    fprintf(stderr, "Ajout du processus %d: %s\n", pid, cmd);
#endif
    for(i=0; i<MAXJOBS; i++){
        if(jobs[i].pid == 0){
            jobs[i].pid = pid;
            jobs[i].numero = i+1;
            jobs[i].status = EN_COURS;
            jobs[i].mode = mode;
            strcpy(jobs[i].commande, cmd);
            return 0;
        }
    }
    fprintf(stderr, "Erreur: Trop de processus en cours d'exécution\n");
    return -1;
}

int numeroJob(pid_t pid) {
    int i;
#ifdef DEBUG
    fprintf(stderr, "Recherche du processus %d\n", pid);
#endif
    for(i=0; i<MAXJOBS; i++){
        if(jobs[i].pid == pid){
            return i+1;
        }
    }
    fprintf(stderr, "Erreur: Processus %d non trouvé\n", pid);
    return -1;
}

Job *getJobPid(pid_t pid) {
    int i;
    i = numeroJob(pid);
    if(i != -1){
        return &jobs[i-1];
    } else {
        fprintf(stderr, "Erreur: Processus %d non trouvé\n", pid);
        return NULL;
    }
}

int printJobs(){
    int i;
    char *status;
    char *mode;
    for(i=0; i<MAXJOBS; i++){
        if(jobs[i].pid != 0){
            switch(jobs[i].status){
                case EN_COURS:
                    status = "En cours";
                    break;
                case TERMINE:
                    status = "Terminé";
                    break;
                case SUSPENDU:
                    status = "Suspendu";
                    break;
                default:
                    status = "Inconnu";
                    break;
            }
            switch(jobs[i].mode){
                case FOREGROUND:
                    mode = "Foreground";
                    break;
                case BACKGROUND:
                    mode = "Background";
                    break;
                default:
                    mode = "Inconnu";
                    break;
            }
            fprintf(stdout, "[%d] %d %s %s %s\n", jobs[i].numero, jobs[i].pid, status, jobs[i].commande, mode);
        }
    }
    return 0;
}

int deletejob(pid_t pid){
    int i = numeroJob(pid);
    if(i != -1){
        i--;
        jobs[i].pid = 0;
        jobs[i].numero = 0;
        jobs[i].status = TERMINE;
        strcpy(jobs[i].commande, "");
        jobs[i].mode = LIBRE;
#ifdef DEBUG
        fprintf(stderr, "Suppression du processus %d\n", pid);
#endif
        return 0;
    }
    fprintf(stderr, "Erreur: Processus %d non trouvé\n", pid);
    return -1;
}

int nombreForeground(){
    int i;
    int nb = 0;
    for(i=0; i<MAXJOBS; i++){
        if(jobs[i].mode == FOREGROUND){
            nb++;
        }
    }
#ifdef DEBUG
    fprintf(stderr, "Nombre de processus en foreground: %d\n", nb);
#endif
    return nb;
}