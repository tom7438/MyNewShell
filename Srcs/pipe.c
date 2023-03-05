//
// Created by tom on 23/02/23.
//
#include "pipe.h"
#include "csapp.h"
#include "CommandesInternes.h"
#include "readcmd.h"
#include "handler.h"
#include "jobs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int commande(struct cmdline * command) {
    /* Masques pour les signaux */
    sigset_t mask_all, mask_one, prev_one;
    Sigfillset(&mask_all);
    Sigemptyset(&mask_one);
    Sigaddset(&mask_one, SIGCHLD);

    /* Commande interne */
    if(command->seq[0] == NULL) {
        return 1;
    }

    /* Redirections */
    if(isCommandeInterne(command->seq[0][0])){
        executeCommandeInterne(command->seq[0][0], command->seq[0]);
    } else {
        if(command->in != NULL){
            if ((access(command->in, R_OK))){
                printf("%s: Permission denied entré\n", command->in);
                return 1;
            }
            int fdin = Open(command->in, O_RDONLY, 0);
            Dup2(fdin, STDIN_FILENO);
            Close(fdin);
        }
        if(command->out != NULL){
            if ((access(command->out, F_OK)==0) && (access(command->out, W_OK))){
                printf("%s: Permission denied sortie\n", command->out);
                return 1;
            }
            int fdout = Open(command->out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            Dup2(fdout, STDOUT_FILENO);
            Close(fdout);
        }
        /* Commande externe */
        pid_t pid;
        Sigprocmask(SIG_BLOCK, &mask_one, &prev_one); /* Block SIGCHLD */
        if((pid = Fork()) == 0){
            /* Fils */
            Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
            if(execvp(command->seq[0][0], command->seq[0]) < 0){
                printf("Commande externe non reconnue: %s\n", command->seq[0][0]);
                exit(1);
            }
        }
        /* Père */
        /* Changement gpid */
        setpgid(pid, pid);
        /* Ajout du job */
        Mode mode = command->background ? BACKGROUND : FOREGROUND;
        Sigprocmask(SIG_BLOCK, &mask_all, NULL); /* Parent process */
        addJob(pid, command->seq[0], mode);
        Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
    }
    /* Attente de la fin des processus en foreground */
    while(nombreForeground() > 0) {
        Sleep(1);
    }
    return 0;
}


