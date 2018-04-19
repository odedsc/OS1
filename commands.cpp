/*
 * commands.cpp
 *
 *  Created on: Apr 19, 2018
 *      Author: os
 */

//		
//********************************************
#include "commands.h"
#include "signals.h"

using namespace std;

bool Job_Sort(const Job job1, const Job job2)
{
	return job1.ID < job2.ID;
}//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(vector<Job> jobs, char* lineSize, char* cmdString)
{
	char* cmd;
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	char* delimiters = " \t\n";
	int i = 0, num_arg = 0;
	bool illegal_cmd = false; // illegal command
	list<char*> history_list;
    	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0;
	if (history_list.size() == MAX_HISTORY)
	{
		history_list.pop_back();

	}
	history_list.push_front(cmd);
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters);
		if (args[i] != NULL)
			num_arg++;

	}
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd") )
	{
		if (num_arg != 1) return -1;
		else if (!strcmp(arg[1], "-"){
			chdir("..");
			cout << getcwd << endl;
		}
		else if (chdir(args[1])==-1){
			cout << "smash error: > " << args[1] << " - path not found" << endl;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "pwd"))
	{
		if (num_arg != 0)
		{
			return -1;
		}
		char directory[MAX_LINE_SIZE];
		if (getcwd(directory, sizeof(directory)) != NULL)
		{
			cout << directory << endl;
		}
		else
		{
			cout << "System function falied: getcwd" << endl;
		}
		return 0;
	}
	/*************************************************/
	else if (!strcmp(cmd, "history"))
	{
		history_list.reverse();
		list<char*>::iterator it;
		for (it = history_list.begin(); it != history_list.end(); it++)
		{
			cout << it << endl;
		}
		history_list.reverse();
		return 0;
	}
	else if (!strcmp(cmd, "jobs"))
	{
		if (num_arg!=0) return -1;
		struct timeval present;
		sort(sort(job_stack.begin(), job_stack.end(), ID_sort));
		for( vector<Job>::iterator i = jobs.begin(); i != jobs.end(); ++i )
		{
			gettimeofday(&present, NULL);
			time_t full_time = present.tv_sec - i->time.tv_sec;

			cout << '[' << i->ID << "] " << i->job_name << " : " << i->pid << " " << full_time << " secs" << endl;
/*			if (i->suspended == true)
				cout << " (Stopped)";
			cout << endl;*/
		}

	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid"))
	{
		cout << "smash pid is " << getpid() << endl;
		//no failures
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg"))
	{
		if ((num_arg!=0) || (num_arg!=1)) return -1;
		else if (jobs.empty()) break;
		else if (num_arg==0){
			cout << jobs.back()->name << endl;
		}
		else if (num_arg==1){
			for( vector<Job>::iterator i = jobs.begin(); i != jobs.end(); ++i )
			{
				if (i->ID==arg[1]){
					cout << i->name << endl;
					if (i->suspended){
						smash_kill(i->pid,SIGCONT));
					}
					smash_wait(i);
					break;
				}
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "bg"))
	{
		if ((num_arg!=0) || (num_arg!=1)) return -1;
		else if (jobs.empty()) break;
		else if (num_arg==0){
			cout << jobs.back()->name << endl;
		}
		else if (num_arg==1){
			for( vector<Job>::iterator i = jobs.begin(); i != jobs.end(); ++i )
			{
				if (i->ID==arg[1]){
					cout << i->name << endl;
					if (i->suspended){
						smash_kill(i->pid,SIGCONT));
					}
					break;
				}
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
			if (num_arg != 2)
				return -1;
			if (*(args[1]) != '-')
				return -1;

			int sig_num = atoi(args[1] + 1); // +1 to remove the '-'
			if (sig_num <= 0 || sig_num >= 32)
			{
				return -1;
			}
			int  job_id;
			if (num_arg == 2)
			{
				job_id = atoi(args[1])
			}
			else
			{
				job_id = -1;
			}
			bool job_exist = false;

			for (i = jobs.begin(); i != jobs.end(); i++)
			{
				if (num_arg == 2 && (i->ID != job_id))
				{
					continue;
				}
				job_exist = true;
				if (smash_kill(i->pID, sig_num) == -1)
				{
					cout << "smash error: > kill " << i->ID << " - cannot send signal" << endl;
				}
			}
			if (job_exist == false && num_arg == 2)
			{
				cout << "smash error: > kill " << job_id << " - job does not exist" << endl;
			}
			return 0;
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		if ((num_arg!=0) || (num_arg!=1)) return -1;
		if (num_arg==0) exit(0);
		else if (num_arg==1){
			while (!job_stack.empty())
			{
				smash_kill(jobs.begin()->pid,SIGTERM);
				cout << "[" << jobs.begin()->ID << "]" << jobs.begin()->name << " - Sending SIGTERM...";
				sleep(5);
				int last_ID = waitpid(jobs.begin()->pid, NULL, WNOHANG);
				if (last_ID == 0){
					Smash_Kill(child_pID, SIGKILL);
					cout << " (5 sec passed) Sending SIGKILL... Done" << endl;
				}
				else {
					cout << " Done." << endl;
				}
				jobs.erase(jobs.begin());
			}

		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "mv"))
	{
		if (num_arg!=2) return -1;
		if (rename(args[1], args[2])==-1){
			perror("error in mv command");
			return -1;
		}
		cout << args[1] << "has been renamed to" << args[2] << endl;
	}
	/*************************************************/
	else // external command
	{
		if (ExeComp(lineSize) == -1){
			BgCmd(lineSize,jobs);
		}
 		ExeExternal(args, cmdString);
	 	return 0;
	}
	if (illegal_cmd == TRUE)
	{
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString)
{
	int pID;
    	switch(pID = fork())
	{
    		case -1:
					perror("error in fork function");
					return;
        	case 0 :
                	// Child Process
               		setpgrp();

                	if (execvp(const_cast<char*>(*args), const_cast<char**>(args))==-1){
                		perror("execution failed");
                		exit(1);
                	}

			default:
					jobs.push_back(Job(args[0],pID,false));
					if (cmdString=="fg"){
						smash_wait(jobs.end()-1);
					}


	}
}
//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize)
{
//	char ExtCmd[MAX_LINE_SIZE+2];
	char *args[MAX_ARG]={"/bin/csh", "-f", "-c", lineSize, NULL};
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
    	if (lineSize[strlen(lineSize)-2] == '&'){
    		lineSize[strlen(lineSize)-2] = '\0';
        	ExeExternal(const_cast<char**>(args), "bg");
    	}

    	ExeExternal(const_cast<char**>(args), "fg");

    	return 0;
	}
	return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, vector<Job> jobs)
{

	char* cmd;
	char* delimiters = " \t\n";
	char *args[MAX_ARG];
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
    	cmd = strtok(lineSize, delimiters);
    	if (cmd == NULL)
    		return 0;
    	args[0] = cmd;
    	for (int i=1; i<MAX_ARG; i++)
    	{
    		args[i] = strtok(NULL, delimiters);
    	}
    	ExeExternal(args,"bg");
	}
	return -1;
}


