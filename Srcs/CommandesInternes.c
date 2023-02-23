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
    char *s=(char *)malloc(sizeof(char)*MAX_CWD);
    printf("%s\n", getcwd(s, MAX_CWD));
    return 0;
}

int cd(char *directory){
#ifdef DEBUG
    fprintf(stderr, "cd: %s\n", directory);
#endif
    putenv("OLDPWD=$PWD");
    if(directory == NULL || !strcmp(directory, "~")){
        char *home = getenv("HOME");
        if(home == NULL){
            printf("cd: HOME not set\n");
            return 0;
        }
        if(chdir(home) == -1){
            perror("chdir ");
            exit(EXIT_FAILURE);
        }
        return 0;
    }
    if(!strcmp(directory, "-")){
        char *oldpwd = getenv("OLDPWD");
        if(oldpwd == NULL){
            printf("cd: OLDPWD not set\n");
            return 0;
        }
        if(chdir(oldpwd) == -1){
            perror("chdir ");
            exit(EXIT_FAILURE);
        }
        return 0;
    }

    if(chdir(directory) == -1){
        perror("chdir ");
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