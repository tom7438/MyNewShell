//
// Created by tom on 03/03/23.
//

#include "jobs.h"
#include "csapp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Tableau de jobs */
Job *jobs;

int initJobs() {
    jobs = malloc(sizeof(Job) * MAXJOBS);
    if(jobs == NULL) {
        perror("initjobs(): malloc");
        return -1;
    }
    /* Initialisation du tableau de jobs */
    for (int i = 0; i < MAXJOBS; i++) {
        jobs[i].pid = 0;
        jobs[i].numero = 0;
        jobs[i].status = TERMINE;
        jobs[i].commande[0] = '\0';
        jobs[i].mode = LIBRE;
    }
    return 0;
}

int joinCmd(char *cmd, char **seq) {
    int i = 0;
    cmd[0] = '\0';
    while (seq[i] != NULL) {
        if(i != 0)
            strcat(cmd, " ");
        strcat(cmd, seq[i]);
        i++;
    }
    return 0;
}

int addJob(pid_t pid, char **seq, Mode mode) {
    int i = 0;
    char cmd[MAXLINE];
    /* On récupère la commande sur un seul char* */
    joinCmd(cmd, seq);
    while (jobs[i].pid != 0 && i < MAXJOBS) {
        i++;
    }
    if (i == MAXJOBS) {
        fprintf(stderr, "addJob(): Nombre maximum de jobs atteint, processus %d non ajouté\n", pid);
        return -1;
    }
    jobs[i].pid = pid;
    jobs[i].numero = i + 1;
    jobs[i].status = EN_COURS;
    jobs[i].mode = mode;
    strcpy(jobs[i].commande, cmd);
    if(jobs[i].mode == BACKGROUND)
        printf("[%d] %d %s\n", jobs[i].numero, jobs[i].pid, jobs[i].commande);
#ifdef DEBUG
    printf("addJob(): Job %d ajouté pid = %d, cmd = %s\n", jobs[i].numero, jobs[i].pid, jobs[i].commande);
#endif
    return 0;
}

int numeroJob(pid_t pid) {
    int i = 0;
    while (jobs[i].pid != pid && i < MAXJOBS) {
        i++;
    }
    if (i == MAXJOBS) {
        fprintf(stderr, "numeroJob(): Processus %d introuvable\n", pid);
        return -1;
    }
    return jobs[i].numero;
}

Job *getJobPid(pid_t pid) {
    int i = 0;
    i = numeroJob(pid);
    if (i == -1) {
        fprintf(stderr, "getJobPid(): Processus %d introuvable\n", pid);
        return NULL;
    }
    return &jobs[i-1];
}

int updateJobPid(pid_t pid, Status status, Mode mode) {
    int i = 0;
    i = numeroJob(pid);
    if (i == -1) {
        fprintf(stderr, "updateJobPid(): Processus %d introuvable\n", pid);
        return -1;
    }
    /* On met à jour le job */
    getJobPid(pid)->status = status;
    getJobPid(pid)->mode = mode;
    /* On vérifie que le job a bien été mis à jour */
    if(jobs[i-1].status != status || jobs[i-1].mode != mode){
        fprintf(stderr, "updateJobPid(): Erreur lors de la mise à jour du job %d\n", pid);
        return -1;
    }
    return 0;
}

int printAllJobs() {
    char *status;
    char *mode;
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid != 0) {
            switch (jobs[i].status) {
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
            switch (jobs[i].mode) {
                case FOREGROUND:
                    mode = "Foreground";
                    break;
                case BACKGROUND:
                    mode = "Background";
                    break;
                case LIBRE:
                    mode = "Libre";
                    break;
                default:
                    mode = "Inconnu";
                    break;
            }
            printf("[%d] %d %s %s %s\n", jobs[i].numero, jobs[i].pid, status, jobs[i].commande, mode);
        }
    }
    return 0;
}

int deletejob(pid_t pid) {
    int i = 0;
    i = numeroJob(pid);
    if (i == -1) {
        fprintf(stderr, "deletejob(): Processus %d introuvable\n", pid);
        return -1;
    }
    char tmp[MAXLINE];
    Mode mode = jobs[i-1].mode;
    strcpy(tmp, jobs[i-1].commande);
    jobs[i-1].pid = 0;
    jobs[i-1].numero = 0;
    jobs[i-1].status = TERMINE;
    jobs[i-1].commande[0] = '\0';
    jobs[i-1].mode = LIBRE;
    /* Affichage du job terminé s'il est en arrière plan*/
    if(mode == BACKGROUND)
        fprintf(stdout, "[%d]+\tDone\t%s\n", i, tmp);
    return 0;
}

int nombreForeground() {
    int i = 0;
    int nb = 0;
    while (i < MAXJOBS) {
        if (jobs[i].pid != 0 && jobs[i].mode == FOREGROUND) {
            nb++;
        }
        i++;
    }
    return nb;
}

int killJobsForeground() {
    int i = 0;
    while (i < MAXJOBS) {
        if (jobs[i].pid != 0 && jobs[i].status == EN_COURS && jobs[i].mode == FOREGROUND) {
            Kill(jobs[i].pid, SIGTERM);
        }
        i++;
    }
    return 0;
}

