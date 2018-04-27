/*
 * commands.h
 *
 *  Created on: Apr 18, 2018
 *      Author: os
 */

#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include "signals.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <vector>
#include <stdbool.h>
#include <list>
#define MAX_HISTORY 50
#define MAX_LINE_SIZE 80
#define MAX_ARG 20
//typedef enum { FALSE , TRUE } bool;
int ExeComp(char* lineSize);
int BgCmd(char* lineSize, vector<Job> jobs);
int ExeCmd(vector<Job> jobs, char* lineSize, char* cmdString);
void ExeExternal(char *args[MAX_ARG], const char* cmdString, bool isFG);
#endif

