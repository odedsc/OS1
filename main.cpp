#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <time.h>

#include "bank.h"

using namespace std;

const int MAX_LINE_SIZE = 500;

typedef void* (*Thread_Func)(void*);

/* thread create wrapper */

static void Thread_Create(pthread_t* pThread, Thread_Func Func, void* parameters)
{
	pthread_create(pThread, NULL, Func, parameters);
}

class Usage {};
ostream& operator << (ostream& cout, Usage)
{
	cout << "Bank <Number of ATMs – N> <ATM_1_input_file> <ATM_2_input_file>...<ATM_N_input_file‬‬>\n";
	return cout;
}

class ATM_Thread_Parameters
{
public:
	int   id;
	char* pIn_file;
};

/* gloabl */
Bank bank;

void* Bank_Log_Routine(void*)
{
	while (bank.done == false)
	{
		bank.Log();
		usleep(500*1000);
	}
	pthread_exit(NULL);
}

void* Bank_Commission_Routine(void*)
{
	while (bank.done == false)
	{
		bank.Commission();
		sleep(3);
	}

	pthread_exit(NULL);
}

void* Bank_Fix_ATM_Routine(void* parameters)
{
	int num_of_ATM = *(int*)parameters;

	while (bank.done == false)
	{
		bank.Fix_ATM(num_of_ATM);
		sleep(2);
	}
	pthread_exit(NULL);
}

void* ATM_Routine(void* ATM_thread_parameters)
{
	ATM_Thread_Parameters& thread_parameters = *(ATM_Thread_Parameters*)ATM_thread_parameters;

	ATM ATM(thread_parameters.id);

	FILE* pIn_file = fopen(thread_parameters.pIn_file, "r");
	char  line[MAX_LINE_SIZE];

	while (fgets(line, MAX_LINE_SIZE, pIn_file))
	{

		ATM_Parameters parameters(line);

		ATM.Lock();

		switch (parameters.action)
		{
		case Account::OPEN_ACCOUNT:    ATM.Open_Account   (parameters); break;
		case Account::DEPOSIT:         ATM.Deposit        (parameters); break;
		case Account::WITHDRAW:        ATM.Withdraw       (parameters); break;
		case Account::GET_BALANCE:     ATM.Get_Balance    (parameters); break;
		case Account::CHANGE_PASSWORD: ATM.Change_Password(parameters); break;
		case Account::TRANSFER:        ATM.Transfer       (parameters); break;
		case Account::MERGE:           ATM.Merge          (parameters); break;

		default: break;
		}

		ATM.Unlock();

		usleep((rand() % 100 + 50) * 1000);
	}

	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		cout << Usage() << endl;
		return -1;
	}

	int                num_of_ATM = atoi(argv[1]);
	pthread_t          ATM_thread[num_of_ATM];
	pthread_t          commission_thread;
	pthread_t          fix_ATM_thread;
	pthread_t          bank_log_thread;
	ATM_Thread_Parameters* pATM_thread_parameters;

	/* Init */
	srand(time(NULL));
	bank.Init(num_of_ATM);

	try {
		pATM_thread_parameters = new ATM_Thread_Parameters[num_of_ATM];
		for (int i = 0; i < num_of_ATM; i++)
		{
			pATM_thread_parameters[i].id        = i;
			pATM_thread_parameters[i].pIn_file  = argv[i+2];

			/* create thread */
			Thread_Create(&ATM_thread[i], ATM_Routine, (void*)&(pATM_thread_parameters[i]));
		}

		Thread_Create(&commission_thread, Bank_Commission_Routine, static_cast<void*>(NULL));
		Thread_Create(&fix_ATM_thread,    Bank_Fix_ATM_Routine,    static_cast<void*>(&num_of_ATM));
		Thread_Create(&bank_log_thread,   Bank_Log_Routine,        static_cast<void*>(NULL));
	}
	catch(int& err)
	{
		fprintf(stderr, "ERROR code: %d\n", err);
		return -1;
	}

	for (int i = 0; i < num_of_ATM; i++)
		pthread_join(ATM_thread[i], NULL);

	bank.done = true;
	delete[] pATM_thread_parameters;

	pthread_join(commission_thread, NULL);
	pthread_join(fix_ATM_thread,    NULL);
	pthread_join(bank_log_thread,   NULL);

	return -1;
}