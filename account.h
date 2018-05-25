/*
 * account.h
 *
 *  Created on: May 19, 2018
 *      Author: os
 */

#ifndef ACCOUNT_H_
#define ACCOUNT_H_

#include <unistd.h>
#include <pthread.h>
#include <iostream>
using namespace std;

#include "ATM.h"

//a class to Account
//Module's role: implementation of all the functions taking care of possible actions for the account

class Account{
private:
	int id_;
	pthread_mutex_t mutex_;
	friend class ATM;
	friend class Bank;

public:
	int password_;
	int balance_;
	bool isVIP_;
	Account(int id, int password, int balance);
	~Account();

	void lock();
	void unlock();

	void turnVIP() {isVIP_=true;};
	void deposit(int amount);
	void withdraw(int amount);
	void transfer(Account& target, int amount);
	int  balance_inquiry() const {return balance_;}
	int  get_id() const {return id_;}
	int  get_password() const {return password_;}

};



#endif /* ACCOUNT_H_ */
