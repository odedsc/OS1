/*
 * ATM.cpp
 *
 *  Created on: May 19, 2018
 *      Author: os
 */

#include "ATM.h"
#include <stdio.h>

extern Bank bank;


//********************************************
// function name: ATM (constructor)
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: None
//**************************************************************************************
ATM::ATM(int id){
	id_=id;
	if (pthread_mutex_init(&mutex_, NULL)){
		perror("mutex initialization failed\n");
	}
}

//********************************************
// function name: ATM (destructor)
// Description: destructor
// Parameters: None
// Returns: None
//**************************************************************************************

ATM::~ATM(){
	if (pthread_mutex_destroy(&mutex_)){
		perror("mutex destruction failed\n");
	}
}

//********************************************
// function name: open_account
// Description: opens new account with relevant parameters and prints a relevant message to the log file
// Parameters: id account, account's password, balance
// Returns: None
//**************************************************************************************

void ATM::open_account(int id, int password, int balance){
	Account* new_acc=new Account(id, password, balance);

	bank.write_lock();

	/*check if already exists*/
	if (bank.account_exist(id)!=NULL){
		sleep(1);
		bank.write_unlock();
		fprintf(bank.file, "Error %d: Your transaction failed - account with the same id exists\n", id_);
	}
	else{
		sleep(1);
		bank.add_account(new_acc);
		bank.write_unlock();
		fprintf(bank.file, "%d: New account id is %d with password %d and initial balance %d\n",id_, id, password, balance);
	}

}
//********************************************
// function name: turn_VIP
// Description: turns an account to a VIP account
// Parameters: id account, account's password
// Returns: None
//**************************************************************************************

void ATM::turn_VIP(int account, int password){
	bank.read_lock();
	Account* pAccount= bank.account_exist(account);
	if (pAccount==NULL || (pAccount!=NULL && pAccount->password_!=password)){
		sleep(1);
		fprintf(bank.file, "Error %d: Your transaction failed - password for account id %d is incorrect\n", id_, account);
		bank.read_unlock();
	}
	else{
		pAccount->lock();
		sleep(1);
		pAccount->turnVIP();
		pAccount->unlock();
		bank.read_unlock();
	}

}

//********************************************
// function name: deposit
// Description: deposits an amount of money to an account and prints a relevant message to the log file
// Parameters: id account, account's password, amount to be deposited
// Returns: None
//**************************************************************************************

void ATM::deposit(int account, int password, int amount){
	bank.read_lock();
	Account* pAccount= bank.account_exist(account);
	if (pAccount==NULL){
		sleep(1);
		fprintf(bank.file, "Error %d: Your transaction failed - account id %d does not exist\n", id_, account);
		bank.read_unlock();
	}
	else if (pAccount!=NULL && pAccount->password_!=password){
		sleep(1);
		fprintf(bank.file, "Error %d: Your transaction failed - password for account id %d is incorrect\n", id_, account);
		bank.read_unlock();
	}
	else{
		pAccount->lock();
		sleep(1);
		pAccount->deposit(amount);
		pAccount->unlock();
		bank.read_unlock();
		fprintf(bank.file, "%d: Account %d new balance is %d after %d $ was deposited\n", id_, account, pAccount->balance_, amount);
	}

}

//********************************************
// function name: withdraw
// Description: withdraws an amount of money from an account and prints a relevant message to the log file
// Parameters: id account, account's password, amount to be withdrawn
// Returns: None
//**************************************************************************************


void ATM::withdraw(int account, int password, int amount){
	bank.read_lock();
	Account* pAccount= bank.account_exist(account);
	if (pAccount==NULL){
		sleep(1);
		fprintf(bank.file, "Error %d: Your transaction failed - account id %d does not exist\n", id_, account);
		bank.read_unlock();
	}
	else if (pAccount!=NULL && pAccount->password_!=password){
		sleep(1);
		fprintf(bank.file, "Error %d: Your transaction failed - password for account id %d is incorrect\n", id_, account);
		bank.read_unlock();
	}
	else if (amount > pAccount->balance_){
		sleep(1);
		fprintf(bank.file, "Error %d: Your transaction failed - account id %d balance is lower than %d\n", id_, account, amount);
		bank.read_unlock();
	}
	else{
		pAccount->lock();
		sleep(1);
		pAccount->withdraw(amount);
		pAccount->unlock();
		bank.read_unlock();
		fprintf(bank.file, "%d: Account %d new balance is %d after %d $ was withdrew\n", id_, account, pAccount->balance_, amount);
	}
}

//********************************************
// function name: balance_inquiry
// Description: prints the updated balance of the account to the log file
// Parameters: id account, account's password
// Returns: None
//**************************************************************************************

void ATM::balance_inquiry(int account, int password){
	bank.read_lock();
	Account* pAccount= bank.account_exist(account);
	if (pAccount==NULL){
		sleep(1);
		fprintf(bank.file, "Error %d: Your transaction failed - account id %d does not exist\n", id_, account);
		bank.read_unlock();
	}
	else if (pAccount!=NULL && pAccount->password_!=password){
		sleep(1);
		fprintf(bank.file, "Error %d: Your transaction failed - password for account id %d is incorrect\n", id_, account);
		bank.read_unlock();
	}
	else{
		pAccount->lock();
		sleep(1);
		//pAccount->balance_inquiry();
		pAccount->unlock();
		bank.read_unlock();
		fprintf(bank.file, "%d: Account %d balance is %d\n", id_, account, pAccount->balance_);
	}
}

//********************************************
// function name: transfer
// Description: transfers an amount from one account to another and prints relevant message to the log file
// Parameters: id account, account's password, target account to be transfered, the amount to be transfered
// Returns: None
//**************************************************************************************


void ATM::transfer(int account, int password, int target_account, int amount){
	bank.read_lock();
	Account* pAccount= bank.account_exist(account);
	Account* pAccount_target=bank.account_exist(target_account);
	if (pAccount==NULL){
		sleep(1);
		fprintf(bank.file, "Error %d: Your transaction failed - account id %d does not exist\n", id_, account);
		bank.read_unlock();
	}
	else if (pAccount_target==NULL){
		sleep(1);
		fprintf(bank.file, "Error %d: Your transaction failed - account id %d does not exist\n", id_, target_account);
		bank.read_unlock();
	}
	else if (pAccount!=NULL && pAccount->password_!=password){
		sleep(1);
		fprintf(bank.file, "Error %d: Your transaction failed - password for account id %d is incorrect\n", id_, account);
		bank.read_unlock();
	}
	else if (amount > pAccount->balance_){
		sleep(1);
		fprintf(bank.file, "Error %d: Your transaction failed - account id %d balance is lower than %d\n", id_, account, amount);
		bank.read_unlock();
	}
	else{
		pAccount->lock();
		sleep(1);
		pAccount->transfer(*pAccount_target, amount);
		pAccount->unlock();
		bank.read_unlock();
		fprintf(bank.file, "%d: Transfer %d from account %d to account %d new account balance is %d new target account balance is %d\n", id_, amount, account, target_account,pAccount->balance_,pAccount_target->balance_);
	}
}

//********************************************
// function name: lock
// Description: locks the mutex of an account
// Parameters: None
// Returns: None
//**************************************************************************************

void ATM::lock(){
	pthread_mutex_lock(&mutex_);
}

//********************************************
// function name: unlock
// Description: unlocks the mutex of an account
// Parameters: None
// Returns: None
//**************************************************************************************

void ATM::unlock(){
	pthread_mutex_unlock(&mutex_);
}


