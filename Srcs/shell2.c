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

int main() {
    int couleur = 31;
    /* Ignore les signaux Ctrl-C et Ctrl-Z */
    Signal(SIGINT, sigint_handler);
    Signal(SIGTSTP, SIG_IGN);
	while (1) {
		struct cmdline *command;

        /* Affichage du prompt */
        char *rep=(char *)malloc(sizeof(char)*100);
        printf("\033[%dm", couleur);
        printf("\033[4mMyShell\033[00m");
        printf(":");
        if(!strcmp(getenv("HOME"), getcwd(rep, 100))){
            printf("\033[34m~\033[00m");
        } else{
            printf("\033[34m%s\033[00m",getcwd(rep, 100));
        }
        printf(" # ");
        if((couleur=(couleur+1-31)%18+31)==34){couleur++;};

		command = readcmd();

		/* If input stream closed, normal termination */
		if (!command) {
			printf("exit\n");
			exit(0);
		}

        /* Si Ctrl-C est pressé */
        if(sigint_flag){
            sigint_flag = 0;
            continue;
        }

		if (command->err) {
			/* Syntax error, read another command */
			printf("error: %s\n", command->err);
			continue;
		}

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
            printf("error: too many commands, not yet implemented\n");
        }
    }
}
