/*
 * signals.cpp
 *
 *  Created on: Apr 18, 2018
 *      Author: os
 */
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/

#include "signals.h"

int smash_kill(pid_t pid, int sig){
	if (pid == 0 || pid == -1) return -1;
	else{
		cout << "signal " << sig_list[sig] << "was sent to pid " << pid << endl;
		if (kill(pid,sig)==-1) return -1;
	}
	return 0;
}



