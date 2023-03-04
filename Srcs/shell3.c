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

int main() {
    initJobs();
    /* Affectation des traitant de signaux */
    Signal(SIGCHLD, sigchld_handler);
    Signal(SIGINT, sigint_handler);
    Signal(SIGTSTP, sigtstp_handler);

    int couleur = 31;
	while (1) {
		struct cmdline *command;

        /* Affichage du prompt */
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
        if((couleur=(couleur+1-31)%18+31)==34){couleur++;};

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

        int i = 0;
        while(command->seq[i] != NULL){
            i++;
        }

        /* Désactivation de l'affichage des séquences d'échappement (^C et ^Z) */
        struct termios term;
        tcgetattr(STDIN_FILENO, &term);
        term.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &term);

        if(i > 1){
            /* Plusieurs pipes */
            Multipipe(command, i);
        } else{
            /* 1 commande unique */
            commande(command);
        }

        /* Réactivation de l'affichage des séquences d'échappement */
        term.c_lflag |= ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &term);

#ifdef DEBUG
        printAllJobs();
#endif
    }
}
