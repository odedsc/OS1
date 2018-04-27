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
extern vector<Job> jobs;
pid_t currFG_PID;
extern list<string> history_list;
extern char prv_directory[MAX_LINE_SIZE];

/*bool Job_Sort(const Job job1, const Job job2)
{
	return job1.ID < job2.ID;
}*/
//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(vector<Job> jobs, char* lineSize, char* cmdString)
{
	char* cmd;
	char* args[MAX_ARG];
	//char pwd[MAX_LINE_SIZE];
	const char* delimiters = " \t\n";
//	char prv_directory[MAX_LINE_SIZE];
	char directory[MAX_LINE_SIZE];
	int i = 0, num_arg = 0;
	bool illegal_cmd = false; // illegal command
    	cmd = strtok(lineSize, (char*)(delimiters));
	if (cmd == NULL)
		return 0;
/*	if (history_list.size() == MAX_HISTORY)
	{
		history_list.pop_back();

	}
	history_list.push_front(cmd);*/
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, (char*)delimiters);
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
		if (num_arg != 1){
			illegal_cmd = true;
		}
		else if (!strcmp(args[1], "-")){
		//	chdir("..");
			cout << prv_directory << endl;
			if (chdir(prv_directory)==-1){
				cout << "smash error: > \"" << args[1] << "\" - path not found" << endl;
			}
		}
		getcwd(directory, sizeof(directory));
		int res=chdir(args[1]);
		if (res==-1 && num_arg==1){
			cout << "smash error: > \"" << args[1] << "\" - path not found" << endl;
		}
		else if (res!=-1 && num_arg==1){
			for (int i=0; i<MAX_LINE_SIZE; i++){
				prv_directory[i]=directory[i];
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "pwd"))
	{
		if (num_arg != 0)
		{
			illegal_cmd = true;
		}
		char directory[MAX_LINE_SIZE];
		if (num_arg==0){
			if (getcwd(directory, sizeof(directory)) != NULL)
			{
				cout << directory << endl;
			}
		}
/*		else
		{
			cout << "System function falied: getcwd" << endl;
		}*/
	}
	/*************************************************/
	else if (!strcmp(cmd, "history"))
	{
		if (num_arg != 0)
		{
			illegal_cmd = true;
		}
		history_list.reverse();
		for (list<string>::iterator it = history_list.begin(); it != history_list.end(); it++)
		{
			cout << *it << endl;

		}
		history_list.reverse();
	}
	/*************************************************/
	else if (!strcmp(cmd, "jobs"))
	{
		if (num_arg!=0) illegal_cmd = true;
		struct timeval present;
		int j=1;
//		sort(sort(job_stack.begin(), job_stack.end(), ID_sort));
		for( vector<Job>::iterator i = jobs.begin(); i != jobs.end(); ++i )
		{
			gettimeofday(&present, NULL);
			time_t full_time = present.tv_sec - i->time.tv_sec;

			cout << '[' << j << "] " << i->name << " : " << i->pid << " " << full_time << " secs";
			if (i->suspended == true)
				cout << " (Stopped)";
			cout << endl;
			j++;
		}

	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid"))
	{
		if (num_arg!=0) illegal_cmd = true;
		else {
			cout << "smash pid is " << getpid() << endl;
		}
		//no failures
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg"))
	{
		if (num_arg>1) illegal_cmd = true;
		else if (jobs.empty()) return -1;
		else if (num_arg==0){
			cout << jobs.back().name << endl;
		}
		else if (num_arg==1){
			illegal_cmd=true;
			int j=0;
			int command_number = atoi(args[1]);
			for( vector<Job>::iterator i = jobs.begin(); i != jobs.end(); i++, j++ )
			{
				if (j==command_number)
				{
					cout << i->name << endl;
					currFG_PID=i->pid;
					if (i->suspended)
					{
						smash_kill(i->pid,SIGCONT);
					}
					smash_wait(i);
					illegal_cmd=false;
					break;
				}
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "bg"))
	{
		if (num_arg>1) illegal_cmd = true;
		else if (jobs.empty()) return 0;
		else if (num_arg==0){
			cout << jobs.back().name << endl;
		}
		else if (num_arg==1){
			illegal_cmd = true;
			int j=1;
			int command_number = atoi(args[1]);
			for( vector<Job>::iterator i = jobs.begin(); i != jobs.end(); i++, j++ )			{
				if (j==command_number)
				{
					cout << i->name << endl;
					if (i->suspended){
						smash_kill(i->pid,SIGCONT);
					}
					illegal_cmd=false;
					break;
				}
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
		if (num_arg != 2)
			illegal_cmd = true;
		else if (*(args[1]) != '-')
			illegal_cmd = true;
		else{
			int sig_num = atoi(args[1] + 1); // +1 to remove the '-'
			if (sig_num <= 0 || sig_num >= 32)	illegal_cmd = true;
			int  job_id=atoi(args[2]);
			bool job_exist = false;
			int j=1;
			if (illegal_cmd == false){
			for( vector<Job>::iterator i = jobs.begin(); i != jobs.end(); ++i )
			{
					if (num_arg == 2 && (j != job_id))
					{
						j++;
						continue;
					}
					job_exist = true;
					cout << sig_num << endl;
					cout << i->pid << endl;
					if (smash_kill(i->pid, sig_num) == -1)
					{
						cout << "smash error: > kill " << j << " - cannot send signal" << endl;
					}
					break;
				}
				if (job_exist == false)
				{
					cout << "smash error: > kill " << job_id << " - job does not exist" << endl;
				}
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		int j=1;
		if (num_arg>1) illegal_cmd = true;
//		signal(SIGCHLD, init_func); // initialize SIGCHLD
		if (num_arg==0) exit(0);
		if (num_arg==1 && strcmp(args[1], "kill")) illegal_cmd = true;
		else if (num_arg==1 && !strcmp(args[1], "kill")){
			while (!jobs.empty())
			{
				smash_kill(jobs.begin()->pid,SIGTERM);
				cout << "[" << j << "]" << jobs.begin()->name << " - Sending SIGTERM...";
				sleep(5);
				int last_ID = waitpid(jobs.begin()->pid, NULL, WNOHANG);
				if (last_ID == 0){
					smash_kill(jobs.begin()->pid, SIGKILL);
					cout << " (5 sec passed) Sending SIGKILL... Done" << endl;
				}
				else {
					cout << " Done." << endl;
				}
				jobs.erase(jobs.begin());
				j++;
			}
			exit(0);
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "mv"))
	{
		if (num_arg!=2) illegal_cmd = true;
		if (rename(args[1], args[2])==-1){
			perror("error in mv command");
			return -1;
		}
		cout << args[1] << " has been renamed to " << args[2] << endl;
	}
	/*************************************************/
	else // external command
	{
		if (BgCmd(lineSize,jobs) == -1){
			ExeExternal(args, const_cast<char*>(args[0]), true);
		}
	 	return 0;
	}
	if (illegal_cmd == true)
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
void ExeExternal(char *args[MAX_ARG], const char* cmdString, bool isFG)
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
                	if (execvp(cmdString, args)==-1){
                		perror("execution failed");
                	}
            		exit(0);
                	//exit(1);

			default:
					jobs.push_back(Job(cmdString,pID,false));
					if (isFG){
						currFG_PID=(jobs.end()-1)->pid;
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

	char *args[MAX_ARG]={"csh", "-f", "-c", lineSize, NULL};

    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
    	if (lineSize[strlen(lineSize)-2] == '&'){
    		lineSize[strlen(lineSize)-2] = '\0';
        	ExeExternal(args, const_cast<char*>(args[3]), false);
        	return 0;
    	}

    	ExeExternal(args, const_cast<char*>(args[3]), true);
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
	const char* delimiters = " \t\n";
	char *args[MAX_ARG];
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
    	cmd = strtok(lineSize, (char*)delimiters);
    	if (cmd == NULL)
    		return 0;
    	args[0] = cmd;
    	for (int i=1; i<MAX_ARG; i++)
    	{
    		args[i] = strtok(NULL, (char*)delimiters);
    	}
    	ExeExternal(args,  const_cast<char*>(args[0]), false);
		return 0;
	}
	return -1;
}


