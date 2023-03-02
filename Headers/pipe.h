//
// Created by tom on 23/02/23.
//

#ifndef MYNEWSHELL_PIPE_H
#define MYNEWSHELL_PIPE_H

#include "readcmd.h"

int redirectionentre(struct cmdline * command);

int redirectionsortie(struct cmdline * command);

int redirectionE_S(struct cmdline * command);

void closePipes(int pipes[][2], int num_pipes, int numeroCommande);

int commande(struct cmdline * command);

int Mypipe(struct cmdline * command);

int Multipipe(struct cmdline * command, int nbrcommande);



#endif //MYNEWSHELL_PIPE_H
