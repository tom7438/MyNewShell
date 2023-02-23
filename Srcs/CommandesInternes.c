//
// Created by théo on 15/02/23.
//
#include "CommandesInternes.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "csapp.h"

char CommandesInternes[5][10]={"pwd", "cd", "echo", "quit", "exit"};

int isCommandeInterne(char *cmd){
    int i;
    for(i=0; i<5; i++){
        if(!strcmp(cmd, CommandesInternes[i])){
            return 1;
        }
    }
    return 0;
}

int executeCommandeInterne(char *cmd, char **args){
#ifdef DEBUG
    fprintf(stderr, "executeCommandeInterne: %s\n", cmd);
    char **tmp = malloc(sizeof(char *)*MAXBUF);
    tmp = args;
    while(*tmp != NULL){
        fprintf(stderr, "arg: %s\n", *tmp);
        tmp++;
    }
#endif
    if(!strcmp(cmd, "pwd")){
        return pwd();
    }
    else if(!strcmp(cmd, "cd")){
        cd(args[1]);
    }
    else if(!strcmp(cmd, "echo")){
        return echo(args);
    }
    else if(!strcmp(cmd, "quit") || !strcmp(cmd, "exit")){
        return quit();
    } else {
        fprintf(stderr, "Commande interne non reconnue: %s\n", cmd);
    }
    return 0;
}

int pwd(){
#ifdef DEBUG
    fprintf(stderr, "pwd called\n");
#endif
    printf("%s\n", getenv("PWD"));
    return 0;
}

int cd(char *directory){
#ifdef DEBUG
    fprintf(stderr, "cd: %s\n", directory);
#endif
    char cwd[PATH_MAX];
    char oldpwd[PATH_MAX];

    if (getcwd(oldpwd, sizeof(oldpwd)) == NULL) {
        perror("getcwd() error");
        exit(EXIT_FAILURE);
    }

    /* Si aucun argument ou l'argument ~, on se deplace dans le repertoire home */
    if(directory == NULL || directory[0] == '~'){
        char *home = getenv("HOME");
        if(home == NULL){
            perror("getenv() error ");
            exit(EXIT_FAILURE);
        }
        strcpy(cwd, home);
        fprintf(stderr, "cd: %s\n", cwd);
    /* Si l'argument est -, on se deplace dans le repertoire precedent */
    } else if(directory[0] ==  '-') {
        char *last_oldpwd = getenv("OLDPWD");
        if(last_oldpwd == NULL){
            perror("getenv() error ");
            exit(EXIT_FAILURE);
        }
        strcpy(cwd, last_oldpwd);
    /* Sinon on se deplace dans le repertoire specifie */
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

int echo(char **args){
#ifdef DEBUG
    fprintf(stderr, "echo: %s\n", args[1]);
#endif
    int i=1;
    while(args[i] != NULL){
        if(i != 1) {
            printf(" ");
        }
        printf("%s", args[i]);
        i++;
    }
    printf("\n");
    return 1;
}

int quit(){
#ifdef DEBUG
    fprintf(stderr, "quit called\n");
#endif
    exit(0);
}