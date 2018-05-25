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

class ATM_params{
public:
	int id;
	char* file_name;
};

void* ATM_EVENT(void* ATM_threads_params){
	ATM_params& params = *(ATM_params*)ATM_threads_params;
	ATM my_ATM(params.id);
	FILE* file=fopen(params.file_name, "r");
	char line[MAX_LINE_SIZE];
	while (fgets(line, MAX_LINE_SIZE, file)){
		char* pEnd;

		int value1 = strtol(&line[2], &pEnd, 10);
		int value2 = strtol(pEnd,     &pEnd, 10);
		int value3 = strtol(pEnd,     &pEnd, 10);
		int value4 = strtol(pEnd,     &pEnd, 10);

		my_ATM.lock();

		switch (line[0])
		{
		case 'O': my_ATM.open_account(value1,value2,value3);   break;
		case 'L': my_ATM.turn_VIP(value1,value2);        	   break;
		case 'D': my_ATM.deposit(value1,value2,value3);        break;
		case 'W': my_ATM.withdraw(value1,value2,value3);       break;
		case 'B': my_ATM.balance_inquiry(value1,value2);       break;
		case 'T': my_ATM.transfer(value1,value2,value3,value4);break;
		default:  break;
		}

		my_ATM.unlock();

		usleep(100000);
	}
	pthread_exit(NULL);
}

void* Commision_Event(void*)
{
	while (finish == false)
	{
		sleep(3);
		bank.commission();
	}

	pthread_exit(NULL);
}

void* Print_Event(void*)
{
	while (finish == false)
	{
		bank.print();
		usleep(500000);
	}
	pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
	if ((argc < 3) || (atoi(argv[1]) != (argc-2)))
	{
		cout << "illegal arguments\n" << endl;
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
		if (pthread_create(&ATM_threads[i], NULL, ATM_EVENT, (void*)&(ATM_threads_params[i]))){
			perror("thread creation failed\n");
		}
	}

	if (pthread_create(&commission_thread, NULL, Commision_Event, static_cast<void*>(NULL))){
		perror("thread creation failed\n");
	}
	if (pthread_create(&print_thread, NULL, Print_Event, static_cast<void*>(NULL))){
		perror("thread creation failed\n");
	}

	for (int i = 0; i < ATM_num; i++){
		if (pthread_join(ATM_threads[i], NULL)){
			perror("thread join failed\n");
		}
	}

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