int stopJobsForeground() {
    int i = 0;
    int signal = 0;
#ifdef DEBUG
    fprintf(stdout, "Jobs : \n");
    printAllJobs();
#endif
    while (i < MAXJOBS) {
        if (jobs[i].pid != 0 && jobs[i].status == EN_COURS && jobs[i].mode == FOREGROUND) {
            if(!signal) {
                Kill(-jobs[i].pid, SIGSTOP);
                signal = 1;
            }
            jobs[i].status = SUSPENDU;
            jobs[i].mode = BACKGROUND;
            fprintf(stdout, "[%d]+\tStopped\t%s\n", jobs[i].numero, jobs[i].commande);
        }
        i++;
    }
#ifdef DEBUG
    fprintf(stdout, "Jobs après avoir stoppé ceux en 1er plan : \n");
    printAllJobs();
#endif
    return 0;
}

int fg(char *num) {
    int numero = atoi(num);
    numero--;
    /* On récupère le numéro du groupe du job */
    pid_t ppid = getpgid(jobs[numero].pid);
    /* On vérifie que le numéro du job est valide */
    if (jobs[numero].pid != 0 && jobs[numero].status == SUSPENDU && jobs[numero].mode == BACKGROUND) {
        /* On envoie le signal SIGCONT au groupe du job */
        Kill(-jobs[numero].pid, SIGCONT);
    } else if (jobs[numero].pid != 0 && jobs[numero].status == EN_COURS && jobs[numero].mode == FOREGROUND) {
        fprintf(stdout, "fg: job %d déjà en premier plan\n", jobs[numero].numero);
        return -1;
    }
    int i = numero;
    while(i < MAXJOBS) {
        /* On met à jour les jobs du même groupe en arrière plan */
        if (jobs[i].pid != 0 && getpgid(jobs[i].pid) == ppid && jobs[i].status == SUSPENDU && jobs[i].mode == BACKGROUND) {
            jobs[i].status = EN_COURS;
            jobs[i].mode = FOREGROUND;
            fprintf(stdout, "[%d]+\tContinued\t%s\n", jobs[i].numero, jobs[i].commande);
        }
        i++;
    }
    return 0;
}

int bg(char *num) {
    int numero = atoi(num);
    numero--;
    /* On récupère le numéro du groupe du job */
    pid_t ppid = getpgid(jobs[numero].pid);
    /* On vérifie que le numéro du job est valide */
    if (jobs[numero].pid != 0 && jobs[numero].status == SUSPENDU && jobs[numero].mode == BACKGROUND) {
        /* On envoie le signal SIGCONT au groupe du job */
        Kill(-jobs[numero].pid, SIGCONT);
    } else if (jobs[numero].pid != 0 && jobs[numero].status == EN_COURS && jobs[numero].mode == BACKGROUND) {
        fprintf(stdout, "bg: job %d déjà en arrière plan\n", jobs[numero].numero);
        return -1;
    }
    int i = numero;
    while(i < MAXJOBS) {
        /* On met à jour les jobs du même groupe en arrière plan */
        if (jobs[i].pid != 0 && getpgid(jobs[i].pid) == ppid && jobs[i].status == SUSPENDU && jobs[i].mode == BACKGROUND) {
            jobs[i].status = EN_COURS;
            jobs[i].mode = BACKGROUND;
            fprintf(stdout, "[%d]+\tContinued\t%s\n", jobs[i].numero, jobs[i].commande);
        }
        i++;
    }
    return 0;
}

int stop(char *num) {
    int numero = atoi(num);
    numero--;
    /* On récupère le numéro du groupe du job */
    pid_t ppid = getpgid(jobs[numero].pid);
    if (jobs[numero].pid != 0 && jobs[numero].status == EN_COURS && jobs[numero].mode == BACKGROUND) {
        /* On envoie le signal SIGSTOP au groupe du job */
        Kill(-jobs[numero].pid, SIGSTOP);
    } else if (jobs[numero].pid != 0 && jobs[numero].status == SUSPENDU && jobs[numero].mode == BACKGROUND) {
        fprintf(stdout, "stop: job %d déjà stoppé\n", jobs[numero].numero);
        return -1;
    }
    int i = numero;
    while(i < MAXJOBS) {
        /* On met à jour les jobs du même groupe en arrière plan */
        if (jobs[i].pid != 0 && getpgid(jobs[i].pid) == ppid && jobs[i].status == EN_COURS && jobs[i].mode == BACKGROUND) {
            jobs[i].status = SUSPENDU;
            fprintf(stdout, "[%d]+\tStopped\t%s\n", jobs[i].numero, jobs[i].commande);
        }
        i++;
    }
    return 0;
}

int killAllJobs() {
    int i = 0;
    while (i < MAXJOBS) {
        if (jobs[i].pid != 0 && jobs[i].status == EN_COURS && jobs[i].mode == BACKGROUND) {
            /* On envoie le signal SIGTERM au groupe du job */
            Kill(jobs[i].pid, SIGTERM);
        }
        i++;
    }
    return 0;
}