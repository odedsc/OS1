/*
 * signals.h
 */


#ifndef _SIGS_H
#define _SIGS_H
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <iostream>
#include <vector>

using namespace std;

const string sig_list[] =
{
	"",				//0
	"SIGHUP",		//1
	"SIGINT",		//2
	"SIGQUIT",		//3
	"SIGILL",		//4
	"SIGTRAP",		//5
	"SIGABRT",		//6
	"SIGBUS",		//7
	"SIGFPE",		//8
	"SIGKILL",		//9
	"SIGUSR1",		//10
	"SIGSEGV",		//11
	"SIGUSR2",		//12
	"SIGPIPE",		//13
	"SIGALRM",		//14
	"SIGTERM",		//15
	"SIGSTKFLT",	//16
	"SIGCHLD",		//17
	"SIGCONT",		//18
	"SIGSTOP",		//19
	"SIGTSTP",		//20
	"SIGTTIN",		//21
	"SIGTTOU",		//22
	"SIGURG",		//23
	"SIGXCPU",		//24
	"SIGXFSZ",		//25
	"SIGVTALRM",	//26
	"SIGPROF",		//27
	"SIGWINCH",		//28
	"SIGIO",		//29
	"SIGPWR",		//30
	"SIGSYS",		//31
};

class Job
{
	public:
//		string name;
		pid_t pid;
		int ID;
		bool suspended;
		struct timeval time;
		time_t susp_time;

		Job(string name, int pid, bool susp):
			job_name(name),
			pID(pid),
			ID( Find_Min_Id()),
			suspended(susp)
		{
			gettimeofday(&time, NULL);
		}
};

int smash_kill(pid_t pid, int sig);
void smash_wait(vector<Job>::iterator pJob);


#endif

#endif /* SIGNALS_H_ */
