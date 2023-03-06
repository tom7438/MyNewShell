//
// Created by tom on 15/02/23.
//
#ifndef SHELL_COMMANDESINTERNES_H
#define SHELL_COMMANDESINTERNES_H

/**
 * @file CommandesInternes.h
 * @brief Définition des fonctions de commandes internes
 * @author groupe 11
 * @date 2021-02-15
 */

/**
 * \def MAX_CWD 100
 */
#define MAX_CWD 100

/**
 * @brief Vérifie si la commande est interne
 * @param cmd nom de la commande
 * @return 1 si la commande est interne, 0 sinon
 */
int isCommandeInterne(char *cmd);

/**
 * @brief Exécute la commande interne
 * @param cmd nom de la commande
 * @param args arguments de la commande
 * @return 0 si la commande s'est bien exécutée, -1 sinon
 */
int executeCommandeInterne(char *cmd, char **args);

/**
 * @brief Affiche le répertoire courant
 * @return 0 si la commande s'est bien exécutée
 */
int pwd();

/**
 * @brief Change le répertoire courant
 * @param directory répertoire dans lequel on veut se déplacer
 * @return 0 si la commande s'est bien exécutée
 */
int cd(char *directory);

/**
 * @brief Quitte le shell en tuant tous les processus restants
 * @return 0 si la commande s'est bien exécutée
 */
int quit();

#endif //SHELL_COMMANDESINTERNES_H