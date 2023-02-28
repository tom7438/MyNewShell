//
// Created by tom on 23/02/23.
//
#include "pipe.h"
#include "csapp.h"
#include "CommandesInternes.h"
#include "readcmd.h"

int commande(struct cmdline * command) {
    sigset_t mask_all, mask_one, prev_one;
    Sigfillset(&mask_all);
    Sigemptyset(&mask_one);
    Sigaddset(&mask_one, SIGCHLD);
    //initjobs(); /* Initialize the job list */
    Sigprocmask(SIG_BLOCK, &mask_one, &prev_one); /* Block SIGCHLD */

    if(command->seq[0] == NULL) {
        return 1;
    }

    /* Permet de rétablir stdin et stdout */
    int oldin = -1;
    int oldout = -1;

    /* Redirections */
    if(command->in != NULL){
        if ((access(command->in, R_OK))){
            printf("%s: Permission denied entré\n", command->in);
            return 1;
        }
        oldin = dup(STDIN_FILENO);
        int fdin = Open(command->in, O_RDONLY, 0);
        Dup2(fdin, STDIN_FILENO);
        Close(fdin);
    }
    if(command->out != NULL){
        if ((access(command->out, F_OK)==0) && (access(command->out, W_OK))){
            printf("%s: Permission denied sortie\n", command->out);
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
            Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
            if(execvp(command->seq[0][0], command->seq[0]) < 0){
                printf("Commande externe non reconnue: %s\n", command->seq[0][0]);
                return 1;
            }
        }
        if(command->background == 0)
            Waitpid(pid, &status, 0);
    }
    Sigprocmask(SIG_BLOCK, &mask_all, NULL); /* Parent process */
    //addjob(pid[i]); /* Add the child to the job list */
    Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
    resetStdinStdout(oldin, oldout);
    return 0;
}

/* Shell v2 */
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
    sigset_t mask_all, mask_one, prev_one;
    Sigfillset(&mask_all);
    Sigemptyset(&mask_one);
    Sigaddset(&mask_one, SIGCHLD);
    //initjobs(); /* Initialize the job list */
    Sigprocmask(SIG_BLOCK, &mask_one, &prev_one); /* Block SIGCHLD */

    /* Plusieurs commandes avec plusieurs pipes */
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
            closePipes(pipes, num_pipes, i);
            if (i==0){ // Si première commande
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
            }
            if (i==nbrcommande-1){ // Si dernière commande
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
        } else if (pid[i] < 0) {
            perror("Erreur lors du fork");
            return 1;
        } else {
            /* Père */
            if(i!=0) {
                Close(pipes[i-1][1]);
            }
            if(i>1) {
                Close(pipes[i-2][0]);
            }
            if(command->background==0)
                Waitpid(pid[i], &status, 0);
            if(i==nbrcommande-1) {
                Close(pipes[i-1][0]);
            }
        }
    }
    Sigprocmask(SIG_BLOCK, &mask_all, NULL); /* Parent process */
    //addjob(pid[i]); /* Add the child to the job list */
    Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
    resetStdinStdout(oldin, oldout);
    return 0;
}

int resetStdinStdout(int oldin, int oldout) {
    if(oldin!=-1)
        Dup2(oldin, STDIN_FILENO);
    if(oldout!=-1)
        Dup2(oldout, STDOUT_FILENO);
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