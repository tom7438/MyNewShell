//
// Created by tom on 23/02/23.
//

#ifndef MYNEWSHELL_PIPE_H
#define MYNEWSHELL_PIPE_H

#include "readcmd.h"

int Mypipe(struct cmdline * command);

int commande(struct cmdline * command);

int resetStdinStdout(int oldin, int oldout);

#endif //MYNEWSHELL_PIPE_H
