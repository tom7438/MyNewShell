//
// Created by tom on 22/02/23.
//
#ifndef SHELL_HANDLER_H
#define SHELL_HANDLER_H

/**
 * @file handler.h
 * @brief Fonctions de gestion des signaux
 */

/**
 * @brief Gestion du signal SIGINT
 * @param sig Signal SIGINT
 */
void sigint_handler(int sig);

/**
 * @brief Gestion du signal SIGCHLD
 * @param sig Signal SIGCHLD
 */
void sigchld_handler(int sig);

/**
 * @brief Gestion du signal SIGTSTP
 * @param sig Signal SIGTSTP
 */
void sigtstp_handler(int sig);

#endif //SHELL_HANDLER_H
