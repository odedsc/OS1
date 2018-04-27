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

extern vector<Job> jobs;
extern pid_t currFG_PID;

int smash_kill(pid_t pid, int sig){
	if (pid == 0 || pid == -1) return -1;
	else{
		cout << "signal " << sig_list[sig] << " was sent to pid " << pid << endl;
		if (kill(pid,sig)==-1) return -1;
	}
	return 0;
}

void smash_wait(vector<Job>::iterator pJob){
//	pid_t child_pid = pJob->pid;
	int* statusPtr;
	pid_t last_pid = waitpid(pJob->pid, statusPtr, WUNTRACED);
	if (last_pid==pJob->pid){
		for (vector<Job>::iterator i = jobs.begin(); i != jobs.end(); ++i)
		{
			if (i->pid==last_pid){
				pJob=i;
				if (!WIFSTOPPED(*statusPtr)){
					jobs.erase(pJob);
				}
				else{
					pJob->suspended=true;
//					time(&pJob->susp_time);
				}
				break;
			}
		}

	}
	currFG_PID=-1;
}

void aux_smash_kill(int sig){
	smash_kill(currFG_PID,sig);
}

void smash_sigchld(int sig){
	for (vector<Job>::iterator i = jobs.begin(); i != jobs.end(); ++i)
	{
		int* statusPtr;
		pid_t last_pid = waitpid(i->pid, statusPtr, WUNTRACED);
		if (last_pid == i->pid && i->pid!=currFG_PID){
			if (!WIFSTOPPED(*statusPtr)){
				jobs.erase(i);
				i--;
			}
			else{
				i->suspended=true;
				//time(&i->susp_time);
			}
		}
	}
}

void init_func(int sig){
}
