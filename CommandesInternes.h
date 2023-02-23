//
// Created by tom on 15/02/23.
//
#ifndef SHELL_COMMANDESINTERNES_H
#define SHELL_COMMANDESINTERNES_H

#define MAX_CWD 100

int isCommandeInterne(char *cmd);

int executeCommandeInterne(char *cmd, char **args);

int pwd();
int cd(char *directory);
int echo(char **args);
int quit();

#endif //SHELL_COMMANDESINTERNES_H