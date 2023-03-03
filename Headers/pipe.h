//
// Created by tom on 23/02/23.
//

#ifndef MYNEWSHELL_PIPE_H
#define MYNEWSHELL_PIPE_H

#include "readcmd.h"

int Mypipe(struct cmdline * command);

int Multipipe(struct cmdline * command, int nbrcommande);

int commande(struct cmdline * command);

int closePipes(int pipes[][2], int num_pipes, int numeroCommande);

#endif //MYNEWSHELL_PIPE_H
