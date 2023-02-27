//
// Created by tom on 23/02/23.
//
#include "../Headers/pipe.h"
#include "csapp.h"
#include "CommandesInternes.h"
#include "readcmd.h"

int Mypipe(struct cmdline * command) {
    /* 2 commandes avec un pipe */
    int fd[2];
    pipe(fd);

    pid_t pid[2];
    int status;

    if((pid[0] = Fork()) == 0) {
        /* Fils */
        if(command->in != NULL){
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
            exit(0);
        }
    }
    else {
        /* Pere */
        if((pid[1] = Fork()) == 0) {
            /* Fils 2 */
            if(command->out != NULL){
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
                exit(0);
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
    return 0;
}

int Multipipe(struct cmdline * command, int i) {
    /* plusieur un pipe */
    int num_pipes = i-1;
    int pipes[num_pipes][2];

    // Création des pipes
    for (int i = 0; i < num_pipes; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("Erreur lors de la création du pipe");
            return 1;
        }
    }

    pid_t pid[2][i];
    int status;

    if((pid[0] = Fork()) == 0) {
        /* Fils */
        if(command->in != NULL){
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
            exit(0);
        }
    }
    else {
        /* Pere */
        if((pid[1] = Fork()) == 0) {
            /* Fils 2 */
            if(command->out != NULL){
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
                exit(0);
            }
        }
        else {
            /* Pere */
        // Fermeture des pipes
        for (int i = 0; i < num_pipes; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);}
            Waitpid(pid[0], &status, 0);
            Waitpid(pid[1], &status, 0);
        }
    }
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
        if ((access(command->out, W_OK))){
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
                exit(0);
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