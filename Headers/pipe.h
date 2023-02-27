//
// Created by tom on 23/02/23.
//

#ifndef MYNEWSHELL_PIPE_H
#define MYNEWSHELL_PIPE_H

#include "readcmd.h"

int resetStdinStdout(int oldin, int oldout);

int Mypipe(struct cmdline * command);

int Multipipe(struct cmdline * command, int nbrcommande);

int commande(struct cmdline * command);

#endif //MYNEWSHELL_PIPE_H
