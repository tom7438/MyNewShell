//
// Created by théo on 15/02/23.
//
#include "CommandesInternes.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "csapp.h"
#include <limits.h>
#include "jobs.h"

/* Tableau de commandes internes */
char CommandesInternes[8][10]={"pwd", "cd", "quit", "exit", "fg", "bg", "jobs", "stop"};

int isCommandeInterne(char *cmd){
#ifdef DEBUG
    fprintf(stderr, "isCommandeInterne(%s) called\n", cmd);
#endif
    int i;
    /* On parcourt le tableau de commandes internes */
    for(i=0; i<8; i++){
        /* Si la commande est interne, on retourne 1 */
        if(!strcmp(cmd, CommandesInternes[i])){
            return 1;
        }
    }
    return 0;
}

int executeCommandeInterne(char *cmd, char **args){
#ifdef DEBUG
    fprintf(stderr, "executeCommandeInterne(%s) called\n", cmd);
    char **tmp = malloc(sizeof(char *)*MAXBUF);
    tmp = args;
    while(*tmp != NULL){
        fprintf(stderr, "arg: %s\n", *tmp);
        tmp++;
    }
#endif
    /* On compare la commande avec les commandes internes */
    /* On exécute la commande correspondante */
    if(!strcmp(cmd, "pwd")){
        return pwd();
    }
    else if(!strcmp(cmd, "cd")){
        cd(args[1]);
    }
    else if(!strcmp(cmd, "quit") || !strcmp(cmd, "exit")){
        return quit();
    } else if (!strcmp(cmd, "fg")) {
        if(args[1] == NULL) {
            fprintf(stderr, "fg: argument manquant (n°job)\n");
            return -1;
        }
        return fg(args[1]);
    } else if (!strcmp(cmd, "bg")) {
        if(args[1] == NULL) {
            fprintf(stderr, "bg: argument manquant (n°job)\n");
            return -1;
        }
        return bg(args[1]);
    } else if (!strcmp(cmd, "jobs")) {
        return printAllJobs();
    } else if (!strcmp(cmd, "stop")) {
        if(args[1] == NULL) {
            fprintf(stderr, "stop: argument manquant (n°job)\n");
            return -1;
        }
        return stop(args[1]);
    } else {
        fprintf(stderr, "Commande interne non reconnue: %s\n", cmd);
        return -1;
    }
    return 0;
}

int pwd(){
#ifdef DEBUG
    fprintf(stderr, "pwd() called\n");
#endif
    printf("%s\n", getenv("PWD"));
    return 0;
}

int cd(char *directory){
#ifdef DEBUG
    fprintf(stderr, "cd(%s) called\n", directory);
#endif
    char cwd[PATH_MAX];
    char oldpwd[PATH_MAX];

    /* On recupere le repertoire courant */
    /* Il devient le repertoire precedent */
    if (getcwd(oldpwd, sizeof(oldpwd)) == NULL) {
        perror("getcwd() error");
        exit(EXIT_FAILURE);
    }

    /* Si aucun argument ou l'argument ~, on se deplace dans le repertoire HOME */
    if(directory == NULL || directory[0] == '~'){
        char *home = getenv("HOME");
        if(home == NULL){
            perror("getenv() error ");
            exit(EXIT_FAILURE);
        }
        strcpy(cwd, home);

    /* Si l'argument est -, on se deplace dans le repertoire precedent OLDPWD*/
    } else if(directory[0] ==  '-') {
        char *last_oldpwd = getenv("OLDPWD");
        if(last_oldpwd == NULL){
            perror("getenv() error ");
            exit(EXIT_FAILURE);
        }
        strcpy(cwd, last_oldpwd);

    /* Sinon on utilise le repertoire specifie */
    } else {
        strcpy(cwd, directory);
    }

    /* On se deplace dans le repertoire */
    if(chdir(cwd) == -1){
        perror("chdir() error ");
        exit(EXIT_FAILURE);
    }

    /* On met a jour les variables d'environnement */
    if(setenv("OLDPWD", oldpwd, 1) == -1){
        perror("setenv(OLDPWD) error ");
        exit(EXIT_FAILURE);
    }

    if(setenv("PWD", getcwd(cwd, sizeof(cwd)), 1) == -1){
        perror("setenv(PWD) error ");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int quit(){
#ifdef DEBUG
    fprintf(stderr, "quit called\n");
#endif
    /* On tue tous les processus restants */
    killAllJobs();
    exit(0);
}