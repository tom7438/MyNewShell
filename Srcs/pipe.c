//
// Created by tom on 23/02/23.
//
#include "../Headers/pipe.h"
#include "csapp.h"
#include "CommandesInternes.h"
#include "readcmd.h"

int resetStdinStdout(int oldin, int oldout) {
    if(oldin!=-1)
        Dup2(oldin, STDIN_FILENO);
    if(oldout!=-1)
        Dup2(oldout, STDOUT_FILENO);
    return 0;
}

int Mypipe(struct cmdline * command) {
    /* 2 commandes avec un pipe */
    int fd[2];
    pipe(fd);

    pid_t pid[2];
    int status;

    /* Permet de rétablir stdin et stdout */
    int oldin = -1;
    int oldout = -1;

    if((pid[0] = Fork()) == 0) {
        /* Fils */
        if(command->in != NULL){
            if ((access(command->in, R_OK))){
                printf("%s: Permission denied entré\n", command->in);
                return 1;
            }
            oldin = dup(STDIN_FILENO);
            int fd_in = Open(command->in, O_RDONLY, 0);
            Dup2(fd_in, STDIN_FILENO);
            Close(fd_in);
        }
        Dup2(fd[1], STDOUT_FILENO);
        Close(fd[0]);
        Close(fd[1]);
        if(isCommandeInterne(command->seq[0][0])) {
            executeCommandeInterne(command->seq[0][0], command->seq[0]);
            exit(0);
        } else if(execvp(command->seq[0][0], command->seq[0]) < 0){
            printf("Commande externe non reconnue: %s\n", command->seq[0][0]);
            return 1;
        }
    }
    else {
        /* Pere */
        if((pid[1] = Fork()) == 0) {
            /* Fils 2 */
            if(command->out != NULL){
                if ((access(command->out, F_OK)==0) && (access(command->out, W_OK))){
                    printf("%s: Permission denied sortie\n", command->out);
                    return 1;
                }
                oldout = dup(STDOUT_FILENO);
                int fd_out = Open(command->out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                Dup2(fd_out, STDOUT_FILENO);
                Close(fd_out);
            }
            Dup2(fd[0], STDIN_FILENO);
            Close(fd[0]);
            Close(fd[1]);
            if(isCommandeInterne(command->seq[1][0])) {
                executeCommandeInterne(command->seq[1][0], command->seq[1]);
                exit(0);
            } else if(execvp(command->seq[1][0], command->seq[1]) < 0){
                printf("Commande externe non reconnue: %s\n", command->seq[1][0]);
                return 1;
            }
        }
        else {
            /* Pere */
            Close(fd[0]);
            Close(fd[1]);
            Waitpid(pid[0], &status, 0);
            Waitpid(pid[1], &status, 0);
        }
    }
    resetStdinStdout(oldin, oldout);
    return 0;
}

int Multipipe(struct cmdline * command, int nbrcommande) {
    /* plusieur un pipe */
    int num_pipes = nbrcommande-1;
    int pipes[num_pipes][2];

    // Création des pipes
    for (int i = 0; i < num_pipes; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("Erreur lors de la création du pipe");
            return 1;
        }
    }

    int oldin = -1;
    int oldout = -1;

    pid_t pid[nbrcommande];
    int status;
    for (int i = 0; i<nbrcommande; i++){
        if((pid[i] = Fork()) == 0) {
            /* Fils */
            if (i==0){
                if(command->in != NULL){    /* si redirection entrée*/
                    if ((access(command->in, R_OK))){
                        printf("%s: Permission denied entré\n", command->in);
                        return 1;
                    }
                    oldin = dup(STDIN_FILENO);
                    int fd_in = Open(command->in, O_RDONLY, 0);
                    Dup2(fd_in, STDIN_FILENO);
                    Close(fd_in);
                }
                Close(pipes[0][0]);
                Dup2(pipes[0][1], STDOUT_FILENO);
                Close(pipes[0][1]);
                if(isCommandeInterne(command->seq[i][0])) {
                    executeCommandeInterne(command->seq[i][0], command->seq[0]);
                    exit(0);
                }
                else {
                    if(execvp(command->seq[i][0], command->seq[i]) < 0){
                        printf("Commande externe non reconnue: %s\n", command->seq[0][0]);
                        return 1;
                    }
                }
            }
            if (i==nbrcommande-1){
                if(command->out != NULL){
                    if ((access(command->out, F_OK)==0) && (access(command->out, W_OK))){
                        printf("%s: Permission denied sortie\n", command->out);
                        return 1;
                    }
                    oldout = dup(STDOUT_FILENO);
                    int fd_out = Open(command->out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    Dup2(fd_out, STDOUT_FILENO);
                    Close(fd_out);
                }
                Close(pipes[i-1][1]);
                Dup2(pipes[i-1][0], STDIN_FILENO);
                Close(pipes[i-1][0]);
                if(isCommandeInterne(command->seq[i][0])) {
                    executeCommandeInterne(command->seq[i][0], command->seq[0]);
                    exit(0);
                }
                else {
                    if(execvp(command->seq[i][0], command->seq[i]) < 0){
                        printf("Commande externe non reconnue: %s\n", command->seq[0][0]);
                        exit(0);
                    }
                }
            }
            else {
                Close(pipes[i-1][1]);
                Close(pipes[i][0]);
                Dup2(pipes[i-1][0], STDIN_FILENO);
                Close(pipes[i-1][0]);
                Dup2(pipes[i][1], STDOUT_FILENO);
                Close(pipes[i][1]);
                if(isCommandeInterne(command->seq[i][0])) {
                    executeCommandeInterne(command->seq[i][0], command->seq[0]);
                    exit(0);
                }
                else {
                    if(execvp(command->seq[i][0], command->seq[i]) < 0){
                        printf("Commande externe non reconnue: %s\n", command->seq[0][0]);
                        exit(0);
                    }
                }
            }
        }
        else {
            /* Pere */
            // Fermeture des pipes
            Close(pipes[i][0]);
            Close(pipes[i][1]);     
            Waitpid(pid[i], &status, 0);
        }
    }
    resetStdinStdout(oldin, oldout);
    return 0;
}

int commande(struct cmdline * command) {
    /* Commande interne */
    if(command->seq[0] == NULL) {
        return 1;
    }

    /* Permet de rétablir stdin et stdout */
    int oldin = -1;
    int oldout = -1;

    /* Redirections */
    if(command->in != NULL){
        if ((access(command->in, R_OK))){
            printf("%s: Permission denied entré\n", command->out);
            return 1;
        }
        oldin = dup(STDIN_FILENO);
        int fdin = Open(command->in, O_RDONLY, 0);
        Dup2(fdin, STDIN_FILENO);
        Close(fdin);
    }
    if(command->out != NULL){
        if (((access(command->out, F_OK) == 0)&&access(command->out, W_OK))){
            printf("%s: Permission denied sortie\n", command->in);
            return 1;
        }
        oldout = dup(STDOUT_FILENO);
        int fdout = Open(command->out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        Dup2(fdout, STDOUT_FILENO);
        Close(fdout);
    }
    if(isCommandeInterne(command->seq[0][0])){
        executeCommandeInterne(command->seq[0][0], command->seq[0]);
    } else {
        /* Commande externe */
        pid_t pid;
        int status;
        if((pid = Fork()) == 0){
            if(execvp(command->seq[0][0], command->seq[0]) < 0){
                printf("Commande externe non reconnue: %s\n", command->seq[0][0]);
                return 1;
            }
        }
        Waitpid(pid, &status, 0);
    }
    
    /* Reinitialiser stdin et stdout */
    if(oldin!=-1)
        Dup2(oldin, STDIN_FILENO);
    if(oldout!=-1)
        Dup2(oldout, STDOUT_FILENO);
    return 0;
}