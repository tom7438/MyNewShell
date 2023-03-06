//
// Created by théo on 23/02/23.
//


#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"
#include "CommandesInternes.h"
#include "pipe.h"
#include "jobs.h"
#include "handler.h"
#include <termios.h>

void printPrompt(int couleur){
    char *rep=(char *)malloc(sizeof(char)*100);
    printf("\033[%dm", couleur);
    printf("\033[4mMyShell3\033[00m");
    printf(":");
    if(!strcmp(getenv("HOME"), getcwd(rep, 100))){
        printf("\033[34m~\033[00m");
    } else{
        printf("\033[34m%s\033[00m",getcwd(rep, 100));
    }
    printf(" # ");
    free(rep);
}

int main() {
    initJobs();
    /* Affectation des traitant de signaux */
    Signal(SIGCHLD, sigchld_handler);
    Signal(SIGINT, sigint_handler);
    Signal(SIGTSTP, sigtstp_handler);

#ifndef TEST
    int couleur = 31;
#endif
	while (1) {
		struct cmdline *command;

#ifndef TEST
        /* Affichage du prompt */
        printPrompt(couleur);
        if((couleur=(couleur+1-31)%18+31)==34){couleur++;};
#endif

		command = readcmd();

		/* If input stream closed, normal termination */
		if (!command) {
			printf("exit\n");
			exit(0);
		}

		if (command->err) {
			/* Syntax error, read another command */
			printf("error: %s\n", command->err);
			continue;
		}

        /* Compte le nombre de commandes séparées par des pipes */
        int i = 0;
        while(command->seq[i] != NULL){
            i++;
        }

        if(i > 1){
            /* Plusieurs pipes */
            Multipipe(command, i);
        } else{
            /* 1 commande unique */
            commande(command);
        }

#ifdef DEBUG
        printAllJobs();
#endif
    }
}
