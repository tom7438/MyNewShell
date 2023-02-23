//
// Created by tom on 22/02/23.
//

#ifndef SHELL_HANDLER_H
#define SHELL_HANDLER_H

extern volatile int sigint_flag;

void sigint_handler(int sig);

#endif //SHELL_HANDLER_H
