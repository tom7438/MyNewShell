//
// Created by tom on 01/03/23.
//

#ifndef MYNEWSHELL_JOBS_H
#define MYNEWSHELL_JOBS_H

/**
 * @file jobs.h
 * @brief Fonctions de gestion des jobs
 */


#include "csapp.h"

/**
 * \def MAXJOBS 20
 * @brief Nombre maximum de jobs
 * @details Nombre maximum de jobs que peut gérer le shell
 */
#define MAXJOBS 20

/**
 * \enum Status
 * @brief État d'un job
 * @var EN_COURS En cours d'exécution
 * @var TERMINE Terminé
 * @var SUSPENDU Suspendu
 */
typedef enum {
    EN_COURS,
    TERMINE,
    SUSPENDU,
} Status;

/**
 * \enum Mode
 * @brief Mode d'exécution d'un job
 * @var FOREGROUND Exécution en premier plan
 * @var BACKGROUND Exécution en arrière plan
 * @var LIBRE Mode réservé pour une future utilisation (job non utilisé)
 */
typedef enum {
    FOREGROUND,
    BACKGROUND,
    LIBRE
} Mode;

/**
 * \struct Job
 * @brief Structure d'un job
 * @details Structure d'un job contenant le pid, le numéro, le status, la commande et le mode
 * @var pid_t pid Identifiant du processus
 * @var int numero Numéro du job
 * @var Status status État du job
 * @var char commande[MAXLINE] Commande du job
 * @var Mode mode Mode d'exécution du job (foreground ou background)
 */
typedef struct {
    pid_t pid;
    int numero;
    Status status;
    char commande[MAXLINE];
    Mode mode;
} Job;

/**
 * @brief Initialise la liste des jobs
 * @details Initialise la liste des jobs en mettant tous les jobs à l'état TERMINE
 * en mettant le pid et le numero à 0, la commande à une chaîne vide et le mode à LIBRE
 * @return 0 si tout s'est bien passé, -1 sinon
 */
int initJobs();

/**
 * @brief Fusionne les arguments d'une commande dans une chaîne de caractères
 * @param cmd Chaîne de caractères dans laquelle on va fusionner les arguments
 * @param seq Tableau de chaînes de caractères contenant les arguments de la commande
 * @return 0 si tout s'est bien passé
 */
int joinCmd(char *cmd, char **seq);

/**
 * @brief Ajoute un job à la liste des jobs
 * @details Ajoute un job à la liste des jobs en mettant à jour le pid, le numero, le status,
 * la commande et le mode
 * @param pid Identifiant du processus
 * @param seq Tableau de chaînes de caractères contenant les arguments de la commande
 * @param mode Mode d'exécution du job (foreground ou background)
 * @return 0 si tout s'est bien passé, -1 sinon
 */
int addJob(pid_t pid, char **seq, Mode mode);

/**
 * @brief Trouve le numéro d'un job à partir de son pid
 * @param pid Identifiant du processus
 * @return Le numéro du job s'il existe, -1 sinon
 */
int numeroJob(pid_t pid);

/**
 * @brief Trouve un job à partir de son pid
 * @param pid Identifiant du processus
 * @return Le job s'il existe, NULL sinon
 */
Job *getJobPid(pid_t pid);

/**
 * @brief Modifie des informations d'un job
 * @details Modifie le status et le mode d'un job à partir de son pid
 * @param pid Identifiant du processus
 * @return 0 si tout s'est bien passé, -1 sinon
 */
int updateJobPid(pid_t pid, Status status, Mode mode);

/**
 * @brief Affiche tous les jobs
 * @return 0
 */
int printAllJobs();

/**
 * @brief Supprime un job de la liste des jobs
 * @param pid
 * @return 0 si tout s'est bien passé, -1 sinon
 */
int deletejob(pid_t pid);

/**
 * @brief Calcule le nombre de jobs en premier plan
 * @return Le nombre de jobs en premier plan
 */
int nombreForeground();

/**
 * @brief Tue tous les jobs en premier plan
 * @return 0
 */
int killJobsForeground();

/**
 * @brief Stoppe tous les jobs en premier plan
 * @return 0
 */
int stopJobsForeground();

/**
 * @brief Met un job en premier plan
 * @param num Numéro du job
 * @return 0 si tout s'est bien passé, -1 sinon
 */
int fg(char *num);

/**
 * @brief Met un job en arrière plan
 * @param num Numéro du job
 * @return 0 si tout s'est bien passé, -1 sinon
 */
int bg(char *num);

/**
 * @brief Stoppe un job en arrière plan
 * @param num Numéro du job
 * @return 0 si tout s'est bien passé, -1 sinon
 */
int stop(char *num);

/**
 * @brief Tue tous les jobs
 * @return 0
 */
int killAllJobs();

#endif //MYNEWSHELL_JOBS_H