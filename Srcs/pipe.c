//
// Created by tom on 23/02/23.
//
#include "pipe.h"
#include "csapp.h"
#include "CommandesInternes.h"
#include "readcmd.h"
#include "jobs.h"

int commande(struct cmdline * command) {
    sigset_t mask_all, mask_one, prev_one;
    Sigfillset(&mask_all);
    Sigemptyset(&mask_one);
    Sigaddset(&mask_one, SIGCHLD);
    initJobs(); /* Initialize the job list */
    Sigprocmask(SIG_BLOCK, &mask_one, &prev_one); /* Block SIGCHLD */

    if(command->seq[0] == NULL) {
        return 1;
    }

    /* Mode bd / fg */
    Mode mode;
    if(command->background == 0)
        mode = FOREGROUND;
    else
        mode = BACKGROUND;

    if(isCommandeInterne(command->seq[0][0])){
        executeCommandeInterne(command->seq[0][0], command->seq[0]);
    } else {
        /* Commande externe */
        pid_t pid=0;

        if((pid = Fork()) == 0){ /* Fils */

            /* Redirections */
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
            /* Fin redirections */

            Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
            if(execvp(command->seq[0][0], command->seq[0]) < 0){
                printf("Commande externe non reconnue: %s\n", command->seq[0][0]);
                return 1;
            }
        }
        Sigprocmask(SIG_BLOCK, &mask_all, NULL); /* Parent process */
        addJob(pid, command->seq[0], mode); /* Add the child to the job list */
        printJobs();
        Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
        while(nombreForeground() > 0) {
            Sleep(1);
        }
    }
    printJobs();
    return 0;
}

/* Shell v2 */
int Mypipe(struct cmdline * command) {
    /* 2 commandes avec un pipe */
    int fd[2];
    pipe(fd);

    pid_t pid[2];
    int status;

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
    //resetStdinStdout(oldin, oldout);
    return 0;
}

int Multipipe(struct cmdline * command, int nbrcommande) {
    sigset_t mask_all, mask_one, prev_one;
    Sigfillset(&mask_all);
    Sigemptyset(&mask_one);
    Sigaddset(&mask_one, SIGCHLD);
    initJobs(); /* Initialize the job list */
    Sigprocmask(SIG_BLOCK, &mask_one, &prev_one); /* Block SIGCHLD */

    /* Plusieurs commandes avec plusieurs pipes */
    int num_pipes = nbrcommande-1;
    int pipes[num_pipes][2];

    /* Mode bd / fg */
    Mode mode;
    if(command->background == 0)
        mode = FOREGROUND;
    else
        mode = BACKGROUND;
    fprintf(stderr, "Mode: %s\n", mode == FOREGROUND ? "FOREGROUND" : "BACKGROUND");

    // Création des pipes
    for (int i = 0; i < num_pipes; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("Erreur lors de la création du pipe");
            return 1;
        }
    }

    pid_t pid[nbrcommande];

    for (int i = 0; i<nbrcommande; i++){
        if(isCommandeInterne(command->seq[i][0])) {
            fprintf(stderr, "Pas de pipe pour les commandes internes\n");
            exit(EXIT_FAILURE);
        } else {
            if(i!=nbrcommande-1) {  // si pas la dernière commande
                Dup2(pipes[i][1], STDOUT_FILENO);
                Close(pipes[i][1]);
            }
            if(i!=0) { // si pas la première commande
                Dup2(pipes[i-1][0], STDIN_FILENO);
                Close(pipes[i-1][0]);
            }
            if((pid[i] = Fork()) == 0) { /* Fils */
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
                Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
                if(execvp(command->seq[i][0], command->seq[i]) < 0){
                    printf("Commande externe non reconnue: %s\n", command->seq[0][0]);
                    return 1;
                }
            } else if (pid[i] < 0) {
                perror("Erreur lors du fork");
                return 1;
            }
            fprintf(stderr, "i = %d\n", i);
            /* Père */
            if(i!=0) {
                fprintf(stderr, "Fermeture du pipe [%d][1]\n", i-1);
                if(pipes[i-1][1] != -1)
                    Close(pipes[i-1][1]);
            }
            if(i>1) {
                fprintf(stderr, "Fermeture du pipe [%d][0]\n", i-2);
                if(pipes[i-2][0] != -1)
                    Close(pipes[i-2][0]);
            }
            if(i==nbrcommande-1) {
                fprintf(stderr, "Fermeture du pipe [%d][0]\n", i-1);
                if(pipes[i-1][0] != -1)
                    Close(pipes[i-1][0]);
            }
            Sigprocmask(SIG_BLOCK, &mask_all, NULL); /* Parent process */
            addJob(pid[i], command->seq[i], mode); /* Add the child to the job list */
            Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
        }
    }
    fprintf(stderr, "Fermeture du pipe [%d][1]\n", num_pipes-1);
    while(nombreForeground() > 0) {
        Sleep(1);
    }
    return 0;
}

int closePipes(int pipes[][2], int num_pipes, int numeroCommande) {
    int depart = 0;
    if(numeroCommande>1)
        depart = numeroCommande-1;
    fprintf(stderr, "depart = %d, num_pipes = %d, numeroCommande = %d\n", depart, num_pipes, numeroCommande);
    for (int i = depart; i < num_pipes; i++) {
        if(i!=numeroCommande){
            fprintf(stderr, "Fermeture du pipe [%d][1]\n", i);
            Close(pipes[i][1]);
        }
        if(i!=numeroCommande-1){
            fprintf(stderr, "Fermeture du pipe [%d][0]\n", i);
            Close(pipes[i][0]);
        }
    }
    fprintf(stderr, "Pipes fermés\n");
    return 0;
}