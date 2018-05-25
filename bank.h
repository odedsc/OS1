/*
File name: bank.h
Module's role: declaration of all the functions taking care of checks an all the of bank actions 
Functions: 
Bank();
Bank();
void read_lock();
void write_lock();
void read_unlock();
void write_unlock();
Account* account_exist(int account);
void add_account(Account*);
void remove_account(Account*);
void commission();
void print();
*/

#ifndef BANK_H_
#define BANK_H_

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "account.h"
#include "ATM.h"

class Account;

class Bank{
private:
	vector<Account*> accounts_vec_;
	pthread_mutex_t write_lock_;
	pthread_mutex_t read_lock_;
	int rd_count_;
	int balance_;

public:
	FILE* file;

	Bank();
	~Bank();

	void read_lock();
	void write_lock();
	void read_unlock();
	void write_unlock();

	Account* account_exist(int account);

	void add_account(Account*);
	void remove_account(Account*);
	void commission();
	void print();

};



#endif /* BANK_H_ */
