/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"
#include "CommandesInternes.h"
#include "handler.h"
#include "pipe.h"
#include "jobs.h"

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

        /* Compte le nombre de commandes séparées par un pipe */
        int i = 0;
        while(command->seq[i] != NULL){
            i++;
        }

        if(i == 2){
            /* 2 commandes avec un pipe */
            Mypipe(command);
        } else if (i == 1){
            /* 1 commande unique */
            commande(command);
        } else {
            printf("error: too many commands for this version\n");
        }
    }
}
