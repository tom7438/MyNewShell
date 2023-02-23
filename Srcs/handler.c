//
// Created by tom on 22/02/23.
//
#include "handler.h"
#include "csapp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

volatile int sigint_flag = 0;

void sigint_handler(int sig) {
    printf("Ctrl-C pressed\n");
    sigint_flag = 1;
}