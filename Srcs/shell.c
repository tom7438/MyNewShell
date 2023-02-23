/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"
#include "CommandesInternes.h"
#include "handler.h"

//volatile int sigint_flag = 0;

int main() {
    int couleur = 31;
    /* Ignore les signaux Ctrl-C et Ctrl-Z */
    Signal(SIGINT, sigint_handler);
    Signal(SIGTSTP, SIG_IGN);
	while (1) {
		struct cmdline *l;

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

		l = readcmd();

		/* If input stream closed, normal termination */
		if (!l) {
			printf("exit\n");
			exit(0);
		}

        /* Si Ctrl-C est pressé */
        if(sigint_flag){
            sigint_flag = 0;
            continue;
        }

		if (l->err) {
			/* Syntax error, read another command */
			printf("error: %s\n", l->err);
			continue;
		}

        /* Commande interne */
        if(isCommandeInterne(l->seq[0][0])){
            executeCommandeInterne(l->seq[0][0], l->seq[0]);
        } else {
            /* Commande externe */
            pid_t pid;
            int status;

            if((pid = Fork()) == 0) {
                /* Fils */
                if(l->in != NULL){
                    int fd_in = Open(l->in, O_RDONLY, 0);
                    Dup2(fd_in, STDIN_FILENO);
                    Close(fd_in);
                }
                if(l->out != NULL){
                    int fd_out = Open(l->out, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                    Dup2(fd_out, STDOUT_FILENO);
                    Close(fd_out);
                }
                if(execvp(l->seq[0][0], l->seq[0]) < 0){
                    printf("Commande externe non reconnue: %s\n", l->seq[0][0]);
                    exit(0);
                }
            }
            else {
                /* Pere */
                Waitpid(pid, &status, 0);
            }
        }
	}
}
