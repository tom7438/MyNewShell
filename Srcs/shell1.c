/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"
#include "CommandesInternes.h"
#include "pipe.h"

int main() {
    int couleur = 31;
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

        if (command->err) {
            /* Syntax error, read another command */
            printf("error: %s\n", command->err);
            continue;
        }
        if ((command->out!=NULL)&&(access(command->out, W_OK)))
        {printf("%s: Permission denied \n", command->out);}

        int res = commande(command);

        if(res == 1){
            continue;
        }

        /* Reinitialiser stdin et stdout */
        Dup2(STDIN_FILENO, STDIN_FILENO);
        Dup2(STDOUT_FILENO, STDOUT_FILENO);
    }
}