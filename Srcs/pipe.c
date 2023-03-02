//
// Created by tom on 23/02/23.
//
#include "pipe.h"
#include "csapp.h"
#include "CommandesInternes.h"
#include "readcmd.h"


/* redirectionentre : redirige la entré de la commande
        renvoi 1 si la redirection de entré n'est pas accessible
        renvoi 0 si la redirection de entré a été correctement faite*/
int redirectionentre(struct cmdline * command){
    if ((access(command->in, R_OK))){
        printf("%s: Permission denied entré\n", command->in);
        return 1;
        }
    int fd_in = Open(command->in, O_RDONLY, 0);
    Dup2(fd_in, STDIN_FILENO);
    Close(fd_in);
    return 0;
}

/* redirectionsortie : redirige la sortie de la commande
        renvoi 1 si la redirection de sortie n'est pas accessible
        renvoi 0 si la redirection de sortie a été correctement faite*/
int redirectionsortie(struct cmdline * command){
    if ((access(command->out, F_OK)==0) && (access(command->out, W_OK))){
        printf("%s: Permission denied sortie\n", command->out);
        return 1;
        }
        int fd_out = Open(command->out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        Dup2(fd_out, STDOUT_FILENO);
        Close(fd_out);
        return 0;
}

/* redirectionE_S : utilise les deux commande ci-dessus pour changer si il le faut, les E/S
        renvoi 0 si fait avec succés
        renvoi 1 si un fichier n'est pas accéssible*/
int redirectionE_S(struct cmdline * command){
    if (command->in != NULL){
        if (redirectionentre(command)==1){return 1;}
    }
    if (command->out != NULL){
        if (redirectionsortie(command)==1){return 1;}
    }
    return 0;
}

/* closePipes : Ferme tous les pipes sauf les deux extrémité de tube utilisé par le fils*/
void closePipes(int pipes[][2], int num_pipes, int numeroCommande) {
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
}

/*Commande : Execute une commande en gerant les redirections E/S */
int commande(struct cmdline * command) {

    /*test si commande vide*/
    if(command->seq[0] == NULL) {return 1;}

    /* Redirections */
    if (redirectionE_S(command)==1){return 1;}

    /*command interne*/
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
    return 0;
}


/* Mypipe : Execute 2 commandes avec un pipe */
int Mypipe(struct cmdline * command) {
    int fd[2];
    pipe(fd);
    pid_t pid[2];
    int status;

    if((pid[0] = Fork()) == 0) {
        /* Fils */

        /* redirection entré*/
        if(command->in != NULL){
            if (redirectionentre(command)==1){return 1;}
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

            /*redirection sortie*/
            if(command->out != NULL){
                if (redirectionsortie(command)==1){return 1;}
            }
            Dup2(fd[0], STDIN_FILENO);
            Close(fd[0]);
            Close(fd[1]);

            /*lance les commandes*/
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
    return 0;
}

/*Multipipe : Execute une ligne de commande avec autant de pipe que l'on veux*/
int Multipipe(struct cmdline * command, int nbrcommande) {
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

    pid_t pid[nbrcommande];
    int status;
    for (int i = 0; i<nbrcommande; i++){
        if((pid[i] = Fork()) == 0) {
            /* Fils */
            closePipes(pipes, num_pipes, i);
            if (i==0){ // Si première commande
                if(command->in != NULL){
                    if (redirectionentre(command)==1){return 1;}
                }
            }
            if (i==nbrcommande-1){ // Si dernière commande
                if(command->out != NULL){
                    if (redirectionsortie(command)==1){return 1;}
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
            Waitpid(pid[i], &status, 0);
            if(i==nbrcommande-1) {
                Close(pipes[i-1][0]);
            }
        }
    }
    return 0;
}