int Mypipe(struct cmdline * command) {
    /* Masques pour les signaux */
    sigset_t mask_all, mask_one, prev_one;
    Sigfillset(&mask_all);
    Sigemptyset(&mask_one);
    Sigaddset(&mask_one, SIGCHLD);

    /* 2 commandes avec un pipe */
    int fd[2];
    pipe(fd);

    pid_t pid[2];

    if(isCommandeInterne(command->seq[0][0]) || isCommandeInterne(command->seq[1][0])) {
        fprintf(stderr, "Commande interne dans un pipe non supportée\n");
        exit(EXIT_FAILURE);
    }

    Sigprocmask(SIG_BLOCK, &mask_one, &prev_one); /* Block SIGCHLD */

    if((pid[0] = Fork()) == 0) {
        /* Fils */
        if(command->in != NULL){
            if ((access(command->in, R_OK))){
                printf("%s: Permission denied entré\n", command->in);
                return 1;
            }
            int fd_in = Open(command->in, O_RDONLY, 0);
            Dup2(fd_in, STDIN_FILENO);
            Close(fd_in);
        }
        Dup2(fd[1], STDOUT_FILENO);
        Close(fd[0]);
        Close(fd[1]);
        Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
        if(execvp(command->seq[0][0], command->seq[0]) < 0){
            printf("Commande externe non reconnue: %s\n", command->seq[0][0]);
            exit(1);
        }
    }
    else {
        /* Pere */
        /* Changement gpid */
        setpgid(pid[0], pid[0]);
        if((pid[1] = Fork()) == 0) {
            /* Fils 2 */
            if(command->out != NULL){
                if ((access(command->out, F_OK)==0) && (access(command->out, W_OK))){
                    printf("%s: Permission denied sortie\n", command->out);
                    return 1;
                }
                int fd_out = Open(command->out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                Dup2(fd_out, STDOUT_FILENO);
                Close(fd_out);
            }
            Dup2(fd[0], STDIN_FILENO);
            Close(fd[0]);
            Close(fd[1]);
            Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
            if(execvp(command->seq[1][0], command->seq[1]) < 0){
                printf("Commande externe non reconnue: %s\n", command->seq[1][0]);
                exit(1);
            }
        }
        else {
            /* Pere */
            /* Changement gpid */
            setpgid(pid[1], pid[0]);
            /* Ajout du job */
            Mode mode = command->background ? BACKGROUND : FOREGROUND;
            Sigprocmask(SIG_BLOCK, &mask_all, NULL); /* Parent process */
            addJob(pid[0], command->seq[0], mode);
            addJob(pid[1], command->seq[1], mode);
            Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
            Close(fd[0]);
            Close(fd[1]);
            /* Attente de la fin des processus en foreground */
            while(nombreForeground() > 0) {
                Sleep(1);
            }
        }
    }
    return 0;
}

int Multipipe(struct cmdline * command, int nbrcommande) {
    /* Masques pour les signaux */
    sigset_t mask_all, mask_one, prev_one;
    Sigfillset(&mask_all);
    Sigemptyset(&mask_one);
    Sigaddset(&mask_one, SIGCHLD);

    /* plusieurs pipes */
    int num_pipes = nbrcommande-1;
    int pipes[num_pipes][2];

    // Création des pipes
    for (int i = 0; i < num_pipes; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("Erreur lors de la création du pipe");
            return 1;
        }
    }

    /* Tableau des pid des fils */
    pid_t pid[nbrcommande];

    for (int i = 0; i<nbrcommande; i++){
        if(isCommandeInterne(command->seq[i][0])) {
            fprintf(stderr, "Commande interne non supportée avec plusieurs pipes\n");
            exit(EXIT_FAILURE);
        }
        Sigprocmask(SIG_BLOCK, &mask_one, &prev_one); /* Block SIGCHLD */
        if((pid[i] = Fork()) == 0) {
            /* Fils */
            closePipes(pipes, num_pipes, i);
            if (i==0){ // Si première commande
                if(command->in != NULL){    /* si redirection entrée*/
                    if ((access(command->in, R_OK))){
                        printf("%s: Permission denied entré\n", command->in);
                        return 1;
                    }
                    int fd_in = Open(command->in, O_RDONLY, 0);
                    Dup2(fd_in, STDIN_FILENO);
                    Close(fd_in);
                }
            }
            if (i==nbrcommande-1){ // Si dernière commande
                if(command->out != NULL){
                    if ((access(command->out, F_OK)==0) && (access(command->out, W_OK))){
                        printf("%s: Permission denied sortie\n", command->out);
                        return 1;
                    }
                    int fd_out = Open(command->out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    Dup2(fd_out, STDOUT_FILENO);
                    Close(fd_out);
                }
            }
            if(i!=nbrcommande-1) {  // si pas la dernière commande
                Dup2(pipes[i][1], STDOUT_FILENO);
                Close(pipes[i][1]);
            }
            if(i!=0) { // si pas la première commande
                Dup2(pipes[i-1][0], STDIN_FILENO);
                Close(pipes[i-1][0]);
            }
            // Exécution de la commande
            Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
            if(execvp(command->seq[i][0], command->seq[i]) < 0){
                printf("Commande externe non reconnue: %s\n", command->seq[0][0]);
                exit(1);
            }
        } else if (pid[i] < 0) {
            perror("Erreur lors du fork");
            return 1;
        } else {
            /* Père */
            /* Changement gpid */
#ifdef DEBUG
            fprintf(stderr, "pid[%d] = %d, pid[0] = %d\n", i, pid[i], pid[0]);
#endif
            setpgid(pid[i], pid[0]);
            /* Ajout du job */
            Mode mode = command->background ? BACKGROUND : FOREGROUND;
#ifdef DEBUG
            fprintf(stderr, "Ajout du job %d (%s) en %s\n", pid[i], command->seq[i][0], mode == BACKGROUND ? "BACKGROUND" : "FOREGROUND");
#endif
            Sigprocmask(SIG_BLOCK, &mask_all, NULL); /* Parent process */
            addJob(pid[i], command->seq[i], mode);
            Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
            if(i!=0) {
                Close(pipes[i-1][1]);
            }
            if(i>1) {
                Close(pipes[i-2][0]);
            }
            if(i==nbrcommande-1) {
                Close(pipes[i-1][0]);
            }
            /* Attente de la fin des processus en foreground */
            while(nombreForeground() > 0 && i == nbrcommande-1) {
                Sleep(1);
            }
        }
    }
    return 0;
}

int closePipes(int pipes[][2], int num_pipes, int numeroCommande) {
    int depart = 0;
    if(numeroCommande>1)
        depart = numeroCommande-1;
    for (int i = depart; i < num_pipes; i++) {
        if(i!=numeroCommande){
            Close(pipes[i][1]);
        }
        if(i!=numeroCommande-1){
            Close(pipes[i][0]);
        }
    }
    return 0;
}