/*
 * signals.cpp
 */
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/

#include "signals.h"

extern std::vector<job> jobs;

int smash_kill(pid_t pid, int sig){
	if (pid == 0 || pid == -1) return -1;
	else{
		cout << "signal " << sig_list[sig] << "was sent to pid " << pid << endl;
		if (kill(pid,sig)==-1) return -1;
	}
	return 0;
}

void smash_wait(vector<Job>::iterator pJob){
//	pid_t child_pid = pJob->pid;
	int* statusPtr;
	pid_t last_pid = waitpid(child_pid, statusPtr, WUNTRACED);
	if (last_pid==pJob->pid){
		for (vector<Job>::iterator i = job_stack.begin(); i != job_stack.end(); ++i)
		{
			if (i->pid==last_pid){
				pJob=i;
				break;
			}
		}
		if (!WIFSTOPPED(*StatusPtr)){
			jobs.erase(pJob);
		}
	}
}


