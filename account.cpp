/*
 * account.cpp
 *
 *  Created on: May 19, 2018
 *      Author: os
 */

#include "account.h"

//********************************************
// function name: Account (constructor)
// Description: constructs an account with relevant parameters(id, password and balance)
// Parameters: id account, account's password, balance
// Returns: None
//**************************************************************************************
Account::Account(int id, int password, int balance){
	id_ = id;
	password_ = password;
	balance_ = balance;
	isVIP_ = false;
	if (pthread_mutex_init(&mutex_, NULL)){
		perror("mutex initialization failed\n");
	}
}

//********************************************
// function name: Account (destructor)
// Description: destructor
// Parameters: None
// Returns: None
//**************************************************************************************
Account::~Account(){
	if (pthread_mutex_destroy(&mutex_)){
		perror("mutex destruction failed\n");
	}
}

//********************************************
// function name: lock
// Description: locks the mutex of an account
// Parameters: None
// Returns: None
//**************************************************************************************
void Account::lock(){
	pthread_mutex_lock(&mutex_);
}

//********************************************
// function name: unlock
// Description: unlocks the mutex of an account
// Parameters: None
// Returns: None
//**************************************************************************************
void Account::unlock(){
	pthread_mutex_unlock(&mutex_);
}

//********************************************
// function name: deposit
// Description: adds an amount of money to the account
// Parameters: amount of money to be added
// Returns: None
//**************************************************************************************

void Account::deposit(int amount){
	balance_+=amount;
}

//********************************************
// function name: withdraw
// Description: withdraws an amount of money from the account
// Parameters: amount of money to be withdrawn
// Returns: None
//**************************************************************************************

void Account::withdraw(int amount){
	balance_-=amount;
}

//********************************************
// function name: transfers
// Description: transfers an amount from one account to another
// Parameters: the account target which the account needs to be transfered to, the amount of money to be transfered
// Returns: None
//**************************************************************************************

void Account::transfer(Account& target, int amount){
	balance_ -= amount;
	target.balance_ += amount;
}


