/*
 * ATM.cpp
 *
 *  Created on: May 19, 2018
 *      Author: os
 */

#include "ATM.h"
#include <stdio.h>

extern Bank bank;

ATM::ATM(int id){
	id_=id;
	pthread_mutex_init(&mutex_, NULL);
}

ATM::~ATM(){
	pthread_mutex_destroy(&mutex_);
}

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

void ATM::turn_VIP(int account, int password){
	bank.read_lock();
	Account* pAccount= bank.account_exist(account);
	if (pAccount==NULL || (pAccount!=NULL && pAccount->password_!=password)){
		sleep(1);
		fprintf(bank.file, "Error %d: your transaction failed - password for account %d is incorrect\n", id_, account);
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

void ATM::deposit(int account, int password, int amount){
	bank.read_lock();
	Account* pAccount= bank.account_exist(account);
	if (pAccount==NULL){
		sleep(1);
		fprintf(bank.file, "Error %d: your transaction failed - account id %d does not exist\n", id_, account);
		bank.read_unlock();
	}
	else if (pAccount!=NULL && pAccount->password_!=password){
		sleep(1);
		fprintf(bank.file, "Error %d: your transaction failed - password for account %d is incorrect\n", id_, account);
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

void ATM::withdraw(int account, int password, int amount){
	bank.read_lock();
	Account* pAccount= bank.account_exist(account);
	if (pAccount==NULL){
		sleep(1);
		fprintf(bank.file, "Error %d: your transaction failed - account id %d does not exist\n", id_, account);
		bank.read_unlock();
	}
	else if (pAccount!=NULL && pAccount->password_!=password){
		sleep(1);
		fprintf(bank.file, "Error %d: your transaction failed - password for account %d is incorrect\n", id_, account);
		bank.read_unlock();
	}
	else if (amount > pAccount->balance_){
		sleep(1);
		fprintf(bank.file, "Error %d: your transaction failed - account id %d balance is lower than %d\n", id_, account, amount);
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

void ATM::balance_inquiry(int account, int password){
	bank.read_lock();
	Account* pAccount= bank.account_exist(account);
	if (pAccount==NULL){
		sleep(1);
		fprintf(bank.file, "Error %d: your transaction failed - acount id %d does not exist\n", id_, account);
		bank.read_unlock();
	}
	else if (pAccount!=NULL && pAccount->password_!=password){
		sleep(1);
		fprintf(bank.file, "Error %d: your transaction failed - password for account %d is incorrect\n", id_, account);
		bank.read_unlock();
	}
	else{
		pAccount->lock();
		sleep(1);
		pAccount->balance_inquiry();
		pAccount->unlock();
		bank.read_unlock();
		fprintf(bank.file, "%d: Account %d balance is %d\n", id_, account, pAccount->balance_);
	}
}

void ATM::transfer(int account, int password, int target_account, int amount){
	bank.read_lock();
	Account* pAccount= bank.account_exist(account);
	Account* pAccount_target=bank.account_exist(target_account);
	if (pAccount==NULL){
		sleep(1);
		fprintf(bank.file, "Error %d: your transaction failed - acount id %d does not exist\n", id_, account);
		bank.read_unlock();
	}
	else if (pAccount_target==NULL){
		sleep(1);
		fprintf(bank.file, "Error %d: your transaction failed - acount id %d does not exist\n", id_, target_account);
		bank.read_unlock();
	}
	else if (pAccount!=NULL && pAccount->password_!=password){
		sleep(1);
		fprintf(bank.file, "Error %d: your transaction failed - password for account %d is incorrect\n", id_, account);
		bank.read_unlock();
	}
	else if (amount > pAccount->balance_){
		sleep(1);
		fprintf(bank.file, "Error %d: your transaction failed - account id %d balance is lower than %d\n", id_, account, amount);
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

void ATM::lock(){
	pthread_mutex_lock(&mutex_);
}

void ATM::unlock(){
	pthread_mutex_unlock(&mutex_);
}


