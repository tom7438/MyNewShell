//
// Created by tom on 23/02/23.
//

#ifndef MYNEWSHELL_PIPE_H
#define MYNEWSHELL_PIPE_H

/**
 * @file pipe.h
 * @brief Fonctions de gestion des commandes simples et des pipes
 */

#include "readcmd.h"

/**
 * @brief Fonction qui gère les commandes simples
 * @param command La commande à exécuter
 * @return 0 si la commande s'est bien exécutée, 1 sinon
 */
int commande(struct cmdline * command);

/**
 * @brief Fonction qui gère 2 commandes séparées par un pipe
 * @param command La commande à exécuter
 * @return 0 si la commande s'est bien exécutée, 1 sinon
 */
int Mypipe(struct cmdline * command);

/**
 * @brief Fonction qui gère n commandes séparées par n-1 pipes
 * @param command La commande à exécuter
 * @param nbrcommande Le nombre de commandes
 * @return 0 si la commande s'est bien exécutée, 1 sinon
 */
int Multipipe(struct cmdline * command, int nbrcommande);

/**
 * @brief Fonction qui ferme les pipes inutiles
 * @details Ferme les pipes inutiles pour la commande numéroCommande
 * @param pipes Le tableau des pipes
 * @param num_pipes Le nombre de pipes
 * @param numeroCommande Le numéro de la commande
 * @return 0 si la commande s'est bien exécutée
 */
int closePipes(int pipes[][2], int num_pipes, int numeroCommande);

#endif //MYNEWSHELL_PIPE_H
