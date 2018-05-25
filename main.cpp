/*
 * main.cpp
 *
 *  Created on: May 20, 2018
 *      Author: os
 */

#include <pthread.h>
#include <unistd.h>

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <iostream>

#include "bank.h"
#include "ATM.h"

#define MAX_LINE_SIZE 80

Bank bank;
bool finish;

//ATM_params
//module's role: gives two pointers to any ATM thread - the ATM's thread and the file's name which it takes care of all its actions
class ATM_params{
public:
	int id;
	char* file_name;
};

//********************************************
// function name: ATM_EVENT
// Description: a function for the routine of an ATM thread, in which the ATM makes all of its actions
// Parameters: ATM_threads_params - a class which includes the id of the ATM and its file name
// Returns: None
//**************************************************************************************

void* ATM_EVENT(void* ATM_threads_params){
	ATM_params& params = *(ATM_params*)ATM_threads_params;
	ATM my_ATM(params.id);
	FILE* file=fopen(params.file_name, "r");
	char line[MAX_LINE_SIZE];
	while (fgets(line, MAX_LINE_SIZE, file)){
		char* pNewVal;

		int val_1 = strtol(&line[2], &pNewVal, 10);
		int val_2 = strtol(pNewVal,     &pNewVal, 10);
		int val_3 = strtol(pNewVal,     &pNewVal, 10);
		int val_4 = strtol(pNewVal,     &pNewVal, 10);

		my_ATM.lock();

		switch (line[0])
		{
		case 'O': my_ATM.open_account(val_1,val_2,val_3);   break;
		case 'L': my_ATM.turn_VIP(val_1,val_2);        	   break;
		case 'D': my_ATM.deposit(val_1,val_2,val_3);        break;
		case 'W': my_ATM.withdraw(val_1,val_2,val_3);       break;
		case 'B': my_ATM.balance_inquiry(val_1,val_2);       break;
		case 'T': my_ATM.transfer(val_1,val_2,val_3,val_4);break;
		default:  break;
		}

		my_ATM.unlock();

		usleep(100000);
	}
	pthread_exit(NULL);
}

//********************************************
// function name: Commision_Event
// Description: a function for the routine of a commission thread
// Parameters: None (an unused void pointer because of the parameters of pthread_create function
// Returns: None
//**************************************************************************************

void* Commision_Event(void*)
{
	while (finish == false)
	{
		sleep(3);
		bank.commission();
	}

	pthread_exit(NULL);
}

//********************************************
// function name: Print_Event
// Description: a function for the routine of a log file printing thread
// Parameters: None (an unused void pointer because of the parameters of pthread_create function
// Returns: None
//**************************************************************************************

void* Print_Event(void*)
{
	while (finish == false)
	{
		bank.print();
		usleep(500000);
	}
	pthread_exit(NULL);
}

//********************************************
// function name: main
// Description: main function which gets the ATM files and makes all the relevant actions given by them, prints it to the log file and makes commission for them
// Parameters: argc - number of arguments, argv[] - a char array for the given parameters
// Returns: None
//**************************************************************************************

int main (int argc, char *argv[])
{
	if ((argc < 3) || (atoi(argv[1]) != (argc-2)))
	{
		cout << "illegal arguments" << endl;
		return -1;
	}

	int ATM_num=atoi(argv[1]);


	/*initialize relevant threads*/
	pthread_t ATM_threads[ATM_num];
	pthread_t commission_thread;
	pthread_t print_thread;
	ATM_params* ATM_threads_params;

	finish=false;

	ATM_threads_params=new ATM_params[ATM_num];
	for (int i=0; i<ATM_num; i++){
		ATM_threads_params[i].id=i+1;
		ATM_threads_params[i].file_name=argv[i+2];
		//creates thread for each ATM
		if (pthread_create(&ATM_threads[i], NULL, ATM_EVENT, (void*)&(ATM_threads_params[i]))){
			perror("thread creation failed\n");
		}
	}
	//creates thread for commission
	if (pthread_create(&commission_thread, NULL, Commision_Event, static_cast<void*>(NULL))){
		perror("thread creation failed\n");
	}
	//creates thread for printing to log file
	if (pthread_create(&print_thread, NULL, Print_Event, static_cast<void*>(NULL))){
		perror("thread creation failed\n");
	}

	for (int i = 0; i < ATM_num; i++){
		if (pthread_join(ATM_threads[i], NULL)){
			perror("thread join failed\n");
		}
	}
	//all actions are done so commission and log file printing threads can be stopped
	finish=true;

	delete[] ATM_threads_params;

	if (pthread_join(commission_thread, NULL)){
		perror("thread join failed\n");
	}
	if (pthread_join(print_thread, NULL)){
		perror("thread join failed\n");
	}

	return 0;
}
