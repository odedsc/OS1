/*
	smash.cpp
 *
 *  Created on: Apr 19, 2018
 *      Author: os
 *
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"
#include "signals.h"
#define MAX_LINE_SIZE 80
#define MAXARGS 20


using namespace std;

char* L_Fg_Cmd;
vector<Job> jobs;
char lineSize[MAX_LINE_SIZE];
extern pid_t currFG_PID;
list<string> history_list;
char prv_directory[MAX_LINE_SIZE];

//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main (int argc, char *argv[])
{
    char cmdString[MAX_LINE_SIZE];

	//signal declaretions
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	 /* add your code here */

	/************************************/
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	//set your signal handlers here
	/* add your code here */
    signal(SIGTSTP, aux_smash_kill);
    signal(SIGINT,  aux_smash_kill);
    signal(SIGCHLD, smash_sigchld);
	/************************************/

	/************************************/
	// Init globals

    currFG_PID = -1;

/*	L_Fg_Cmd =(char*)malloc(sizeof(char)*(MAX_LINE_SIZE+1));
	if (L_Fg_Cmd == NULL)
			exit (-1);
	L_Fg_Cmd[0] = '\0';*/

    	while (1)
    	{
	 	printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);
		cmdString[strlen(lineSize)-1]='\0';
					// perform a complicated Command
		if(!ExeComp(lineSize)) continue;
					// background command
	 	if(!BgCmd(lineSize, jobs)) continue;
					// built in commands
		ExeCmd(jobs, lineSize, cmdString);
		if (history_list.size() == MAX_HISTORY)
		{
			history_list.pop_back();

		}
		std::string str(lineSize);
		history_list.push_front(lineSize);
		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
    return 0;
}





