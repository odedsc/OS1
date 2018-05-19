#ifndef BANK_H_
#define BANK_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <list>

#include "account.h"

class Bank
{
private:

	list<Account*> account_list;
	typedef list<Account*>::iterator account_iterator;

	pthread_mutex_t write_mutex;
	pthread_mutex_t read_mutex;
	pthread_mutex_t Bank_mutex;

	int bank_balance;

	int read_counter;

public:
	bool done;
	
	FILE* plog_file;
	ATM** ATM_array;

	Bank();
	~Bank();

	void Init(int num_of_ATMs);

	void Read_Lock   ();
	void Read_Unlock ();
	void Write_Lock  ();
	void Write_Unlock();

	Account* pAccount (int account_id);
	void Add_Account   (Account*);
	void Delete_Account(Account*);

	void Commission();
	void Log       ();
};


#endif